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
#include <sstream>

template <class T> T sqr( T t) {return t*t;}
namespace {
  TH1D *hTimingL1_Rpc, *hTimingL1_RpcEmu, *hTimingL1_Dt, *hTimingL1_Csc;
  TH1D *hTimingL1_Gmt, *hTimingL1_GmtEmu, *hTimingL1_GmtAll; 
  TH2D *hTimingL1_RpcVsDt, *hTimingL1_RpcVsCsc;
  TH1D *hTimingL1_EtaDt, *hTimingL1_EtaCsc;
  TProfile *hTimingL1_Prof;
  TH1D *hTimingL1_RMS;
  TH1D *hTimingL1_DR_Dt, *hTimingL1_DR_Rpc, *hTimingL1_DR_Csc, *hTimingL1_DR_Gmt;

  std::map<std::string, TH1D* > hmap;
  std::vector<double> drs = {9999, 0.5, 0.3, 0.2, 0.1, 0.06};
  std::vector<std::string> tags = {"Rpc","Dt","Csc","Gmt","RpcEmu","GmtEmu"};
  const unsigned int nptBins = 7;
  double ptBins[nptBins+1]={0.1, 5., 7., 12.,16., 20., 25.,161.};
}

void AnaTimingL1::init(TObjArray& histos)
{
  hTimingL1_Rpc = new TH1D("hTimingL1_Rpc","hTimingL1_Rpc", 5,-2.5,2.5);   hTimingL1_Rpc->Sumw2(); histos.Add(hTimingL1_Rpc);
  hTimingL1_RpcEmu = new TH1D("hTimingL1_RpcEmu","hTimingL1_RpcEmu", 5,-2.5,2.5);   hTimingL1_RpcEmu->Sumw2(); histos.Add(hTimingL1_RpcEmu);
  hTimingL1_Dt  = new TH1D("hTimingL1_Dt","hTimingL1_Dt",   5,-2.5,2.5);   hTimingL1_Dt->Sumw2();  histos.Add(hTimingL1_Dt);
  hTimingL1_Csc = new TH1D("hTimingL1_Csc","hTimingL1_Csc", 5,-2.5,2.5);   hTimingL1_Csc->Sumw2(); histos.Add(hTimingL1_Csc);
  hTimingL1_Gmt = new TH1D("hTimingL1_Gmt","hTimingL1_Gmt", 5,-2.5,2.5);   hTimingL1_Gmt->Sumw2(); histos.Add(hTimingL1_Gmt);
  hTimingL1_GmtEmu = new TH1D("hTimingL1_GmtEmu","hTimingL1_GmtEmu", 5,-2.5,2.5);   hTimingL1_GmtEmu->Sumw2(); histos.Add(hTimingL1_GmtEmu);

  hTimingL1_GmtAll = new TH1D("hTimingL1_GmtAll","hTimingL1_GmtAll", 5,-2.5,2.5);   hTimingL1_GmtAll->Sumw2(); histos.Add(hTimingL1_GmtAll);
  hTimingL1_RpcVsDt  = new TH2D("hTimingL1_RpcVsDt", "hTimingL1_RpcVsDt", 5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsDt);
  hTimingL1_RpcVsCsc = new TH2D("hTimingL1_RpcVsCsc","hTimingL1_RpcVsCsc",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsCsc);
  hTimingL1_EtaDt  = new TH1D("hTimingL1_EtaDt","hTimingL1_EtaDt",50,-2.5,2.5); histos.Add(hTimingL1_EtaDt);
  hTimingL1_EtaCsc = new TH1D("hTimingL1_EtaCsc","hTimingL1_EtaCsc",50,-2.5,2.5); histos.Add(hTimingL1_EtaCsc);

  hTimingL1_DR_Dt  = new TH1D("hTimingL1_DR_Dt", "hTimingL1_DR_Dt", 80,0.,0.8);histos.Add(hTimingL1_DR_Dt); 
  hTimingL1_DR_Rpc = new TH1D("hTimingL1_DR_Rpc","hTimingL1_DR_Rpc",80,0.,0.8);histos.Add(hTimingL1_DR_Rpc); 
  hTimingL1_DR_Csc = new TH1D("hTimingL1_DR_Csc","hTimingL1_DR_Csc",80,0.,0.8);histos.Add(hTimingL1_DR_Csc); 
  hTimingL1_DR_Gmt = new TH1D("hTimingL1_DR_Gmt","hTimingL1_DR_Gmt",80,0.,0.8);histos.Add(hTimingL1_DR_Gmt); 

  const unsigned int nBins = 5;
  double myPtBins[nBins+1]={5., 7., 12.,16., 20., 25.};
  hTimingL1_Prof = new TProfile("hTimingL1_Prof","hTimingL1_Prof",nBins, myPtBins,"s"); histos.Add(hTimingL1_Prof);
  hTimingL1_RMS = new TH1D("hTimingL1_RMS","hTimingL1_RMS",nBins, myPtBins); histos.Add(hTimingL1_RMS);

  //
  // 1D timing histos
  //
  for (std::vector<std::string>::const_iterator is = tags.begin(); is !=  tags.end(); is++) {

    //
    // different DR
    //
    for (std::vector<double>::const_iterator id = drs.begin(); id != drs.end(); id++) {
      std::stringstream str;
      str << "hTimingL1_"<<*is<<"_DR"<<*id;
      TH1D *h = new TH1D(str.str().c_str(), str.str().c_str(), 5,-2.5,2.5); h->Sumw2(); histos.Add(h); hmap[str.str()] = h;
    }

    //
    // different L1Candpt
    //
    for (unsigned int ipt = 0; ipt < nptBins; ipt++) {
      std::stringstream str;
      str << "hTimingL1_"<<*is<<"_PT"<<ptBins[ipt];
      TH1D *h = new TH1D(str.str().c_str(), str.str().c_str(), 5,-2.5,2.5); h->Sumw2(); histos.Add(h); hmap[str.str()] = h;
    }
  }
  histos.Add( new TH1D("hPrefire","hPrefire",nptBins, ptBins) );

}

void  AnaTimingL1::resume(TObjArray& histos)
{
  for (int ibin=1; ibin<=hTimingL1_Prof->GetNbinsX(); ibin++) {
    double v=hTimingL1_Prof->GetBinError(ibin);
    hTimingL1_RMS->SetBinContent(ibin, v);
    hTimingL1_RMS->SetBinError(ibin, 0.);
  }

}

void  AnaTimingL1::run(const EventObj* ev, const MuonObj* muon, const L1ObjColl *l1Coll)
{
  if (!muon->isGlobal()) return;

//  if (l1Coll->l1RpcColl().selectByBx(1,1).getL1Objs().size()>0) std::cout <<"-----------HAS RPC IN BX 1: "<<std::endl<<*l1Coll<<std::endl;

  for (std::vector<std::string>::const_iterator is = tags.begin(); is !=  tags.end(); is++) {
    L1ObjColl coll;
    if ( *is == "Rpc"    ) coll = l1Coll->l1RpcColl();  
    if ( *is == "RpcEmu" ) coll = l1Coll->l1RpcCollEmu();  
    if ( *is == "Dt"     ) coll = l1Coll->selectByType(L1Obj::DT);
    if ( *is == "Csc"    ) coll = l1Coll->selectByType(L1Obj::CSC);
    if ( *is == "Gmt"    ) coll = l1Coll->selectByType(L1Obj::GMT);
    if ( *is == "GmtEmu" ) coll = l1Coll->selectByType(L1Obj::GMT_emu);

    
    // timing for different DR
    for (std::vector<double>::const_iterator idr = drs.begin(); idr != drs.end(); idr++) {
      std::stringstream name; name<<"hTimingL1_"<<*is<<"_DR"<<*idr;
      for (int ibx=-2; ibx <=2; ibx++) {
        L1ObjColl scoll = coll.selectByDeltaR(*idr).selectByBx(ibx,ibx).selectByPtMin(theConfig.getParameter<double>("l1ptCutForDR"));
        if(scoll.getL1Objs().size() >0) hmap[name.str()]->Fill(ibx);
      }
    }
    
    // timing for different L1pt
    for (unsigned int ipt=0; ipt < nptBins; ipt++) {
      std::stringstream name; name<<"hTimingL1_"<<*is<<"_PT"<<ptBins[ipt];
      for (int ibx=-2; ibx <=2; ibx++) {
        L1ObjColl scoll = coll.selectByDeltaR(theConfig.getParameter<double>("l1DRCutForpT")).selectByBx(ibx,ibx).selectByPt(ptBins[ipt],ptBins[ipt+1]);
        if(scoll.getL1Objs().size() >0) hmap[name.str()]->Fill(ibx);
      }
    } 
  }


  std::vector<L1Obj> l1Rpcs = l1Coll->l1RpcColl().selectByMatched().getL1Objs();
  std::vector<L1Obj> l1Cscs = l1Coll->selectByType(L1Obj::CSC).selectByMatched().getL1Objs();
  std::vector<L1Obj> l1Dts  = l1Coll->selectByType(L1Obj::DT).selectByMatched().getL1Objs();
  std::vector<L1Obj> l1Gmts = l1Coll->selectByType(L1Obj::GMT).selectByMatched().getL1Objs();
  std::vector<L1Obj> l1RpcEmus = l1Coll->l1RpcCollEmu().selectByMatched().getL1Objs();
  std::vector<L1Obj> l1GmtEmus = l1Coll->selectByType(L1Obj::GMT_emu).selectByMatched().getL1Objs();

  int bxRpc = 999;
  int bxDt = 999;
  int bxCsc = 999;
  bool rpc, dt,csc, gmt; 
  rpc=dt=csc=gmt=false; 

  //
  // DR histos
  //
  std::vector<double> deltaR;
  deltaR = l1Coll->l1RpcColl().getL1ObjDeltaR();
  for (unsigned int i=0; i< deltaR.size(); ++i) hTimingL1_DR_Rpc->Fill(deltaR[i]);
  deltaR = l1Coll->selectByType(L1Obj::CSC).getL1ObjDeltaR();
  for (unsigned int i=0; i< deltaR.size(); ++i) hTimingL1_DR_Csc->Fill(deltaR[i]);
  deltaR = l1Coll->selectByType(L1Obj::DT).getL1ObjDeltaR();
  for (unsigned int i=0; i< deltaR.size(); ++i) hTimingL1_DR_Dt->Fill(deltaR[i]);
  deltaR = l1Coll->selectByType(L1Obj::GMT).getL1ObjDeltaR();
  for (unsigned int i=0; i< deltaR.size(); ++i) hTimingL1_DR_Gmt->Fill(deltaR[i]);
    

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
  double ptMu  = muon->pt();
  for (unsigned int i=0; i< l1Gmts.size(); i++) {
    gmt = true;
    hTimingL1_GmtAll->Fill(l1Gmts[i].bx);
    if ( (l1Gmts[i].bx >= 0) && (l1Gmts[i].bx < bxGmtpo)  )bxGmtpo = l1Gmts[i].bx; 
    if ( (l1Gmts[i].bx <= 0) && (l1Gmts[i].bx > bxGmtpre) )bxGmtpre = l1Gmts[i].bx; 
  }
  if (gmt) {
    if (bxGmtpo==0 || bxGmtpre==0)  {hTimingL1_Gmt->Fill(0); hTimingL1_Prof->Fill(ptMu,0);}
    else {
//      std::cout <<*l1Coll<<std::endl;
      if (bxGmtpo > 0 && bxGmtpo<999)   {hTimingL1_Gmt->Fill(bxGmtpo);  hTimingL1_Prof->Fill(ptMu,bxGmtpo);}
      if (bxGmtpre< 0 && bxGmtpre>-999) {hTimingL1_Gmt->Fill(bxGmtpre); hTimingL1_Prof->Fill(ptMu, bxGmtpre);}
    }
  }


  if (rpc) hTimingL1_Rpc->Fill(bxRpc);
  if (rpc && bxRpc==2) std::cout << *l1Coll << std::endl;
  if (dt)  hTimingL1_Dt->Fill(bxDt);
  if (csc) hTimingL1_Csc->Fill(bxCsc);

  int bxRpcEmu = 999;
  for (unsigned int i=0; i< l1RpcEmus.size(); i++) {
    if (l1RpcEmus[i].bx < bxRpcEmu) bxRpcEmu = l1RpcEmus[i].bx;
  }
  if ( dt && rpc)  hTimingL1_RpcVsDt->Fill(bxRpcEmu, bxDt);
  if (csc && rpc) hTimingL1_RpcVsCsc->Fill(bxRpcEmu, bxCsc);
  
}
