#include "UserCode/L1RpcTriggerAnalysis/interface/GoldenPattern.h"

#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"


void GoldenPattern::Result::runNoCheck() const 
{
  double fract = 1;
  for (auto i=posRpcResult.begin(); i!=posRpcResult.end();i++) fract *= norm(POSRPC,i->second); 
  for (auto i=posCscResult.begin(); i!=posCscResult.end();i++) fract *= norm(POSCSC,i->second); 
  for (auto i=posDtResult.begin();  i!=posDtResult.end();i++)  fract *= norm(POSDT, i->second);
  for (auto i=benCscResult.begin(); i!=benCscResult.end();i++) fract *= norm(BENCSC,i->second);
  for (auto i=benDtResult.begin();  i!=benDtResult.end();i++)  fract *= norm(BENDT, i->second);
  unsigned int nTot = nMatchedPosRpc+nMatchedPosCsc+nMatchedPosDt+nMatchedBenCsc+nMatchedBenDt;
  theValue = ( nTot > 4) ? pow(fract, 1./((double) nTot)) : 0.;
  if (posRpcResult.size() != nMatchedPosRpc) theValue = 0.;
  if (posCscResult.size() != nMatchedPosCsc) theValue = 0.;
  if (posDtResult.size()  != nMatchedPosDt)  theValue = 0.;
}

double GoldenPattern::Result::norm(GoldenPattern::PosBenCase where, double whereInDist) const {
  double normValue = 2.*(0.5-fabs(whereInDist-0.5));   
  static const double epsilon = 1.e-9;
  if (normValue > epsilon) {
    switch (where) {
      case POSRPC : nMatchedPosRpc++; break;
      case POSCSC : nMatchedPosCsc++; break;
      case BENCSC : nMatchedBenCsc++; break;
      case POSDT  : nMatchedPosDt++;  break;
      case BENDT  : nMatchedBenDt++;  break;
    };
  } else {
    normValue =1.;
  }
  return normValue; 
}

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

GoldenPattern::Result::operator bool() const {
  return (value() > 0.1); // && hasStation1 && hasStation2);
}

double GoldenPattern::Result::value() const { 
  run(); 
  return theValue; 
}

unsigned int GoldenPattern::Result::nMatchedTot() const {
  run();
  return nMatchedPosRpc+nMatchedPosCsc+nMatchedBenCsc+nMatchedPosDt+nMatchedBenDt;
}

std::ostream & operator << (std::ostream &out, const GoldenPattern::Result& o)
{
  o.run();
  out <<"Result: "
      << " value: "<<o.theValue
      <<" nPos+Ben: ("<<o.nMatchedPosCsc<<"/"<<o.posCscResult.size()<<"+"<<o.nMatchedBenCsc<<"/"<<o.benCscResult.size()
                <<", "<<o.nMatchedPosDt <<"/"<<o.posDtResult.size()<<"+"<<o.nMatchedBenDt<<"/"<<o.benDtResult.size()
                <<", "<<o.nMatchedPosRpc<<"/"<<o.posRpcResult.size()<<", tot:"<<o.nMatchedTot()<<")";
  return out;
}



void GoldenPattern::add( GoldenPattern::PosBenCase aCase, uint32_t rawId, int posOrBen, unsigned int freq)
{
  switch ( aCase ) {
    case POSRPC : posRpc[rawId][posOrBen]     += freq; break;
    case POSCSC : posCsc[rawId][posOrBen]     += freq; break;
    case BENCSC : bendingCsc[rawId][posOrBen] += freq; break;
    case POSDT  : posDt[rawId][posOrBen]      += freq; break;
    case BENDT  : bendingDt[rawId][posOrBen]  += freq; break;
  };
}


void GoldenPattern::add(const Pattern & p) 
{
  const Pattern::DataType & detdigi = p ;
  for (Pattern::DataType::const_iterator is = detdigi.begin(); is != detdigi.end(); ++is) {
    uint32_t rawId = is->first;
    DetId detId(rawId);
    if (detId.det() != DetId::Muon) std::cout << "PROBLEM ;;;"<<std::endl;
    switch (detId.subdetId()) {
      case MuonSubdetId::RPC: {
        RPCDigiSpec digi(rawId, is->second);
        posRpc[rawId][digi.halfStrip()]++;
        break;
      }
      case MuonSubdetId::DT: {
        DTphDigiSpec digi(rawId, is->second);
        if (digi.bxNum() != 0 || digi.bxCnt() != 0 || digi.ts2() != 0) break;
        posDt[rawId][digi.phi()]++;
        bendingDt[rawId][digi.phiB()]++;
        break;
      }
      case MuonSubdetId::CSC: {
        CSCDigiSpec digi(rawId, is->second);
        posCsc[rawId][digi.strip()]++;
        bendingCsc[rawId][digi.pattern()]++;
        break;
      }
      default: {std::cout <<" Unexpeced sebdet case, id ="<<is->first <<std::endl; return; }
    };
  } 
}

GoldenPattern::Result GoldenPattern::compare(const Pattern &p) const 
{
  Result result;
  const Pattern::DataType & detdigi = p;
  for (Pattern::DataType::const_iterator is = detdigi.begin(); is != detdigi.end(); ++is) {
    uint32_t rawId = is->first;
    DetId detId(rawId);
    if (detId.det() != DetId::Muon) std::cout << "PROBLEM ;;;"<<std::endl;
    if (detId.subdetId() == MuonSubdetId::RPC) {
      RPCDigiSpec digi(rawId, is->second);
      DetFreq::const_iterator idm = posRpc.find(rawId);
      if (idm != posRpc.end() ) {
        double f = whereInDistribution(digi.halfStrip(), idm->second);
        result.posRpcResult.push_back( std::make_pair(rawId, f) ); 
        RPCDetId rpc(rawId);
        if(rpc.station()==1) result.hasStation1 = true;
        if(rpc.station()==2) result.hasStation2 = true;
      }
    } else if (detId.subdetId() == MuonSubdetId::DT) {
      DTphDigiSpec digi(rawId, is->second);
      DetFreq::const_iterator idm = posDt.find(rawId);
      if (idm != posDt.end() ) {
        double f = whereInDistribution(digi.phi(), idm->second);
        result.posDtResult.push_back( std::make_pair(rawId, f) );
        DTChamberId dt(rawId);
        if(dt.station()==1) result.hasStation1 = true;
        if(dt.station()==2) result.hasStation2 = true;
      }
      idm = bendingDt.find(rawId);
      if (idm != bendingDt.end() ) {
        double f = whereInDistribution(digi.phiB(), idm->second);
        result.benDtResult.push_back( std::make_pair(rawId, f) );
      }
    } else if (detId.subdetId() == MuonSubdetId::CSC) {
      CSCDigiSpec digi(rawId, is->second);
      DetFreq::const_iterator idm = posCsc.find(rawId);
      if (idm != posCsc.end() ) {
        double f = whereInDistribution(digi.strip(), idm->second);
        result.posCscResult.push_back( std::make_pair(rawId, f) );
        CSCDetId csc(rawId);
        if (csc.station()==1) result.hasStation1 = true;
        if (csc.station()==2) result.hasStation1 = true;
      }
      idm = bendingCsc.find(rawId);
      if (idm != bendingCsc.end() ) {
        double f = whereInDistribution(digi.pattern(), idm->second);
        result.benCscResult.push_back( std::make_pair(rawId, f) );
      }
    }
  }
  return result;
}

double GoldenPattern::whereInDistribution( int obj, const GoldenPattern::MFreq & m) const
{
  double sum_before = 0;
  double sum_after = 0;
  double sum_obj = 0;
  for (MFreq::const_iterator im = m.begin(); im != m.end(); im++) {
    if (im->first  < obj) sum_before+= im->second;
    if (im->first == obj) sum_obj = im->second;  
    if (im->first  > obj) sum_after += im->second;
  }
  double sum = std::max(1.,sum_before+sum_after+sum_obj );
  return (sum_before+sum_obj/2.)/sum; 
}

void GoldenPattern::purge()
{
  for (DetFreq::iterator idf=posRpc.begin(); idf != posRpc.end(); idf++) {
    MFreq & mfreq = idf->second;
    MFreq::iterator imf =  mfreq.begin();
    while (imf != mfreq.end() ) {
      bool remove = false;
      int pos = imf->first;  
      unsigned int bef2 = (mfreq.find(pos-2) != mfreq.end()) ?  mfreq[pos-2] : 0;  
      unsigned int bef1 = (mfreq.find(pos-1) != mfreq.end()) ?  mfreq[pos-1] : 0;  
      unsigned int aft1 = (mfreq.find(pos+1) != mfreq.end()) ?  mfreq[pos+1] : 0;  
      unsigned int aft2 = (mfreq.find(pos+2) != mfreq.end()) ?  mfreq[pos+2] : 0;  
      unsigned int aft3 = (mfreq.find(pos+3) != mfreq.end()) ?  mfreq[pos+3] : 0;  
      if (mfreq[pos]==1 && bef1==0 && aft1==0) remove = true;
      if (mfreq[pos]==1 && aft1==1 && aft2==0 && aft3==0 && bef1==0 && bef2==0)  remove = true;
      if (mfreq[pos]==2 && aft1==0 && aft2==0 && bef1==0 && bef2==0)  remove = true;
      if (remove) { mfreq.erase(imf++); } else { ++imf; }
    }
  }
  DetFreq::iterator idf=posRpc.begin();  
  while (idf != posRpc.end()) if (idf->second.size()==0) posRpc.erase(idf++);  else  ++idf; 
 
}

std::ostream & operator << (std::ostream &out, const GoldenPattern & o) {
   out <<"GoldenPattern"<< o.theKey <<std::endl;
   // RPC
   for (GoldenPattern::DetFreq::const_iterator im = o.posRpc.begin(); im != o.posRpc.end(); im++) {
     out <<"RPC Det:"<< im->first<<" Pos: ";
     for (GoldenPattern::MFreq::const_iterator it = im->second.begin(); it !=  im->second.end(); it++) { out << it->first<<":"<<it->second<<", "; }
     out << std::endl;
   }
   // DT pos
   for (GoldenPattern::DetFreq::const_iterator im = o.posDt.begin(); im != o.posDt.end(); im++) {
     out <<"DT  Det:"<< im->first<<" Pos: ";
     for (GoldenPattern::MFreq::const_iterator it = im->second.begin(); it !=  im->second.end(); it++) { out << it->first<<":"<<it->second<<", "; }
     out << std::endl;
   }
   // DT bending 
   for (GoldenPattern::DetFreq::const_iterator im = o.bendingDt.begin(); im != o.bendingDt.end(); im++) {
     out <<"DT  Det:"<< im->first<<" Ben: ";
     for (GoldenPattern::MFreq::const_iterator it = im->second.begin(); it !=  im->second.end(); it++) { out << it->first<<":"<<it->second<<", "; }
     out << std::endl;
   }

   // Csc pos
   for (GoldenPattern::DetFreq::const_iterator im = o.posCsc.begin(); im != o.posCsc.end(); im++) {
     out <<"Csc  Det:"<< im->first<<" Pos: ";
     for (GoldenPattern::MFreq::const_iterator it = im->second.begin(); it !=  im->second.end(); it++) { out << it->first<<":"<<it->second<<", "; }
     out << std::endl;
   }
   // Csc bending 
   for (GoldenPattern::DetFreq::const_iterator im = o.bendingCsc.begin(); im != o.bendingCsc.end(); im++) {
     out <<"Csc  Det:"<< im->first<<" Ben: ";
     for (GoldenPattern::MFreq::const_iterator it = im->second.begin(); it !=  im->second.end(); it++) { out << it->first<<":"<<it->second<<", "; }
     out << std::endl;
   }

   return out;
  }

