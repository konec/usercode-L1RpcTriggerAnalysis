#include "UserCode/L1RpcTriggerAnalysis/interface/AnaDigiSpec.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"

#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"

#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TGraphErrors.h"

#include <cmath>
#include <algorithm>

template <class T> T sqr( T t) {return t*t;}
namespace {
  TH2D *hDigiSpec_PosCorr_DT2, *hDigiSpec_PosCorr_CSC2, *hDigiSpec_PosCorr_RPC2;
  
}
void AnaDigiSpec::init(TObjArray& histos)
{
  hDigiSpec_PosCorr_DT2  = new TH2D("hDigiSpec_PosCorr_DT2", "hDigiSpec_PosCorr_DT2", 100,0.8,1.2, 100, -2000.,2000.);
  histos.Add(hDigiSpec_PosCorr_DT2);
  hDigiSpec_PosCorr_RPC2 = new TH2D("hDigiSpec_PosCorr_RPC2","hDigiSpec_PosCorr_RPC2",100,0.8,1.2, 191, 2.,193.);
  histos.Add(hDigiSpec_PosCorr_RPC2);
  hDigiSpec_PosCorr_CSC2 = new TH2D("hDigiSpec_PosCorr_CSC2","hDigiSpec_PosCorr_CSC2",100,0.8,1.2, 160, 0.,159.);
  histos.Add(hDigiSpec_PosCorr_CSC2);
}

void  AnaDigiSpec::run(const EventObj* ev, const TrackObj * simu, const HitSpecObj * hitSpec,  const VDigiSpec & vdigSpec)
{
//
// position correlation at station 2 wrt RPC SimHit
// skip events with not RPC simhit in proper place
//
  if (!hitSpec) return;
  uint32_t hitRawId = hitSpec->rawId();
  if (!hitRawId) return;
//  if (hitSpec->position().phi() < 0.6 && hitSpec->position().phi() > 1.20) return;
  if (hitRawId != 637602109 && hitRawId != 637634877 &&  
//      hitRawId != 637620266 && hitRawId != 637653034 &&
      hitRawId != 637599914 && hitRawId != 637632682 ) return;
//  if (hitRawId != 637599914) return;
//  if (hitRawId != 637632682 ) return;
  RPCDetIdUtil idHit( hitRawId );
  bool barrelRefHit = idHit.isBarrel();

//
// correlation betweem sim hit position@RPC and digi
//
  for (VDigiSpec::const_iterator is= vdigSpec.begin(); is!=vdigSpec.end(); is++) {
    
    DetId detId(is->first);
    if (detId.det() != DetId::Muon) std::cout << "PROBLEM ;;;"<<std::endl;
    switch (detId.subdetId()) {
      case MuonSubdetId::RPC: {
        RPCDigiSpec digi(is->first, is->second);
        if (digi.rawId() != hitRawId) continue;
//  std::vector<uint32_t> allDetsInDigis;
//  for (VDigiSpec::const_iterator is= vdigSpec.begin(); is!=vdigSpec.end(); is++) allDetsInDigis.push_back(is->first);
//        unsigned int ndet = std::count( allDetsInDigis.begin(), allDetsInDigis.end(), hitRawId); 
//        if (digi.fromStrip() == digi.toStrip() && ndet == 1 ) 
        hDigiSpec_PosCorr_RPC2->Fill(hitSpec->position().phi(), digi.halfStrip());
        break; 
      }
      case MuonSubdetId::DT: {
//        if (!idHit.isBarrel()) break;
        DTphDigiSpec digi(is->first, is->second);
        DTChamberId dt(digi.rawId());
        if (dt.station() != 2) break;
        hDigiSpec_PosCorr_DT2->Fill(hitSpec->position().phi(), digi.phi());
        break;
      } 
      case MuonSubdetId::CSC: {
//        if (idHit.isBarrel()) break;
        CSCDigiSpec digi(is->first, is->second);
        CSCDetId csc(digi.rawId());
//        std::cout << csc << std::endl;
        if (csc.station() != 2) break;
        hDigiSpec_PosCorr_CSC2->Fill(hitSpec->position().phi(), digi.strip());
        break;
      } 
      default: {std::cout <<" Unexpeced sebdet case, id ="<<is->first <<std::endl; return; }
    };
  } 

//
// for given fixed hit postion in RPC check distributions
//
  bool precisePos = ( fabs(hitSpec->position().phi()-1.025) < 0.001); 
 // bool precisePos = ( 
 //   fabs(hitSpec->position().phi()-0.99) < 0.001 
 //   ||  fabs(hitSpec->position().phi()-0.89) < 0.001 );
    for (VDigiSpec::const_iterator is= vdigSpec.begin(); is!=vdigSpec.end(); is++) {
    
      DetId detId(is->first);
      if (detId.det() != DetId::Muon) std::cout << "PROBLEM ;;;"<<std::endl;
      switch (detId.subdetId()) {

        case MuonSubdetId::RPC: {
          RPCDigiSpec digi(is->first, is->second);
          RPCDetId     rpcId(digi.rawId());
          RPCDetIdUtil util(digi.rawId()); 
          if (util.isBarrel()) {
            if (barrelRefHit && (theRpcBPosB.find(digi.rawId())==theRpcBPosB.end()) ){
              std::stringstream str;
              str<<"hDigiSpec_RpcBVsPt_PosB_"<<digi.rawId();
              std::string hName = str.str();
              str<<"_la"<<util.layer()<<"sc"<<rpcId.sector()<<"ro"<<rpcId.roll();
              std::string hTitle = str.str();
              theRpcBPosB[digi.rawId()] = new TH2D(hName.c_str(), hTitle.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins, 191,2.,193.);
            }
            if (!barrelRefHit && (theRpcBPosE.find(digi.rawId())==theRpcBPosE.end()) ) {
              std::stringstream str;
              str<<"hDigiSpec_RpcBVsPt_PosE_"<<digi.rawId();
              std::string hName = str.str();
              str<<"_la"<<util.layer()<<"sc"<<rpcId.sector()<<"ro"<<rpcId.roll();
              std::string hTitle = str.str();
              theRpcBPosE[digi.rawId()] = new TH2D(hName.c_str(), hTitle.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins, 191,2.,193.);
            }
            if (precisePos &&  barrelRefHit) theRpcBPosB[digi.rawId()]->Fill(simu->pt(), digi.halfStrip());
            if (precisePos && !barrelRefHit) theRpcBPosE[digi.rawId()]->Fill(simu->pt(), digi.halfStrip());
          } else {
            if (barrelRefHit && (theRpcEPosB.find(digi.rawId())==theRpcEPosB.end()) ) {
              std::stringstream str;
              str<<"hDigiSpec_RpcEVsPt_PosB_"<<digi.rawId();
              std::string hName = str.str();
              str<<"_la"<<util.layer()<<"ch"<<(rpcId.sector()-1)*6+rpcId.subsector()<<"ro"<<rpcId.roll();
              std::string hTitle = str.str();
              theRpcEPosB[digi.rawId()] = new TH2D(hName.c_str(), hTitle.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins, 191,2.,193.);
            }
            if (!barrelRefHit && (theRpcEPosE.find(digi.rawId())==theRpcEPosE.end()) ) {
              std::stringstream str;
              str<<"hDigiSpec_RpcEVsPt_PosE_"<<digi.rawId();
              std::string hName = str.str();
              str<<"_la"<<util.layer()<<"ch"<<(rpcId.sector()-1)*6+rpcId.subsector()<<"rn"<<rpcId.ring()<<"ro"<<rpcId.roll();
              std::string hTitle = str.str();
              theRpcEPosE[digi.rawId()] = new TH2D(hName.c_str(), hTitle.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins, 191,2.,193.);
            }
            if (precisePos &&  barrelRefHit) theRpcEPosB[digi.rawId()]->Fill(simu->pt(), digi.halfStrip());
            if (precisePos && !barrelRefHit) theRpcEPosE[digi.rawId()]->Fill(simu->pt(), digi.halfStrip());
          }
          break; 
        }

        case MuonSubdetId::DT: {
          DTphDigiSpec digi(is->first, is->second);
          DTChamberId dt(digi.rawId());
          if (barrelRefHit && (theDtPosB.find(digi.rawId())==theDtPosB.end()) ) {
            std::stringstream str1, str2;
            str1<<"hDigiSpec_DtVsPt_PosB_"<<digi.rawId();
            str2<<"hDigiSpec_DtVsPt_BendB_"<<digi.rawId();
            std::string hName1 = str1.str();
            std::string hName2 = str2.str();
            str1<<"_s"<<dt.station()<<"sc"<<dt.sector();
            str2<<"_s"<<dt.station()<<"sc"<<dt.sector();
            std::string hTitle1 = str1.str();
            std::string hTitle2 = str2.str();
            theDtPosB[digi.rawId()] = new TH2D(hName1.c_str(), hTitle1.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins, 100,-2048.,2048.);
            theDtBendB[digi.rawId()] = new TH2D(hName2.c_str(), hTitle2.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins, 1100,-550.,550.);
          }
          if (!barrelRefHit && (theDtPosE.find(digi.rawId())==theDtPosE.end()) ) {
            std::stringstream str1, str2;
            str1<<"hDigiSpec_DtVsPt_PosE_"<<digi.rawId();
            str2<<"hDigiSpec_DtVsPt_BendE_"<<digi.rawId();
            std::string hName1 = str1.str();
            std::string hName2 = str2.str();
            str1<<"_s"<<dt.station()<<"sc"<<dt.sector();
            str2<<"_s"<<dt.station()<<"sc"<<dt.sector();
            std::string hTitle1 = str1.str();
            std::string hTitle2 = str2.str();
            theDtPosE[digi.rawId()] = new TH2D(hName1.c_str(), hTitle1.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins, 100,-2048.,2048.);
            theDtBendE[digi.rawId()] = new TH2D(hName2.c_str(), hTitle2.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins, 1100,-550.,550.);
          }
          if (precisePos &&  barrelRefHit) theDtPosB[digi.rawId()]->Fill(simu->pt(), digi.phi()); 
          if (precisePos &&  barrelRefHit) theDtBendB[digi.rawId()]->Fill(simu->pt(), digi.phiB()); 
          if (precisePos && !barrelRefHit) theDtPosE[digi.rawId()]->Fill(simu->pt(), digi.phi()); 
          if (precisePos && !barrelRefHit) theDtBendE[digi.rawId()]->Fill(simu->pt(), digi.phiB()); 
          break;
        } 

        case MuonSubdetId::CSC: {
          CSCDigiSpec digi(is->first, is->second);
          CSCDetId csc(digi.rawId());
          if ( barrelRefHit && (theCscPosB.find(digi.rawId())==theCscPosB.end()) ) {
            std::stringstream str1, str2;
            str1<<"hDigiSpec_CscVsPt_PosB_"<<digi.rawId();
            str2<<"hDigiSpec_CscVsPt_BendB_"<<digi.rawId();
            std::string hName1 = str1.str();
            std::string hName2 = str2.str();
            str1<<"_s"<<csc.station()<<"ch"<<csc.chamber()<<"rn"<<csc.ring();
            str2<<"_s"<<csc.station()<<"ch"<<csc.chamber()<<"rn"<<csc.ring();
            std::string hTitle1 = str1.str();
            std::string hTitle2 = str2.str();
            theCscPosB[digi.rawId()] = new TH2D(hName1.c_str(), hTitle1.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins, 100,0.,160.);
            theCscBendB[digi.rawId()] = new TH2D(hName2.c_str(), hTitle2.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins, 11,0.5,11.5);
          }
          if (!barrelRefHit && (theCscPosE.find(digi.rawId())==theCscPosE.end()) ) {
            std::stringstream str1, str2;
            str1<<"hDigiSpec_CscVsPt_PosE_"<<digi.rawId();
            str2<<"hDigiSpec_CscVsPt_BendE_"<<digi.rawId();
            std::string hName1 = str1.str();
            std::string hName2 = str2.str();
            str1<<"_s"<<csc.station()<<"ch"<<csc.chamber()<<"rn"<<csc.ring();
            str2<<"_s"<<csc.station()<<"ch"<<csc.chamber()<<"rn"<<csc.ring();
            std::string hTitle1 = str1.str();
            std::string hTitle2 = str2.str();
            theCscPosE[digi.rawId()] = new TH2D(hName1.c_str(), hTitle1.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins, 100,0.,160.);
            theCscBendE[digi.rawId()] = new TH2D(hName2.c_str(), hTitle2.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins, 11,0.5,11.5);
          }
          if ( barrelRefHit && precisePos) theCscPosB[digi.rawId()]->Fill(simu->pt(), digi.strip()); 
          if ( barrelRefHit && precisePos) theCscBendB[digi.rawId()]->Fill(simu->pt(), digi.pattern()); 
          if (!barrelRefHit && precisePos) theCscPosE[digi.rawId()]->Fill(simu->pt(), digi.strip()); 
          if (!barrelRefHit && precisePos) theCscBendE[digi.rawId()]->Fill(simu->pt(), digi.pattern()); 
          break;
        } 

        default: {std::cout <<" Unexpeced sebdet case, id ="<<is->first <<std::endl; return; }
      };
    } 
}

void AnaDigiSpec::resume(TObjArray& histos)
{
  std::cout <<" AnaDigiSpec, size of HMaps (barrel ref hit): CscPosB="<<theCscPosB.size()
                                         <<", CscBendB="<<theCscBendB.size()
                                         <<", DtPosB="<<theDtPosB.size()
                                         <<", DtBendB="<<theDtBendB.size()
                                         <<", RpcBPosB="<<theRpcBPosB.size()
                                         <<", RpcEPosB="<<theRpcEPosB.size()<<std::endl;
  std::cout <<" AnaDigiSpec, size of HMaps (endcal ref hit): CscPosE="<<theCscPosE.size()
                                         <<", CscBendE="<<theCscBendE.size()
                                         <<", DtPosE="<<theDtPosE.size()
                                         <<", DtBendE="<<theDtBendE.size()
                                         <<", RpcBPosE="<<theRpcBPosE.size()
                                         <<", RpcEPosE="<<theRpcEPosE.size()<<std::endl;
  VHisto aCscPos = topN(theCscPosB, 9);
  VHisto aRpcBPos = topN(theRpcBPosB, 9);
  VHisto aRpcEPos = topN(theRpcEPosB, 9);
  VHisto aDtPos = topN(theDtPosB, 9);
  VHisto aCscBend = topN(theCscBendB, 9);
  VHisto aDtBend = topN(theDtBendB, 9);
  typedef VHisto::const_iterator IV;
  for (IV iv =  aCscPos.begin(); iv !=  aCscPos.end(); ++iv) histos.Add(*iv);
  for (IV iv = aRpcBPos.begin(); iv != aRpcBPos.end(); ++iv) histos.Add(*iv);
  for (IV iv = aRpcEPos.begin(); iv != aRpcEPos.end(); ++iv) histos.Add(*iv);
  for (IV iv =   aDtPos.begin(); iv !=   aDtPos.end(); ++iv) histos.Add(*iv);
  for (IV iv =  aDtBend.begin(); iv !=  aDtBend.end(); ++iv) histos.Add(*iv);
  for (IV iv = aCscBend.begin(); iv != aCscBend.end(); ++iv) histos.Add(*iv);

  aCscPos = topN(theCscPosE, 9);
  aRpcBPos = topN(theRpcBPosE, 9);
  aRpcEPos = topN(theRpcEPosE, 9);
  aDtPos = topN(theDtPosE, 9);
  aCscBend = topN(theCscBendE, 9);
  aDtBend = topN(theDtBendE, 9);
  for (IV iv =  aCscPos.begin(); iv !=  aCscPos.end(); ++iv) histos.Add(*iv);
  for (IV iv = aRpcBPos.begin(); iv != aRpcBPos.end(); ++iv) histos.Add(*iv);
  for (IV iv = aRpcEPos.begin(); iv != aRpcEPos.end(); ++iv) histos.Add(*iv);
  for (IV iv =   aDtPos.begin(); iv !=   aDtPos.end(); ++iv) histos.Add(*iv);
  for (IV iv =  aDtBend.begin(); iv !=  aDtBend.end(); ++iv) histos.Add(*iv);
  for (IV iv = aCscBend.begin(); iv != aCscBend.end(); ++iv) histos.Add(*iv);

}

AnaDigiSpec::VHisto AnaDigiSpec::topN(const HMap & hmap, unsigned int nElements) const 
{
  VHisto result;
  typedef std::map<unsigned int, VHisto> StatMap;
  StatMap statMap;

  for (HMap::const_iterator im = hmap.begin(); im != hmap.end(); ++im) {
    statMap[im->second->Integral()].push_back(im->second);
  }  

  unsigned int nCurrElements = 0;
  for (StatMap::const_reverse_iterator im=statMap.rbegin(); im != statMap.rend(); im++) {
    for (VHisto::const_iterator ih=im->second.begin(); ih != im->second.end(); ih++) {
      std::string name( (*ih)->GetName() );
      std::size_t pos = name.find_last_of('_');
      std::string newName = name.substr(0,pos)+"_"+std::to_string(++nCurrElements);
//      std::cout<<" name: "<< name <<" NEW NAME: " << newName << std::endl;
      result.push_back( static_cast<TH2D*>( (*ih)->Clone(newName.c_str()) ) );
    } 
    if (result.size() >= nElements) return result; 
  }
  return result; 
}
