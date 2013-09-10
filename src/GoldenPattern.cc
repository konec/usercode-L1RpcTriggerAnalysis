#include <functional>
#include <numeric>

#include "UserCode/L1RpcTriggerAnalysis/interface/GoldenPattern.h"

#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"

void GoldenPattern::Result::runNoCheck() const {

  float fract = 1;
  for(auto mType=myResults.cbegin();mType!=myResults.cend();++mType){    
    for (auto it=mType->second.cbegin(); it!=mType->second.cend();++it) fract *= norm(mType->first,it->second); 
  }

  unsigned int nTot = 0;
  for(auto it=nMatchedPoints.cbegin();it!=nMatchedPoints.cend();++it) nTot+=it->second;    

  theValue*=(myResults[GoldenPattern::POSRPC].size()==nMatchedPoints[GoldenPattern::POSRPC]);
  theValue*=(myResults[GoldenPattern::POSDT].size()==nMatchedPoints[GoldenPattern::POSDT]);
  theValue*=(myResults[GoldenPattern::POSCSC].size()==nMatchedPoints[GoldenPattern::POSCSC]);
  theValue = ( nTot > 2) ? pow(fract, 1./((float) nTot)) : 0.;
  //AK theValue = ( nTot > 4) ? -log(fract) : 9999.0;
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
float GoldenPattern::Result::norm(GoldenPattern::PosBenCase where, float whereInDist) const {
  float normValue = 2.*(0.5-fabs(whereInDist-0.5));   
  //normValue = 0.95*whereInDist; //AK

  static const float epsilon = 1.e-9;
  if (normValue > epsilon) ++nMatchedPoints[where];
  else normValue = 0.05;
  return normValue; 
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
bool GoldenPattern::Result::operator < (const GoldenPattern::Result &o) const {
  if ( *this && o) {
    if (nMatchedTot() < o.nMatchedTot()) return true;
    else if (nMatchedTot() == o.nMatchedTot() && value() < o.value()) return true;
    else return false; 
  } 
  else if (o) {return true; }
  else if (*this) { return false; } 
  else return false;

//  return (value() < o.value() ); 
//  return false;
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
GoldenPattern::Result::operator bool() const {
  return (value() > 0.1); // && hasStation1 && hasStation2);
}

float GoldenPattern::Result::value() const { 
  run(); 
  return theValue; 
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
unsigned int GoldenPattern::Result::nMatchedTot() const {
  run();
  unsigned int nTot = 0;
  for(auto it=nMatchedPoints.cbegin();it!=nMatchedPoints.cend();++it) nTot+=it->second;    
  return nTot;   
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &out, const GoldenPattern::Result& o)
{
  o.run();
  
  out <<"Result: "
      << " value: "<<o.theValue
      <<" (POSRPC, POSCSC, BENCSC, POSDT, BENDT)(";
  for(auto cit=o.myResults.cbegin();cit!=o.myResults.cend();++cit){
    out<<o.nMatchedPoints[cit->first]<<"/"<<cit->second.size()<<", ";
  }
  out <<"tot:"<<o.nMatchedTot()<<")";
  
  return out;
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
void GoldenPattern::add( GoldenPattern::PosBenCase aCase, uint32_t rawId, int posOrBen, unsigned int freq){
  PattCore[aCase][rawId][posOrBen] += freq; 
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
void GoldenPattern::add(const Pattern & p) {

  const Pattern::DataType & detdigi = p ;
  for (auto is = detdigi.cbegin(); is != detdigi.cend(); ++is) {
    uint32_t rawId = is->first;
    DetId detId(rawId);
    if (detId.det() != DetId::Muon) 
      std::cout << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
    switch (detId.subdetId()) {
      case MuonSubdetId::RPC: {
        RPCDigiSpec digi(rawId, is->second);
	PattCore[GoldenPattern::POSRPC][rawId][digi.halfStrip()]++;
        break;
      }
      case MuonSubdetId::DT: {
        DTphDigiSpec digi(rawId, is->second);
        if (digi.bxNum() != 0 || digi.bxCnt() != 0 || digi.ts2() != 0 ||  digi.code()<4) break;	
        PattCore[GoldenPattern::POSDT][rawId][digi.phi()]++;
	PattCore[GoldenPattern::BENDT][rawId][digi.phiB()]++;
        break;
      }
      case MuonSubdetId::CSC: {
        CSCDigiSpec digi(rawId, is->second);
        PattCore[GoldenPattern::POSCSC][rawId][digi.strip()]++;
        PattCore[GoldenPattern::BENCSC][rawId][digi.pattern()]++;
        break;
      }
      default: {std::cout <<" Unexpeced sebdet case, id ="<<is->first <<std::endl; return; }
    };
  } 
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
GoldenPattern::Result GoldenPattern::compare(const Pattern &p){
  Result result;
  const Pattern::DataType & detdigi = p;
  const std::vector<Pattern::DataType::value_type> detsHit=p.uniqueDigis();

  uint32_t  nTot = 0;
  ///Check spatial compatibility of GP with Pattern.
  ///Require at least 5 measurements in common detUnits
  for (auto is = detdigi.cbegin(); is != detdigi.cend(); ++is) {
    uint32_t rawId = is->first;    
    for(auto mType = PattCore.cbegin();mType!=PattCore.cend();++mType){
      nTot+=mType->second.count(rawId);
    }
  }
  //if(nTot<5) return result;
  /////////////////////////////////////////////////////////////////////

  SystFreq::const_iterator cit;
  DetFreq::const_iterator idm;
  PosBenCase mType;

  ///Loop over unique detIds
  for (auto aEntry : detsHit){
    uint32_t rawId = aEntry.first;
    DetId detId(rawId);
    if (detId.det() != DetId::Muon){
      std::cout << "GoldenPattern::compare PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
      return result;
    }
    
    auto aRange = detdigi.equal_range(rawId);
    auto beginIt = aRange.first;
    auto endIt =   aRange.second;
    ///Maximum measure over hits in the same detId
    float fMax = 0.0;
    ///Loop over hits in given detId
    for (auto is = beginIt; is != endIt; ++is) {
      if (detId.subdetId() == MuonSubdetId::RPC) {
	RPCDigiSpec digi(rawId, is->second);
	mType = GoldenPattern::POSRPC;
	cit = PattCore.find(mType);
	if(cit==PattCore.cend()) continue; //AK: Ugly, FIX.
	idm = cit->second.find(rawId);
	if (idm != cit->second.cend() ) {
	  //float f = whereInDistribution(digi.halfStrip(), idm->second);
	  float f = whereInDistribution(mType,rawId, digi.halfStrip());
	  if(f>fMax) fMax = f;
	  RPCDetId rpc(rawId);
	  if(rpc.station()==1) result.hasStation1 = true;
	  if(rpc.station()==2) result.hasStation2 = true;
	}
      }
      else if (detId.subdetId() == MuonSubdetId::DT) {
	DTphDigiSpec digi(rawId, is->second);
	mType = GoldenPattern::POSDT;
	cit = PattCore.find(mType);
	if(cit==PattCore.cend()) continue;
	idm = cit->second.find(rawId);
	if (idm != cit->second.cend() ) {
	  //float f = whereInDistribution(digi.phi(), idm->second);
	  float f = whereInDistribution(mType,rawId, digi.phi());
	  if(f>fMax) fMax = f;
	  DTChamberId dt(rawId);
	  if(dt.station()==1) result.hasStation1 = true;
	  if(dt.station()==2) result.hasStation2 = true;
	}
	mType = GoldenPattern::BENDT;
	cit = PattCore.find(mType);
	if(cit==PattCore.cend()) continue;
	idm = cit->second.find(rawId);
	if (idm != cit->second.cend() ) {
	  //float f = whereInDistribution(digi.phiB(), idm->second);
	  float f = whereInDistribution(mType,rawId, digi.phiB());
	  if(f>fMax) fMax = f;
	}
      }
      else if (detId.subdetId() == MuonSubdetId::CSC) {
	CSCDigiSpec digi(rawId, is->second);
	mType = GoldenPattern::POSCSC;
	cit = PattCore.find(mType);
	if(cit==PattCore.cend()) continue;
	auto idm = cit->second.find(rawId);
	if (idm != cit->second.cend() ) {
	  //float f = whereInDistribution(digi.strip(), idm->second);
	  float f = whereInDistribution(mType,rawId, digi.strip());
	  if(f>fMax) fMax = f;
	  CSCDetId csc(rawId);
	  if (csc.station()==1) result.hasStation1 = true;
	  if (csc.station()==2) result.hasStation1 = true;
	}
	mType = GoldenPattern::BENCSC;
	cit = PattCore.find(mType);
	if(cit==PattCore.cend()) continue;
	idm = cit->second.find(rawId);
	if (idm != cit->second.cend() ) {
	  //float f = whereInDistribution(digi.pattern(), idm->second);
	  float f = whereInDistribution(mType,rawId, digi.pattern());
	  if(f>fMax) fMax = f;
	}
      }
    }
    result.myResults[mType].push_back(std::make_pair(rawId, fMax));
    fMax = -999.0;
  }
  return result;
}

float GoldenPattern::whereInDistribution( int obj, const GoldenPattern::MFreq & m) const
{

  float sum_before = 0;
  float sum_after = 0;
  float sum_obj = 0;
  for (MFreq::const_iterator im = m.begin(); im != m.end(); ++im) {
    if (im->first  < obj) sum_before+= im->second;
    if (im->first == obj) sum_obj = im->second;  
    if (im->first  > obj) sum_after += im->second;
  }
  float sum = std::max((float)1.,sum_before+sum_after+sum_obj );
  //return sum_obj/sum; //AK
  return (sum_before+sum_obj/2.)/sum; 
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
float GoldenPattern::whereInDistribution(PosBenCase mType, uint32_t rawId, int pos) const{

  float freqInt = 0;
  auto detsMap = PattCoreIntegrated.find(mType);
  if(detsMap!=PattCoreIntegrated.cend()){
    auto freqMap = detsMap->second.find(rawId);
    if(freqMap!=detsMap->second.cend()){
      auto freqVal = freqMap->second.find(pos);
      if(freqVal!=freqMap->second.cend()) freqInt = freqVal->second;	    
    }
  }
  return freqInt;
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
bool GoldenPattern::purge(){

  bool remove = false;
  int pos;
  unsigned int bef2, bef1, aft1, aft2, aft3;
  for (auto isf=PattCore.begin();isf!=PattCore.end();){
    for (auto idf = isf->second.begin(); idf !=isf->second.end();) {
      for (auto imf = idf->second.begin(); imf != idf->second.end();) {
	remove = false;
	pos = imf->first;  
	bef2 = (idf->second.find(pos-2) != idf->second.end()) ?  idf->second[pos-2] : 0;  
	bef1 = (idf->second.find(pos-1) != idf->second.end()) ?  idf->second[pos-1] : 0;  
	aft1 = (idf->second.find(pos+1) != idf->second.end()) ?  idf->second[pos+1] : 0;  
	aft2 = (idf->second.find(pos+2) != idf->second.end()) ?  idf->second[pos+2] : 0;  
	aft3 = (idf->second.find(pos+3) != idf->second.end()) ?  idf->second[pos+3] : 0; 
	if (idf->second[pos]==1 && bef1==0 && aft1==0) remove = true;
	if (idf->second[pos]==1 && aft1==1 && aft2==0 && aft3==0 && bef1==0 && bef2==0)  remove = true;
	if (idf->second[pos]==2 && aft1==0 && aft2==0 && bef1==0 && bef2==0)  remove = true;
	//if(remove) std::cout<<"Cleaning pattern: "<<*this<<std::endl;      
	if(remove) {idf->second.erase(imf++); } else { ++imf; } 
      }
      if (idf->second.size()==0) isf->second.erase(idf++);  else  ++idf;
    }
      if (isf->second.size()==0) PattCore.erase(isf++);  else  ++isf;
  }
  ///////////////////
  ///Usefull pattern has at least 4 measurements and has a RPC measurement
  return PattCore.find(POSRPC)!=PattCore.end() && PattCore.size()>2;
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
void GoldenPattern::makeIntegratedCache(){

  ///////////////////
  ///Prepare tables of integrated frequencies
  PattCoreIntegrated = PattCore;
  for (auto isf=PattCoreIntegrated.begin();isf!=PattCoreIntegrated.end();++isf){
    for (auto idf = isf->second.begin(); idf !=isf->second.end();++idf) {
      float sum = 0;
      float sumTmp = 0;
      for (auto imf = idf->second.begin(); imf != idf->second.end();++imf) sum+= imf->second;  
      sumTmp = sum;
      for (auto rmf = idf->second.rbegin(); rmf != idf->second.rend();++rmf){
	sumTmp-=rmf->second;
	rmf->second=(sumTmp+rmf->second/2.0)/sum;        
      }
    }
  }
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &out, const GoldenPattern & o) {

 out <<"GoldenPattern"<< o.theKey <<std::endl;

 std::vector<std::string> typeInfos = {"POSRPC","POSCSC","BENCSC","POSDT","BENDT"};

 for (auto isf=o.PattCore.cbegin();isf!=o.PattCore.cend();++isf){
   for (auto idf = isf->second.cbegin(); idf!=isf->second.cend();++idf) {      
     out <<typeInfos[isf->first]<<" Det: "<< idf->first;
     if(typeInfos[isf->first].find("RPC")!=std::string::npos){
       RPCDetId rpc(idf->first);
       out<<" ("<<rpc<<") ";
     }
     if(typeInfos[isf->first].find("CSC")!=std::string::npos){
       CSCDetId csc(idf->first);
       out<<" ("<<csc<<") ";
     }
     if(typeInfos[isf->first].find("DT")!=std::string::npos){
       DTChamberId dt(idf->first);
       out<<" ("<<dt<<") ";
     }
     out <<" Value: ";
     for (auto imf = idf->second.cbegin(); imf != idf->second.cend();++imf) 
       { out << imf->first<<":"<<imf->second<<", "; }
     out << std::endl;
   }
 }
 return out;
}
//////////////////////////////////////////////////// 
////////////////////////////////////////////////////
