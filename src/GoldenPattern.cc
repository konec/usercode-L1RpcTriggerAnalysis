#include <iostream>
#include <functional>
#include <iterator>
#include <numeric>

#include "TCanvas.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TString.h"
#include "TPaveText.h"
#include "TFile.h"

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

  float fract = 0;
  for(auto mType=myResults.cbegin();mType!=myResults.cend();++mType){    
    for (auto it=mType->second.cbegin(); it!=mType->second.cend();++it){
      float val = norm(mType->first,it->second);
      fract += 2.0*val; 
      //std::cout<<mType->first<<" "<<2*val<<std::endl;
    }
  }

  unsigned int nTot = 0;
  for(auto it=nMatchedPoints.cbegin();it!=nMatchedPoints.cend();++it) nTot+=it->second;    
  
  //theValue = ( nTot > 3) ? fract : -99999.;  
  theValue = ( nTot > 0) ? fract : -99999.;  

}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
float GoldenPattern::Result::norm(GoldenPattern::PosBenCase where, float whereInDist) const {
  static const float epsilon = 1.e-6;
  float normValue = whereInDist;  
  if(normValue > log(epsilon)) ++nMatchedPoints[where];
  else normValue = log(epsilon);
  /////////////////
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
  return(value()>-99999 && nMatchedTot()>1);
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
void GoldenPattern::add(const Pattern & p,  MtfCoordinateConverter *myPhiConverter) {

  int nPhi = this->theKey.nPhi(this->theKey.theDet);

  const Pattern::DataType & detdigi = p ;
  for (auto is = detdigi.cbegin(); is != detdigi.cend(); ++is) {
    uint32_t rawId = is->second.first;
    DetId detId(rawId);
    if (detId.det() != DetId::Muon) 
      std::cout << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
    switch (detId.subdetId()) {
      case MuonSubdetId::RPC: {
        RPCDigiSpec digi(rawId, is->second.second);
	PattCore[GoldenPattern::POSRPC][myPhiConverter->getLayerNumber(rawId)][myPhiConverter->convert(is->second,nPhi)]++;
        break;
      }	
      case MuonSubdetId::DT: {
        DTphDigiSpec digi(rawId, is->second.second);
        PattCore[GoldenPattern::POSDT][myPhiConverter->getLayerNumber(rawId)][myPhiConverter->convert(is->second,nPhi)]++;
	PattCore[GoldenPattern::BENDT][myPhiConverter->getLayerNumber(rawId)][digi.phiB()]++;
        break;
      }
      case MuonSubdetId::CSC: {
        CSCDigiSpec digi(rawId, is->second.second);
        PattCore[GoldenPattern::POSCSC][myPhiConverter->getLayerNumber(rawId)][myPhiConverter->convert(is->second,nPhi)]++;
        PattCore[GoldenPattern::BENCSC][myPhiConverter->getLayerNumber(rawId)][digi.pattern()]++;
        break;
      }
      default: {std::cout <<" Unexpeced subdet case, id ="<<is->first <<std::endl; return; }
    };
  } 
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
GoldenPattern::Result GoldenPattern::compare(const Pattern &p,  MtfCoordinateConverter *myPhiConverter){

  Result result;
  const Pattern::DataType & detdigi = p;
  const std::vector<Pattern::DataType::value_type> detsHit=p.uniqueDigis();
  
  SystFreq::const_iterator cit;
  DetFreq::const_iterator idm;
  PosBenCase mType;

  int nPhi = this->theKey.nPhi(this->theKey.theDet);
    
  ///Loop over unique detIds
  for (auto aEntry : detsHit){
    uint32_t rawId = aEntry.second.first;
    uint32_t layer = aEntry.first;
    DetId detId(rawId);
    if (detId.det() != DetId::Muon){
      std::cout << "GoldenPattern::compare PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
      return result;
    }
    
    auto aRange = detdigi.equal_range(layer);
    auto beginIt = aRange.first;
    auto endIt =   aRange.second;

    ///Maximum measure over hits in the same detId
    float fMax = -30.0;
    float fPosMax = -30.0;
    float fPos=-30.0, fBen=-30.0;
    ///Loop over hits in given detId

    std::cout.precision(15);

    ////////////////
    ////////////////
    /*
    float fDev=-30, fDevMax = -30.0;
    mType = GoldenPattern::TOTDEV;
    cit = PattCoreIntegrated.find(mType);
    if(cit==PattCoreIntegrated.cend()) continue; //AK: Ugly, FIX.
    idm = cit->second.find(501);
    if (idm != cit->second.cend() ) {
      fDev = whereInDistribution(mType,
				 501,
				 p.deviationSum(myPhiConverter,nPhi));
      
      std::cout<<" layer: "<<501
	       <<" pos rel: "<<p.deviationSum(myPhiConverter,nPhi)
	       <<" TOTDEV  f: "<<fPos<<std::endl;      
      
    }
    if(fDev>fDevMax) fDevMax = fDev;
    if(mType==GoldenPattern::TOTDEV)
      result.myResults[GoldenPattern::TOTDEV].push_back(std::make_pair(1, fDevMax));
    ////////////////
    if(fDevMax<-29) return result;
    ////////////////
    fDevMax = -30.0;
    */
    for (auto is = beginIt; is != endIt; ++is) {
      if (detId.subdetId() == MuonSubdetId::RPC) {
	RPCDigiSpec digi(rawId, is->second.second);
	if(rawId==theKey.theDet && digi.halfStrip()==theKey.theRefStrip) result.hasRefStation = true;
	mType = GoldenPattern::POSRPC;
	cit = PattCoreIntegrated.find(mType);
	if(cit==PattCoreIntegrated.cend()) continue; //AK: Ugly, FIX.
	idm = cit->second.find(myPhiConverter->getLayerNumber(rawId));
	if (idm != cit->second.cend() ) {
	  fPos = whereInDistribution(mType,
				     myPhiConverter->getLayerNumber(rawId),
				     myPhiConverter->convert(is->second,nPhi));
	  /*	  	  
	  std::cout<<digi<<"layer: "<<myPhiConverter->getLayerNumber(rawId)
		   <<" pos rel: "<<myPhiConverter->convert(is->second)
		   <<" RPC  f: "<<fPos<<std::endl;
	  */
	  if(fPos>fPosMax) fPosMax = fPos;
	  RPCDetId rpc(rawId);
	  if(rpc.station()==1) result.hasStation1 = true;
	  if(rpc.station()==2) result.hasStation2 = true;
	  if(rpc.station()==3) result.hasStation3 = true;
	}
      }
      else if (detId.subdetId() == MuonSubdetId::DT) {
	DTphDigiSpec digi(rawId, is->second.second);
	mType = GoldenPattern::POSDT;
	cit = PattCoreIntegrated.find(mType);
	if(cit==PattCoreIntegrated.cend()) continue;
	idm = cit->second.find(myPhiConverter->getLayerNumber(rawId));
	if (idm != cit->second.cend() ) {
	  fPos = whereInDistribution(mType, 
				     myPhiConverter->getLayerNumber(rawId),
				     myPhiConverter->convert(is->second,nPhi));
	  /*
	  std::cout<<digi<<"layer: "<<myPhiConverter->getLayerNumber(rawId)
		   <<" pos rel: "<<myPhiConverter->convert(is->second)
		   <<" DT  f: "<<fPos<<std::endl;
	  */
	  DTChamberId dt(rawId);
	  if(dt.station()==1) result.hasStation1 = true;
	  if(dt.station()==2) result.hasStation2 = true;
	  if(dt.station()==3) result.hasStation3 = true;
	}
	mType = GoldenPattern::BENDT;
	cit = PattCoreIntegrated.find(mType);
	if(cit==PattCoreIntegrated.cend()) continue;
	idm = cit->second.find(myPhiConverter->getLayerNumber(rawId));
	if (idm != cit->second.cend() ) {
	  fBen = whereInDistribution(mType, myPhiConverter->getLayerNumber(rawId), digi.phiB());
	  //std::cout<<digi<<" DT bend f: "<<fBen<<std::endl;
	}
	if(fPos+fBen>fMax && fBen>-30 && fPos>-30){
	  fMax = fPos+fBen;
	  fPosMax = fPos+fBen;
	}
      }
      else if (detId.subdetId() == MuonSubdetId::CSC) {
	CSCDigiSpec digi(rawId, is->second.second);
	mType = GoldenPattern::POSCSC;
	cit = PattCoreIntegrated.find(mType);
	if(cit==PattCoreIntegrated.cend()) continue;
	auto idm = cit->second.find(myPhiConverter->getLayerNumber(rawId));
	if (idm != cit->second.cend() ) {
	  fPos = whereInDistribution(mType, 
				     myPhiConverter->getLayerNumber(rawId),
				     myPhiConverter->convert(is->second,nPhi));
	  /*
	  std::cout<<digi<<"layer: "<<myPhiConverter->getLayerNumber(rawId)
		   <<" pos rel: "<<myPhiConverter->convert(is->second)
		   <<" CSC  f: "<<fPos<<std::endl;
	  */	  
	  CSCDetId csc(rawId);
	  if (csc.station()==1) result.hasStation1 = true;
	  if (csc.station()==2) result.hasStation2 = true;
	  if (csc.station()==3) result.hasStation3 = true;
	}
	mType = GoldenPattern::BENCSC;
	cit = PattCoreIntegrated.find(mType);
	if(cit==PattCoreIntegrated.cend()) continue;
	idm = cit->second.find(myPhiConverter->getLayerNumber(rawId));
	if (idm != cit->second.cend() ) {
	  fBen = whereInDistribution(mType,
				     myPhiConverter->getLayerNumber(rawId), 
				     digi.pattern());
	  //std::cout<<digi<<" CSC bend: "<<digi.pattern()
	  // <<" f: "<<fBen<<std::endl;
	}
	if(fPos+fBen>fMax && fBen>-30 && fPos>-30){
	  fMax = fPos+fBen;
	  fPosMax = fPos+fBen;
	}
      }
    }
    if(mType==GoldenPattern::BENCSC){
      result.myResults[GoldenPattern::POSCSC].push_back(std::make_pair(rawId, fPosMax));
    }
    if(mType==GoldenPattern::BENDT){
      result.myResults[GoldenPattern::POSDT].push_back(std::make_pair(rawId, fPosMax));
    }
    if(mType==GoldenPattern::POSRPC)
      result.myResults[GoldenPattern::POSRPC].push_back(std::make_pair(rawId, fPosMax));

    fMax = -30.0;
    fPosMax = -30.0;
  }

  result.run();
  return result;
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
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
  return (sum_before+sum_obj/2.)/sum; 
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
float GoldenPattern::whereInDistribution(PosBenCase mType, uint32_t rawId, int pos) const{

  float freqInt = -30;
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

  int refSum = theKey.theRefStrip;

  for (auto isf=PattCore.begin();isf!=PattCore.end();){
    for (auto idf = isf->second.begin(); idf !=isf->second.end();) {
      int sum=0;
      for (auto imf = idf->second.begin(); imf != idf->second.end();) {
	remove = false;
	pos = imf->first;  
	sum += idf->second[pos];
 	if (idf->second[pos]/refSum<1E-3) remove = true;
	if(remove) {idf->second.erase(imf++); } else { ++imf; } 	
      }
      if (idf->second.size()==0 || (float)sum/refSum<-0.05) 
	isf->second.erase(idf++);  else  ++idf;
    }
      if (isf->second.size()==0) PattCore.erase(isf++);  else  ++isf;
  }
  ///////////////////  
  return PattCore.size();
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
void GoldenPattern::makeIntegratedCache(){

  if(hasIntegratedCache) return;
  else hasIntegratedCache = true;

  aPhiOffset = -999;
  int max = 0;
  int commonRef = GoldenPattern::PosBenCase::POSDT;
  if( (theKey.theDet>200 && theKey.theDet<300) || theKey.theDet%100>10) commonRef = GoldenPattern::PosBenCase::POSCSC;

  int refSum = theKey.theRefStrip;
  int nBits = 6;
  float minPlog = log(1E-3);
  ///////////////////
  ///Prepare tables of integrated frequencies
  PattCoreIntegrated = PattCore;
  for (auto isf=PattCoreIntegrated.begin();isf!=PattCoreIntegrated.end();++isf){
    for (auto idf = isf->second.begin(); idf !=isf->second.end();++idf) {
      float sum = 0;
      for (auto imf = idf->second.begin(); imf != idf->second.end();++imf) sum+= imf->second;  
      //refSum = sum;
      for (auto rmf = idf->second.rbegin(); rmf != idf->second.rend();++rmf){
	///Find offset wrt. DT/CSC reference 
	//if(theKey.theDet==301){
	// std::cout<<commonRef<<" "<<isf->first<<" "<<idf->first<<" "<<rmf->second<<" "<<max<<std::endl;
	//}
	if(isf->first==commonRef && idf->first==2 && rmf->second>max){
	  max = rmf->second;
	  aPhiOffset =  rmf->first;
	}
	//if((rmf->second)/refSum>1.0 || (rmf->second)/refSum<0) std::cout<<"Normalisation problem: "<<rmf->second<<" "<<refSum<<std::endl<<*this<<std::endl;
	float val=0;	
	///Distance from mean        
        //sum-= rmf->second/2.0; 
	//val = sum/refSum;
	//val = 2.*(0.5-fabs(val-0.5)); 
	////////////////////
	///Plain pdf
        val = log(rmf->second/refSum);	      
	///Digitisation
	int digitisedVal = (val/minPlog)*std::pow(2,nBits);
	rmf->second= -digitisedVal/std::pow(2,nBits);	
	//rmf->second = val;
      }
    }
  }
  /*
  std::cout<<"det: "<<theKey.theDet
	   <<" pt: "<<theKey.thePtCode
	   <<" eta: "<<theKey.theEtaCode
	   <<" offset: "<<aPhiOffset<<std::endl;
  */
  PattCore.clear();
  //////////////
}
////////////////////////////////////////////////////
////////////////////////////////////////////////////
std::ostream & operator << (std::ostream &out, const GoldenPattern & o) {

 out <<"GoldenPattern "<< o.theKey <<std::endl;

 std::vector<std::string> typeInfos = {"POSRPC","POSCSC","BENCSC","POSDT","BENDT","TOTDEV"};

 for (auto isf=o.PattCoreIntegrated.cbegin();isf!=o.PattCoreIntegrated.cend();++isf){
   for (auto idf = isf->second.cbegin(); idf!=isf->second.cend();++idf) {      
     out <<typeInfos[isf->first]<<" Det: "<< idf->first;    
     float aSum = 0.0;
     out <<" Value: ";     
     out.precision(5);
     for (auto imf = idf->second.cbegin(); imf != idf->second.cend();++imf) 
       {out << imf->first<<":"<<exp(-imf->second*log(1E-3))<<", "; aSum+=exp(-imf->second*log(1E-3));}
       //{out << imf->first<<":"<<exp(imf->second)<<", "; aSum+=exp(imf->second);}
     out <<" Sum: "<<aSum;
     out << std::endl;
   }
 }
 return out;
}
//////////////////////////////////////////////////// 
////////////////////////////////////////////////////
int GoldenPattern::size(){

  int aSize = 0;
  int aChamberSize = 0;

  for (auto isf=PattCoreIntegrated.cbegin();isf!=PattCoreIntegrated.cend();++isf){
    for (auto idf = isf->second.cbegin(); idf!=isf->second.cend();++idf){
      aChamberSize+=1;
      for (auto imf = idf->second.cbegin(); imf != idf->second.cend();++imf){
	aSize+=(imf->second>0);
      }
    }
  }
  return aChamberSize;
}
//////////////////////////////////////////////////// 
////////////////////////////////////////////////////
void GoldenPattern::plot(){

 makeIntegratedCache();
 std::vector<std::string> typeInfos = {"POSRPC","POSCSC","BENCSC","POSDT","BENDT","TOTDEV"};

 std::map<unsigned int, TGraph *> graphsRPCMap;
 std::map<unsigned int, TGraph *> graphsDTMap, graphsBENDTMap; 
 std::map<unsigned int, TGraph *> graphsCSCMap, graphsBENCSCMap;
 std::map<unsigned int, TGraph *> graphsDevMap;

 std::map<unsigned int, TGraph *>  *aMap;

 for (auto isf=PattCoreIntegrated.cbegin();isf!=PattCoreIntegrated.cend();++isf){   
   if(typeInfos[isf->first].find("RPC")!=std::string::npos) aMap = &graphsRPCMap;
   else if(typeInfos[isf->first].find("POSCSC")!=std::string::npos) aMap = &graphsCSCMap;
   else if(typeInfos[isf->first].find("POSDT")!=std::string::npos) aMap = &graphsDTMap;
   else if(typeInfos[isf->first].find("BENCSC")!=std::string::npos) aMap = &graphsBENCSCMap;
   else if(typeInfos[isf->first].find("BENDT")!=std::string::npos) aMap = &graphsBENDTMap;
   else if(typeInfos[isf->first].find("TOTDEV")!=std::string::npos) aMap = &graphsDevMap;
   else continue;

   for (auto idf = isf->second.cbegin(); idf!=isf->second.cend();++idf) {      
     TGraph *gr = new TGraph();    
     gr->SetLineWidth(3);
     (*aMap)[idf->first] = gr;
     for (auto imf = idf->second.cbegin(); imf != idf->second.cend();++imf){  
       Double_t x = imf->first;
       Double_t y = exp(-imf->second*log(1E-3));
       gr->SetPoint(gr->GetN(),x,y);
     }
   }
 }

 TFile file("GoldenPatterns.root","UPDATE");
 //////////////
 TCanvas* cRPC = new TCanvas(TString::Format("RPCtower%d_ref%d_pt%d_sign%d",theKey.theEtaCode, theKey.theDet, theKey.thePtCode, theKey.theCharge+1).Data(),
			  TString::Format("GoldenPattern").Data(),
			  600,150,1100,600);

 TCanvas* cDT = new TCanvas(TString::Format("DTtower%d_ref%d_pt%d_sign%d",theKey.theEtaCode, theKey.theDet, theKey.thePtCode, theKey.theCharge+1).Data(),
			    TString::Format("GoldenPattern").Data(),
			    600,150,1100,600);
 
 TCanvas* cCSC = new TCanvas(TString::Format("CSCtower%d_ref%d_pt%d_sign%d",theKey.theEtaCode, theKey.theDet, theKey.thePtCode, theKey.theCharge+1).Data(),
			     TString::Format("GoldenPattern").Data(),
			     600,150,1100,600);

 TCanvas* cDev = new TCanvas(TString::Format("Devtower%d_ref%d_pt%d_sign%d",theKey.theEtaCode, theKey.theDet, theKey.thePtCode, theKey.theCharge+1).Data(),
			     TString::Format("GoldenPattern").Data(),
			     600,150,1100,600);

 int iCnt = 0;
 cDev->Divide(graphsDevMap.size());
 cRPC->Divide(graphsRPCMap.size());
 cCSC->Divide(graphsCSCMap.size() + graphsBENCSCMap.size());
 cDT->Divide(graphsDTMap.size() + graphsBENDTMap.size());

 //////////////Dev
 for(auto it=graphsDevMap.begin();
     it!=graphsDevMap.end();++it){   
   iCnt++;
   cDev->cd(iCnt);   
   it->second->Sort();
   it->second->GetXaxis()->SetLabelSize(0.06);
   it->second->GetXaxis()->SetLabelOffset(-0.03);
   it->second->GetYaxis()->SetLabelSize(0.08);
   if(it->second->GetN()==1) it->second->Draw("A*");
   else it->second->Draw("AL");
   TPaveText *pt = new TPaveText(0.25,0.9,0.75,0.95,"blNDC");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextSize(0.1);
   pt->AddText(TString::Format("Dev layer: %d",it->first));
   pt->Draw();
   it->second->SetName(TString::Format("Dev_layer_%d",it->first));
 }
 cDev->Write();
 //////////////RPC
 for(auto it=graphsRPCMap.begin();
     it!=graphsRPCMap.end();++it){   
   iCnt++;
   cRPC->cd(iCnt);   
   it->second->Sort();
   it->second->GetXaxis()->SetLabelSize(0.06);
   it->second->GetXaxis()->SetLabelOffset(-0.03);
   it->second->GetYaxis()->SetLabelSize(0.08);
   if(it->second->GetN()==1) it->second->Draw("A*");
   else it->second->Draw("AL");
   TPaveText *pt = new TPaveText(0.25,0.9,0.75,0.95,"blNDC");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextSize(0.1);
   pt->AddText(TString::Format("RPC layer: %d",it->first));
   pt->Draw();
   it->second->SetName(TString::Format("RPC_layer_%d",it->first));
 }
 cRPC->Write();
 //////////////DT
 iCnt = 0;
 for(auto it=graphsDTMap.begin();
     it!=graphsDTMap.end();++it){   
   iCnt++;
   cDT->cd(iCnt);   
   it->second->Sort();
   it->second->GetXaxis()->SetLabelSize(0.06);
   it->second->GetXaxis()->SetLabelOffset(-0.03);
   it->second->GetYaxis()->SetLabelSize(0.08);
   if(it->second->GetN()==1) it->second->Draw("A*");
   else it->second->Draw("AL");
   TPaveText *pt = new TPaveText(0.25,0.9,0.75,0.95,"blNDC");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextSize(0.1);
   pt->AddText(TString::Format("DT position layer: %d",it->first));
   pt->Draw();
   it->second->SetName(TString::Format("DTPOS_layer_%d",it->first));
 }

 for(auto it=graphsBENDTMap.begin();
     it!=graphsBENDTMap.end();++it){   
   iCnt++;
   cDT->cd(iCnt);   
   it->second->Sort();
   it->second->GetXaxis()->SetLabelSize(0.06);
   it->second->GetXaxis()->SetLabelOffset(-0.03);
   it->second->GetYaxis()->SetLabelSize(0.08);
   if(it->second->GetN()==1) it->second->Draw("A*");
   else it->second->Draw("AL");
   TPaveText *pt = new TPaveText(0.25,0.9,0.75,0.95,"blNDC");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextSize(0.1);
   pt->AddText("BEN");
   pt->AddLine(.0,.5,1.,.5);
   pt->AddText(TString::Format("DT bending layer: %d",it->first));
   pt->Draw();
   it->second->SetName(TString::Format("DTBEN_layer_%d",it->first));
 }
 cDT->Write();

//////////////CSC
 iCnt = 0;
 for(auto it=graphsCSCMap.begin();
     it!=graphsCSCMap.end();++it){   
   iCnt++;
   cCSC->cd(iCnt);   
   it->second->Sort();
   it->second->GetXaxis()->SetLabelSize(0.06);
   it->second->GetXaxis()->SetLabelOffset(-0.03);
   it->second->GetYaxis()->SetLabelSize(0.08);
   if(it->second->GetN()==1) it->second->Draw("A*");
   else it->second->Draw("AL");
   TPaveText *pt = new TPaveText(0.25,0.9,0.75,0.95,"blNDC");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextSize(0.1);
   pt->AddText(TString::Format("CSC position layer: %d",it->first));
   pt->Draw();
   it->second->SetName(TString::Format("CSCPOS_layer_%d",it->first));
 }
 for(auto it=graphsBENCSCMap.begin();
     it!=graphsBENCSCMap.end();++it){   
   iCnt++;
   cCSC->cd(iCnt);   
   it->second->Sort();
   it->second->GetXaxis()->SetLabelSize(0.06);
   it->second->GetXaxis()->SetLabelOffset(-0.03);
   it->second->GetYaxis()->SetLabelSize(0.08);
   if(it->second->GetN()==1) it->second->Draw("A*");
   else it->second->Draw("AL");
   TPaveText *pt = new TPaveText(0.25,0.9,0.75,0.95,"blNDC");
   pt->SetBorderSize(0);
   pt->SetFillColor(0);
   pt->SetFillStyle(0);
   pt->SetTextSize(0.1);
   pt->AddText("BEN");
   pt->AddLine(.0,.5,1.,.5);
   pt->AddText(TString::Format("CSC bending layer: %d",it->first));
   pt->Draw();
   it->second->SetName(TString::Format("CSCBEN_layer_%d",it->first));
 }
 cCSC->Write();

 delete cRPC;
 delete cDT;
 delete cCSC;
}
