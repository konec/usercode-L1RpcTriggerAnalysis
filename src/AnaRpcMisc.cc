#include "UserCode/L1RpcTriggerAnalysis/interface/AnaRpcMisc.h"
#include "TProfile.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "DataFormats/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "DataFormats/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include <math.h>
#include <vector>

template <class T> T sqr( T t) {return t*t;}


void AnaRpcMisc::init(TObjArray& histos)
{
  hRpcMisc_UE = new TH2D("hRpcMisc_US","hRpcMisc_UnderEstimated", L1RpcEtaScale::nEtaBins, L1RpcEtaScale::etaBins, 144, 0., 2.*M_PI); histos.Add(hRpcMisc_UE);
  hRpcMisc_OE = new TH2D("hRpcMisc_OS","hRpcMisc_OverEstimated", L1RpcEtaScale::nEtaBins, L1RpcEtaScale::etaBins, 144, 0., 2.*M_PI); histos.Add(hRpcMisc_OE);
  hRpcMisc_EffRun = new TH1D("hRpcMisc_EffRun","Efficiency in run; efficiency; weighted numb. of runs / bin",200,0.,1.); histos.Add(hRpcMisc_EffRun);
  hRpcMisc_Time = new TH1D("hRpcMisc_Time","RPC Trigger timing",5,-2.5,2.5); histos.Add(hRpcMisc_Time);
  hRpcMisc_TimeAll = new TH1D("hRpcMisc_TimeAll","RPC Trigger timing",5,-2.5,2.5); histos.Add(hRpcMisc_TimeAll);
  hRpcMisc_TimeDen = new TH1D("hRpcMisc_TimeDen","RPC Trigger timing",5,-2.5,2.5); histos.Add(hRpcMisc_TimeDen);
}

double AnaRpcMisc::maxPt(const std::vector<L1Obj> & l1Objs) const
{
  double result = 0.;
  for (unsigned int i=0; i<l1Objs.size(); i++) if (l1Objs[i].pt > result) result = l1Objs[i].pt;
  return result; 
}

void AnaRpcMisc::run(const EventObj* event,  const MuonObj *muon, const L1ObjColl *l1Coll)
{
  if (effRunMap.find(event->run) == effRunMap.end()) effRunMap[event->run] = std::make_pair(0,0);
  if (purRunMap.find(event->run) == purRunMap.end()) purRunMap[event->run] = std::make_pair(0,0);

//  double etaMu = fabs(muon->eta());
  double ptMu  = muon->pt();   
  if (!muon->isGlobal()) return;

  L1ObjColl l1RpcColl = l1Coll->l1RpcColl();
  std::vector<L1Obj> l1Rpcs = l1RpcColl.selectByMatched().selectByBx().getL1Objs();

    if (ptMu > 50. && l1Rpcs.size()==1 && l1Rpcs[0].pt < 8.) { 
//     debug = true;
     if (debug) std::cout <<"HERE  ptMu: "<<ptMu<<" ptRPC: "<<l1Rpcs[0].pt <<" eta:"<<l1Rpcs[0].eta << std::endl;
      hRpcMisc_UE->Fill(l1Rpcs[0].eta, l1Rpcs[0].phi);
//      if (fabs(l1Rpcs[0].eta)>1.2) {
//         std::cout <<"RPC_pt: "<<l1Rpcs[0].pt<<" eta: "<<l1Rpcs[0].eta<<"phi: "<<l1Rpcs[0].phi<<std::endl;
//         std::cout <<"MU__pt:"<<muon->pt()<<" eta: "<<muon->eta()<<" phi: "<<muon->phi()<<std::endl;
//      }
    }
    if (ptMu < 8. && l1Rpcs.size()==1 && l1Rpcs[0].pt > 80.) hRpcMisc_OE->Fill(l1Rpcs[0].eta, l1Rpcs[0].phi);

    if (ptMu > 20.) {
    effRunMap[event->run].second++;
    if (l1Rpcs.size() > 0 && l1Rpcs[0].pt >= 16) effRunMap[event->run].first++; 
    }
    if (l1Rpcs.size() > 0 && fabs(muon->eta()) < 1.6) {
      if (ptMu > 10. && l1Rpcs[0].pt >= 10) purRunMap[event->run].first++;
      if (ptMu < 7.  && l1Rpcs[0].pt >= 10) purRunMap[event->run].second++;
    }

//
// timing
//
  if (ptMu > 10. && fabs(muon->eta()) < 1.6 ) { 
    std::vector<L1Obj> l1RpcsAll = l1RpcColl.getL1Objs();
    std::vector<L1Obj> l1RpcsMatched = l1RpcColl.selectByMatched().getL1Objs();
    for (unsigned int i=0; i<5; i++) hRpcMisc_TimeDen->Fill(i-2.);
    for (unsigned int i=0; i<l1RpcsMatched.size(); i++) hRpcMisc_Time->Fill(l1RpcsMatched[i].bx);
    for (unsigned int i=0; i<l1RpcsAll.size();i++) hRpcMisc_TimeAll->Fill(l1RpcsAll[i].bx);
//    for (unsigned int i=0; i<l1RpcsMatched.size(); i++) if (l1RpcsMatched[i].bx==-2) debug = true;
  }
  if (debug) {
    std::cout <<"------------"<<std::endl;
    std::cout <<" Event: "<<(*event).id <<" Lumi: "<<(*event).lumi<< std::endl;
    std::cout <<"MU__pt:"<<muon->pt()<<" eta: "<<muon->eta()<<" phi: "<<muon->phi()<<std::endl;
    std::cout << l1RpcColl << std::endl;
    
  }
}	


void AnaRpcMisc::resume(TObjArray& histos)
{
  // average efficiency per Run
  TGraphErrors * hGraphRun = new TGraphErrors;
  hGraphRun->SetName("hGraph_RunEff"); 
  histos.Add(hGraphRun);
  unsigned int nPoints = 0;
//  for( EffRunMap::const_iterator im = effRunMap.begin(); im != effRunMap.end(); ++im) if (im->second.first != 0) ++nPoints;
  nPoints = effRunMap.size();
  hGraphRun->Set(nPoints);
  unsigned int iPoint=0;
  for( EffRunMap::const_iterator im = effRunMap.begin(); im != effRunMap.end(); ++im) {
    float eff = 0.;
    float effErr = 1.;
//    if (im->second.first==0 ) continue;
    if (im->second.second != 0) {
      eff = float(im->second.first)/float(im->second.second);
    float effM1 = (im->second.first > 0)? float(im->second.first-1)/float(im->second.second) : 0.;
          effErr = sqrt( (1-effM1)*std::max((int) im->second.first,1))/im->second.second;
    }
    std::cout <<" RUN: "<<im->first
              <<" Effic: "<< eff <<" ("<<im->second.first<<"/"<<im->second.second<<")"<<std::endl;
    hRpcMisc_EffRun->Fill(eff, 1./sqr(effErr));
    hGraphRun->SetPoint(iPoint, im->first, eff);
    hGraphRun->SetPointError(iPoint, 0., effErr);
    iPoint++;
  }

  TGraphErrors * hGraphPur = new TGraphErrors;
  hGraphPur->SetName("hGraph_RunPur"); 
  histos.Add(hGraphPur);
  iPoint = 0;
  for( PurRunMap::const_iterator im = purRunMap.begin(); im != purRunMap.end(); ++im) { 
//    if (im->second.second == 0) continue;
    hGraphPur->Set(++iPoint);
    double pur=0.;
    double purErr=0.;;
    if (im->second.second != 0) {
      double second = double(im->second.second);
      pur = double(im->second.first)/second;
      double firstE = im->second.first==0 ? 1. : double(im->second.first);
      purErr = firstE/second*sqrt(1./firstE+ 1./second); 
    }
    std::cout<<" RUN: " <<  im->first <<" Good: "<<im->second.first<<" Bad: "<< im->second.second << std::endl;
    hGraphPur->SetPoint(iPoint-1, im->first, pur);
    hGraphPur->SetPointError(iPoint-1, 0., purErr);
  }

}
