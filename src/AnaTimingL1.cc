#include "UserCode/L1RpcTriggerAnalysis/interface/AnaTimingL1.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TGraphErrors.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObjBXExtra.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include <vector>
#include <cmath>
#include <sstream>

template <class T> T sqr( T t) {return t*t;}
namespace {
  TH1D *hTimingL1_Rpc, *hTimingL1_Dt, *hTimingL1_Csc, *hTimingL1_Gmt, *hTimingL1_GmtAll; 
  TH2D *hTimingL1_RpcVsDtOpen, *hTimingL1_RpcVsCscOpen, *hTimingL1_RpcVsDtTight, *hTimingL1_RpcVsCscTight;
  TProfile *hTimingL1_Prof;
  TH1D *hTimingL1_RMS;
  TH1D *hTimingL1_DR_Dt, *hTimingL1_DR_Rpc, *hTimingL1_DR_Csc, *hTimingL1_DR_Gmt;

//  std::map<std::string, TH1D* > hmap;

  std::vector<double> drs = {9999, 0.5, 0.3, 0.2, 0.1, 0.06};
  std::vector<std::string> tags = {"Rpc","Dt","Csc","Gmt","RpcEmu","GmtEmu"};
  const unsigned int nptBins = 7;
  double ptBins[nptBins+1]={0.1, 5., 7., 12.,16., 20., 25.,161.};
  const unsigned int nptCuts = 3;
  double ptCuts[nptCuts+1]={0., 6., 16.};

  TH1D* hL1ptBxForDR[6][nptBins][nptCuts]={ { {0} } };
  TH1D* hL1ptBxForDR_noBXm2[6][nptBins][nptCuts]={ { {0} } };
  TH1D* hL1ptBxForDR_noBXp2[6][nptBins][nptCuts]={ { {0} } };
  TH1D* hL1DRForPt[6][6] = {{0}};
}

void AnaTimingL1::init(TObjArray& histos)
{
  hTimingL1_Rpc = new TH1D("hTimingL1_Rpc","hTimingL1_Rpc", 5,-2.5,2.5);   hTimingL1_Rpc->Sumw2(); histos.Add(hTimingL1_Rpc);
  hTimingL1_Dt  = new TH1D("hTimingL1_Dt","hTimingL1_Dt",   5,-2.5,2.5);   hTimingL1_Dt->Sumw2();  histos.Add(hTimingL1_Dt);
  hTimingL1_Csc = new TH1D("hTimingL1_Csc","hTimingL1_Csc", 5,-2.5,2.5);   hTimingL1_Csc->Sumw2(); histos.Add(hTimingL1_Csc);
  hTimingL1_Gmt = new TH1D("hTimingL1_Gmt","hTimingL1_Gmt", 5,-2.5,2.5);   hTimingL1_Gmt->Sumw2(); histos.Add(hTimingL1_Gmt);
  hTimingL1_GmtAll = new TH1D("hTimingL1_GmtAll","hTimingL1_GmtAll", 5,-2.5,2.5);   hTimingL1_GmtAll->Sumw2(); histos.Add(hTimingL1_GmtAll);

  hTimingL1_RpcVsDtOpen  = new TH2D("hTimingL1_RpcVsDtOpen", "hTimingL1_RpcVsDtOpen", 5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsDtOpen);
  hTimingL1_RpcVsCscOpen = new TH2D("hTimingL1_RpcVsCscOpen","hTimingL1_RpcVsCscOpen",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsCscOpen);
  hTimingL1_RpcVsDtTight  = new TH2D("hTimingL1_RpcVsDtTight", "hTimingL1_RpcVsDtTight", 5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsDtTight);
  hTimingL1_RpcVsCscTight = new TH2D("hTimingL1_RpcVsCscTight","hTimingL1_RpcVsCscTight",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsCscTight);

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
    unsigned int itag = is-tags.begin();

    //
    // different DR
    //
    for (std::vector<double>::const_iterator id = drs.begin(); id != drs.end(); id++) {
      unsigned int idr = id-drs.begin();
      std::stringstream str;
      str << "hTimingL1_"<<*is<<"_DR"<<*id;
      TH1D *h = new TH1D(str.str().c_str(), str.str().c_str(), 5,-2.5,2.5); h->Sumw2(); histos.Add(h); 
      hL1DRForPt[itag][idr] = h;
    }

    //
    // different L1Candpt
    //
    for (unsigned int iptL1 = 0; iptL1 < nptBins; iptL1++) {
      for (unsigned int iptMu = 0; iptMu < nptCuts; iptMu++) {
        std::stringstream str;
        str << "hTimingL1_"<<*is<<"_L1PT"<<ptBins[iptL1]<<"_MuPT"<<ptCuts[iptMu];
        TH1D *h = new TH1D(str.str().c_str(), str.str().c_str(), 5,-2.5,2.5); h->Sumw2(); histos.Add(h); 
        hL1ptBxForDR[itag][iptL1][iptMu]=h;
          //noBXm2
        {
          std::string name = str.str()+"_noBXm2";
          TH1D *h = new TH1D(name.c_str(), name.c_str(), 5,-2.5,2.5); h->Sumw2(); histos.Add(h); 
          hL1ptBxForDR_noBXm2[itag][iptL1][iptMu]=h;
        }
        {
          std::string name = str.str()+"_noBXp2";
          TH1D *h = new TH1D(name.c_str(), name.c_str(), 5,-2.5,2.5); h->Sumw2(); histos.Add(h); 
          hL1ptBxForDR_noBXp2[itag][iptL1][iptMu]=h;
        }
      }
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

void  AnaTimingL1::run(const EventObj* ev_noBx, const MuonObj* muon, const L1ObjColl *l1Coll)
{
  if (!muon->isGlobal()) return;
  const EventObjBXExtra *ev = dynamic_cast<const EventObjBXExtra*>(ev_noBx);
  if (!ev) return;


  //
  // main 1D histos, BX vs DR and BX vs PT 
  //
//  for (std::vector<std::string>::const_iterator is = tags.begin(); is !=  tags.end(); is++) {
  for (unsigned int itag=0; itag < tags.size(); itag++) {
    L1ObjColl coll;
    if ( tags[itag] == "Rpc"    ) coll = l1Coll->l1RpcColl();  
    if ( tags[itag] == "RpcEmu" ) coll = l1Coll->l1RpcCollEmu();  
    if ( tags[itag] == "Dt"     ) coll = l1Coll->selectByType(L1Obj::DT);
    if ( tags[itag] == "Csc"    ) coll = l1Coll->selectByType(L1Obj::CSC);
    if ( tags[itag] == "Gmt"    ) coll = l1Coll->selectByType(L1Obj::GMT); //.selectByQuality(4,7);
    if ( tags[itag] == "GmtEmu" ) coll = l1Coll->selectByType(L1Obj::GMT_emu); //.selectByQuality(4,7);
    if (!coll) continue;

    
    // timing for different DR
    static double l1ptCutForDR = theConfig.getParameter<double>("l1ptCutForDR");
    L1ObjColl collPt = coll.selectByPtMin(l1ptCutForDR);
    if (collPt) {
      for (std::vector<double>::const_iterator idr = drs.begin(); idr != drs.end(); idr++) {
        for (int ibx=-2; ibx <=2; ibx++) {
          if (collPt.isMatching_DRBx(*idr,ibx)) hL1DRForPt[itag][idr-drs.begin()]->Fill(ibx);
        }
      }
    }
    
    static double l1DRCutForpT = theConfig.getParameter<double>("l1DRCutForpT");
    L1ObjColl collDR = coll.selectByDeltaR( l1DRCutForpT);
    if (collDR) { 
    // timing for different L1pt
      for (unsigned int iptL1=0; iptL1 < nptBins; iptL1++) {
        for (int ibx=-2; ibx <=2; ibx++) {
          if (!collDR.isMatching_PtminPtmaxBx(ptBins[iptL1], ptBins[iptL1+1],ibx)) continue; 
          for (unsigned int iptMu=0; iptMu < nptCuts; iptMu++) {
            if (muon->pt() < ptCuts[iptMu]) continue;
            hL1ptBxForDR[itag][iptL1][iptMu]->Fill(ibx); 
            if (!ev->hasValidBX_Minus2) hL1ptBxForDR_noBXm2[itag][iptL1][iptMu]->Fill(ibx);
            if (!ev->hasValidBX_Plus2)  hL1ptBxForDR_noBXp2[itag][iptL1][iptMu]->Fill(ibx);
          }
        }
      } 
    }

    //
    // DR histos
    //
    const std::vector<double> & deltaR = coll.getL1ObjDeltaR();
    if (tags[itag]=="Rpc") for (unsigned int i=0; i< deltaR.size(); ++i) hTimingL1_DR_Rpc->Fill(deltaR[i]);
    if (tags[itag]=="Dt" ) for (unsigned int i=0; i< deltaR.size(); ++i) hTimingL1_DR_Dt->Fill(deltaR[i]);
    if (tags[itag]=="Csc") for (unsigned int i=0; i< deltaR.size(); ++i) hTimingL1_DR_Csc->Fill(deltaR[i]);
    if (tags[itag]=="Gmt") for (unsigned int i=0; i< deltaR.size(); ++i) hTimingL1_DR_Gmt->Fill(deltaR[i]);
  }


  //
  // tight correlation histograms;
  //
  static double muptCutForCorrTight = theConfig.getParameter<double>("muptCutForCorrTight");
  static double l1DRCutForCorrTight = theConfig.getParameter<double>("l1DRCutForCorrTight");
  static double l1ptCutForCorrTight = theConfig.getParameter<double>("l1ptCutForCorrTight");
  if (muon->pt() >= muptCutForCorrTight) {
    L1ObjColl collTight = l1Coll->selectByPtMin(l1ptCutForCorrTight).selectByDeltaR(l1DRCutForCorrTight);
    if (collTight) { 
      std::vector<L1Obj> l1Cscs = collTight.selectByType(L1Obj::CSC).getL1Objs();
      std::vector<L1Obj> l1Dts  = collTight.selectByType(L1Obj::DT ).getL1Objs();
      std::vector<L1Obj> l1RpcEmus = collTight.l1RpcCollEmu().getL1Objs();
      for (unsigned int ir=0; ir < l1RpcEmus.size(); ir++) {
        for (unsigned int io=0; io < l1Dts.size(); io++) hTimingL1_RpcVsDtTight->Fill( l1RpcEmus[ir].bx, l1Dts[io].bx);
        for (unsigned int io=0; io < l1Cscs.size(); io++) hTimingL1_RpcVsCscTight->Fill( l1RpcEmus[ir].bx, l1Cscs[io].bx); 
      }
    }
  }

  //
  // open/loose correlation histograms
  //
  {
    std::vector<L1Obj> l1Cscs = l1Coll->selectByType(L1Obj::CSC).getL1Objs();
    std::vector<L1Obj> l1Dts  = l1Coll->selectByType(L1Obj::DT).getL1Objs();
    std::vector<L1Obj> l1RpcEmus = l1Coll->l1RpcCollEmu().getL1Objs();
    for (unsigned int ir=0; ir < l1RpcEmus.size(); ir++) {
      for (unsigned int io=0; io < l1Dts.size(); io++) hTimingL1_RpcVsDtOpen->Fill( l1RpcEmus[ir].bx, l1Dts[io].bx);
      for (unsigned int io=0; io < l1Cscs.size(); io++) hTimingL1_RpcVsCscOpen->Fill( l1RpcEmus[ir].bx, l1Cscs[io].bx); 
    }
  }
    

  //
  // first candidate arrival histos (from data only)
  //
  {
    static double ptMin = theConfig.getParameter<double>("l1ptCutForDR");
    static double drMax = theConfig.getParameter<double>("l1DRCutForpT"); 
    std::vector<L1Obj> l1Rpcs = l1Coll->l1RpcColl().selectByPtMin(ptMin).selectByDeltaR(drMax).getL1Objs();
    std::vector<L1Obj> l1Cscs = l1Coll->selectByType(L1Obj::CSC).selectByPtMin(ptMin).selectByDeltaR(drMax).getL1Objs();
    std::vector<L1Obj> l1Dts  = l1Coll->selectByType(L1Obj::DT).selectByPtMin(ptMin).selectByDeltaR(drMax).getL1Objs();
    std::vector<L1Obj> l1Gmts = l1Coll->selectByType(L1Obj::GMT).selectByPtMin(ptMin).selectByDeltaR(drMax).getL1Objs();

    int bxRpc = 999;
    int bxDt = 999;
    int bxCsc = 999;
    bool rpc, dt,csc, gmt; 
    rpc=dt=csc=gmt=false; 

    for (unsigned int i=0; i< l1Rpcs.size(); i++) { if (1+l1Rpcs[i].bx < bxRpc) { rpc=true; bxRpc = l1Rpcs[i].bx+1; } }
    for (unsigned int i=0; i< l1Dts.size(); i++)  { if (l1Dts[i].bx < bxDt) { bxDt = l1Dts[i].bx; dt = true; } }
    for (unsigned int i=0; i< l1Cscs.size(); i++) { if (l1Cscs[i].bx < bxCsc) { bxCsc = l1Cscs[i].bx; csc = true; } }

    if (rpc) hTimingL1_Rpc->Fill(bxRpc);
    if (dt)  hTimingL1_Dt->Fill(bxDt);
    if (csc) hTimingL1_Csc->Fill(bxCsc);
  
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
        if (bxGmtpo > 0 && bxGmtpo<999)   {hTimingL1_Gmt->Fill(bxGmtpo);  hTimingL1_Prof->Fill(ptMu,bxGmtpo);}
        if (bxGmtpre< 0 && bxGmtpre>-999) {hTimingL1_Gmt->Fill(bxGmtpre); hTimingL1_Prof->Fill(ptMu, bxGmtpre);}
      }
    }
  }

}
