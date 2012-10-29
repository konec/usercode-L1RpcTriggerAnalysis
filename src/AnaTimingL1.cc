#include "UserCode/L1RpcTriggerAnalysis/interface/AnaTimingL1.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TGraphErrors.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include <vector>
#include <cmath>

template <class T> T sqr( T t) {return t*t;}
namespace {
  TH1D *hTimingL1_Rpc, *hTimingL1_Dt, *hTimingL1_Csc;
  TH1D *hTimingL1_Gmt, *hTimingL1_GmtAll; 
  TH2D *hTimingL1_RpcVsDt, *hTimingL1_RpcVsCsc;
  TH1D *hTimingL1_EtaDt, *hTimingL1_EtaCsc;
  TProfile *hTimingL1_Prof;
  TH1D *hTimingL1_RMS;
}

void AnaTimingL1::init(TObjArray& histos)
{
  hTimingL1_Rpc = new TH1D("hTimingL1_Rpc","hTimingL1_Rpc", 5,-2.5,2.5);   hTimingL1_Rpc->Sumw2(); histos.Add(hTimingL1_Rpc);
  hTimingL1_Dt  = new TH1D("hTimingL1_Dt","hTimingL1_Dt",   5,-2.5,2.5);   hTimingL1_Dt->Sumw2();  histos.Add(hTimingL1_Dt);
  hTimingL1_Csc = new TH1D("hTimingL1_Csc","hTimingL1_Csc", 5,-2.5,2.5);   hTimingL1_Csc->Sumw2(); histos.Add(hTimingL1_Csc);
  hTimingL1_Gmt = new TH1D("hTimingL1_Gmt","hTimingL1_Gmt", 5,-2.5,2.5);   hTimingL1_Gmt->Sumw2(); histos.Add(hTimingL1_Gmt);
  hTimingL1_GmtAll = new TH1D("hTimingL1_GmtAll","hTimingL1_GmtAll", 5,-2.5,2.5);   hTimingL1_GmtAll->Sumw2(); histos.Add(hTimingL1_GmtAll);
  hTimingL1_RpcVsDt  = new TH2D("hTimingL1_RpcVsDt", "hTimingL1_RpcVsDt", 5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsDt);
  hTimingL1_RpcVsCsc = new TH2D("hTimingL1_RpcVsCsc","hTimingL1_RpcVsCsc",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsCsc);
  hTimingL1_EtaDt  = new TH1D("hTimingL1_EtaDt","hTimingL1_EtaDt",50,-2.5,2.5); histos.Add(hTimingL1_EtaDt);
  hTimingL1_EtaCsc = new TH1D("hTimingL1_EtaCsc","hTimingL1_EtaCsc",50,-2.5,2.5); histos.Add(hTimingL1_EtaCsc);
  const unsigned int nBins = 5;
  double myPtBins[nBins+1]={5., 7., 12.,16., 20., 25.};
  hTimingL1_Prof = new TProfile("hTimingL1_Prof","hTimingL1_Prof",nBins, myPtBins,"s"); histos.Add(hTimingL1_Prof);
  hTimingL1_RMS = new TH1D("hTimingL1_RMS","hTimingL1_RMS",nBins, myPtBins); histos.Add(hTimingL1_RMS);
}

void  AnaTimingL1::resume(TObjArray& histos)
{
  for (int ibin=1; ibin<=hTimingL1_Prof->GetNbinsX(); ibin++) {
    double v=hTimingL1_Prof->GetBinError(ibin);
    hTimingL1_RMS->SetBinContent(ibin, v);
    hTimingL1_RMS->SetBinError(ibin, 0.);
  }

}

void  AnaTimingL1::run(const EventObj* ev, const MuonObj* muon, const L1ObjColl *l1RpcColl, const L1ObjColl *l1OtherColl, const L1ObjColl *l1GmtColl)
{
  double ptMu  = muon->pt();
  if (ptMu < 5) return;
  if (!muon->isGlobal()) return;

  std::vector<L1Obj> l1Rpcs = l1RpcColl->getL1ObjsMatched();
  std::vector<L1Obj> l1Oths = l1OtherColl->getL1ObjsMatched();  
  std::vector<L1Obj> l1Cscs = L1ObjColl::typeSelector(l1Oths, L1Obj::CSC);
  std::vector<L1Obj> l1Dts  = L1ObjColl::typeSelector(l1Oths, L1Obj::DT);
  std::vector<L1Obj> l1Gmts = l1GmtColl->getL1ObjsMatched();  

  int bxRpc = 999;
  int bxDt = 999;
  int bxCsc = 999;
  bool rpc, dt,csc, gmt; 
  rpc=dt=csc=gmt=false; 

  for (unsigned int i=0; i< l1Rpcs.size(); i++) {
    if (1+l1Rpcs[i].bx < bxRpc) { 
      rpc=true; 
      bxRpc = l1Rpcs[i].bx+1;
    }
  }

  for (unsigned int i=0; i< l1Dts.size(); i++) {
    if (l1Dts[i].bx < bxDt) { bxDt = l1Dts[i].bx; dt = true; }
    hTimingL1_EtaDt->Fill(l1Dts[i].eta); 
  }

  for (unsigned int i=0; i< l1Cscs.size(); i++) {
    if (l1Cscs[i].bx < bxCsc) { bxCsc = l1Cscs[i].bx; csc = true; }
    hTimingL1_EtaCsc->Fill(l1Cscs[i].eta); 
  }
  
  int bxGmtpre = -999;
  int bxGmtpo  = 999;
  for (unsigned int i=0; i< l1Gmts.size(); i++) {
    gmt = true;
    hTimingL1_GmtAll->Fill(l1Gmts[i].bx);
    if ( (l1Gmts[i].bx >= 0) && (l1Gmts[i].bx < bxGmtpo)  )bxGmtpo = l1Gmts[i].bx; 
    if ( (l1Gmts[i].bx <= 0) && (l1Gmts[i].bx > bxGmtpre) )bxGmtpre = l1Gmts[i].bx; 
  }
  if (gmt) {
    if (bxGmtpo==0 || bxGmtpre==0)  {hTimingL1_Gmt->Fill(0); hTimingL1_Prof->Fill(ptMu,0);}
    else {
      l1GmtColl->print();
      l1OtherColl->print();
      l1RpcColl->print();
      if (bxGmtpo > 0 && bxGmtpo<999)   {hTimingL1_Gmt->Fill(bxGmtpo);  hTimingL1_Prof->Fill(ptMu,bxGmtpo);}
      if (bxGmtpre< 0 && bxGmtpre>-999) {hTimingL1_Gmt->Fill(bxGmtpre); hTimingL1_Prof->Fill(ptMu, bxGmtpre);}
    }
  }


  if (rpc) hTimingL1_Rpc->Fill(bxRpc);
  if (dt)  hTimingL1_Dt->Fill(bxDt);
  if (csc) hTimingL1_Csc->Fill(bxCsc);
  if ( dt && rpc)  hTimingL1_RpcVsDt->Fill(bxRpc, bxDt);
  if (csc && rpc) hTimingL1_RpcVsCsc->Fill(bxRpc, bxCsc);
  
}
