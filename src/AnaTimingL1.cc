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
  TH1D *hTimingL1_Rpc, *hTimingL1_RpcEmu, *hTimingL1_Dt, *hTimingL1_Csc, *hTimingL1_Gmt, *hTimingL1_GmtEmu; 
  TH2D *hTimingL1_RpcVsDtOpen, *hTimingL1_RpcVsCscOpen, *hTimingL1_RpcVsDtTight, *hTimingL1_RpcVsCscTight;
  TH2D *hTimingL1_RpcVsDt_MuOK, *hTimingL1_RpcVsDt_MuBad, *hTimingL1_RpcVsDt_NoMu, 
       *hTimingL1_RpcVsCsc_MuOK , *hTimingL1_RpcVsCsc_MuBad, *hTimingL1_RpcVsCsc_NoMu, 
       *hTimingL1_RpcVsDt_noRPCBX0, *hTimingL1_RpcVsCsc_noRPCBX0;
//  TProfile *hTimingL1_Prof;
//  TH1D *hTimingL1_RMS;
  TH1D *hTimingL1_DR_Dt, *hTimingL1_DR_Rpc, *hTimingL1_DR_Csc, *hTimingL1_DR_Gmt;
  TH1D *hTimingL1_preFire_GmtEmu, *hTimingL1_preFire_GmtEmu_D;
  TH1D *hTimingL1_postFire_GmtEmu, *hTimingL1_postFire_GmtEmu_D;

//  std::map<std::string, TH1D* > hmap;

  std::vector<double> drs = {9999, 0.5, 0.3, 0.2, 0.1, 0.06};
  std::vector<std::string> tags = {"Rpc","Dt","Csc","Gmt","RpcEmu","GmtEmu"};
  const unsigned int nptBins = 7;
//  double ptBins[nptBins+1]={0.1, 5., 7., 12., 16., 20., 25.,161.};
  double ptBins[nptBins+1]={0.1,   4., 6., 10., 16., 25., 50.,161.};
  const unsigned int nptCuts = 3;
  double ptCuts[nptCuts+1]={0., 6., 16.};

  TH1D* hL1ptBxForDR[6][nptBins][nptCuts]={ { {0} } };
  TH1D* hL1ptBxForDR_noBXm2[6][nptBins][nptCuts]={ { {0} } };
  TH1D* hL1ptBxForDR_noBXp2[6][nptBins][nptCuts]={ { {0} } };
  TH1D* hL1DRForPt[6][6] = {{0}};

  static float epsilon = 1.e-6;
}

void AnaTimingL1::init(TObjArray& histos)
{

  //
  // timging for events with GMTQ5-7, ptGMT > threshold;
  //
  hTimingL1_Rpc = new TH1D("hTimingL1_Rpc","hTimingL1_Rpc", 5,-2.5,2.5);   hTimingL1_Rpc->Sumw2(); histos.Add(hTimingL1_Rpc);
  hTimingL1_RpcEmu = new TH1D("hTimingL1_RpcEmu","hTimingL1_RpcEmu", 5,-2.5,2.5);   hTimingL1_RpcEmu->Sumw2(); histos.Add(hTimingL1_RpcEmu);
  hTimingL1_Dt  = new TH1D("hTimingL1_Dt","hTimingL1_Dt",   5,-2.5,2.5);   hTimingL1_Dt->Sumw2();  histos.Add(hTimingL1_Dt);
  hTimingL1_Csc = new TH1D("hTimingL1_Csc","hTimingL1_Csc", 5,-2.5,2.5);   hTimingL1_Csc->Sumw2(); histos.Add(hTimingL1_Csc);
  hTimingL1_GmtEmu = new TH1D("hTimingL1_GmtEmu","hTimingL1_GmtEmu", 5,-2.5,2.5);   hTimingL1_GmtEmu->Sumw2(); histos.Add(hTimingL1_GmtEmu);
  hTimingL1_Gmt = new TH1D("hTimingL1_Gmt","hTimingL1_Gmt", 5,-2.5,2.5);   hTimingL1_Gmt->Sumw2(); histos.Add(hTimingL1_Gmt);

  hTimingL1_RpcVsDtOpen  = new TH2D("hTimingL1_RpcVsDtOpen", "hTimingL1_RpcVsDtOpen", 5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsDtOpen);
  hTimingL1_RpcVsCscOpen = new TH2D("hTimingL1_RpcVsCscOpen","hTimingL1_RpcVsCscOpen",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsCscOpen);
  hTimingL1_RpcVsDtTight  = new TH2D("hTimingL1_RpcVsDtTight", "hTimingL1_RpcVsDtTight", 5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsDtTight);
  hTimingL1_RpcVsCscTight = new TH2D("hTimingL1_RpcVsCscTight","hTimingL1_RpcVsCscTight",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsCscTight);

  hTimingL1_RpcVsDt_MuOK = new TH2D("hTimingL1_RpcVsDt_MuOK","hTimingL1_RpcVsDt_MuOK",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsDt_MuOK);
  hTimingL1_RpcVsDt_MuBad = new TH2D("hTimingL1_RpcVsDt_MuBad","hTimingL1_RpcVsDt_MuBad",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsDt_MuBad);
  hTimingL1_RpcVsDt_NoMu = new TH2D("hTimingL1_RpcVsDt_NoMu","hTimingL1_RpcVsDt_NoMu",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsDt_NoMu);
  hTimingL1_RpcVsCsc_MuOK = new TH2D("hTimingL1_RpcVsCsc_MuOK","hTimingL1_RpcVsCsc_MuOK",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsCsc_MuOK);
  hTimingL1_RpcVsCsc_MuBad = new TH2D("hTimingL1_RpcVsCsc_MuBad","hTimingL1_RpcVsCsc_MuBad",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsCsc_MuBad);
  hTimingL1_RpcVsCsc_NoMu = new TH2D("hTimingL1_RpcVsCsc_NoMu","hTimingL1_RpcVsCsc_NoMu",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsCsc_NoMu);
  hTimingL1_RpcVsDt_noRPCBX0 = new TH2D("hTimingL1_RpcVsDt_noRPCBX0","hTimingL1_RpcVsDt_noRPCBX0",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsDt_noRPCBX0);
  hTimingL1_RpcVsCsc_noRPCBX0 = new TH2D("hTimingL1_RpcVsCsc_noRPCBX0","hTimingL1_RpcVsCsc_noRPCBX0",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsCsc_noRPCBX0);

  hTimingL1_DR_Dt  = new TH1D("hTimingL1_DR_Dt", "hTimingL1_DR_Dt", 80,0.,0.8);histos.Add(hTimingL1_DR_Dt); 
  hTimingL1_DR_Rpc = new TH1D("hTimingL1_DR_Rpc","hTimingL1_DR_Rpc",80,0.,0.8);histos.Add(hTimingL1_DR_Rpc); 
  hTimingL1_DR_Csc = new TH1D("hTimingL1_DR_Csc","hTimingL1_DR_Csc",80,0.,0.8);histos.Add(hTimingL1_DR_Csc); 
  hTimingL1_DR_Gmt = new TH1D("hTimingL1_DR_Gmt","hTimingL1_DR_Gmt",80,0.,0.8);histos.Add(hTimingL1_DR_Gmt); 

//  const unsigned int nBins = 5;
//  double myPtBins[nBins+1]={5., 7., 12.,16., 20., 25.};
//  hTimingL1_Prof = new TProfile("hTimingL1_Prof","hTimingL1_Prof",nBins, myPtBins,"s"); histos.Add(hTimingL1_Prof);
//  hTimingL1_RMS = new TH1D("hTimingL1_RMS","hTimingL1_RMS",nBins, myPtBins); histos.Add(hTimingL1_RMS);

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
  
  // pre+post firing for L1paper
  hTimingL1_preFire_GmtEmu = new TH1D("hTimingL1_preFire_GmtEmu","hTimingL1_preFire_GmtEmu", nptBins, ptBins);  histos.Add(hTimingL1_preFire_GmtEmu);
  hTimingL1_preFire_GmtEmu_D = new TH1D("hTimingL1_preFire_GmtEmu_D","hTimingL1_preFire_GmtEmu_D", nptBins, ptBins);  histos.Add(hTimingL1_preFire_GmtEmu_D);
  hTimingL1_postFire_GmtEmu   = new TH1D("hTimingL1_postFire_GmtEmu",  "hTimingL1_postFire_GmtEmu",   nptBins, ptBins);  histos.Add(hTimingL1_postFire_GmtEmu);
  hTimingL1_postFire_GmtEmu_D = new TH1D("hTimingL1_postFire_GmtEmu_D","hTimingL1_postFire_GmtEmu_D", nptBins, ptBins);  histos.Add(hTimingL1_postFire_GmtEmu_D);

}

void  AnaTimingL1::resume(TObjArray& histos)
{
/*
  for (int ibin=1; ibin<=hTimingL1_Prof->GetNbinsX(); ibin++) {
    double v=hTimingL1_Prof->GetBinError(ibin);
    hTimingL1_RMS->SetBinContent(ibin, v);
    hTimingL1_RMS->SetBinError(ibin, 0.);
  }
*/

}

void  AnaTimingL1::run(const EventObj* ev_noBx, const MuonObj* muon, const L1ObjColl *l1Coll)
{
  std::cout<<*l1Coll<<std::endl;
  const EventObjBXExtra *ev = dynamic_cast<const EventObjBXExtra*>(ev_noBx);
  if (!ev) return;


  bool dbg = false;
  static double l1ptCutForDR = theConfig.getParameter<double>("l1ptCutForDR");
  static double l1DRCutForpT = theConfig.getParameter<double>("l1DRCutForpT");

  //
  // additional check for DT prefireing
  //
  {
    L1ObjColl collGMT = l1Coll->selectByType(L1Obj::GMT).selectByQuality(4,7).selectByPtMin(l1ptCutForDR).selectByBx(0,0);
    L1ObjColl collDT  = l1Coll->selectByType(L1Obj::DT);
    L1ObjColl collCSC = l1Coll->selectByType(L1Obj::CSC);
    L1ObjColl collRPC = l1Coll->l1RpcColl();
  
    if (collGMT) {
      bool matchingMu = collGMT.isMatching_DRBx(0.3, 0, l1ptCutForDR);
      double eta = collGMT.getL1Objs()[0].eta;
      double phi = collGMT.getL1Objs()[0].phi;
      bool noRPCBX0   = !collRPC.isMatching_DRBx_At(0.3, 0, 0., eta,phi); 
      for (int ibxRPC=-2; ibxRPC <=2; ibxRPC++) {
        if (collRPC.isMatching_DRBx_At(0.3, ibxRPC, 0., eta,phi) ) {
          for (int ibxDTCSC=-2; ibxDTCSC <=2; ibxDTCSC++) {
            if (collDT.isMatching_DRBx_At(0.3, ibxDTCSC, 0., eta,phi) ) {
              if (ibxDTCSC==0 && noRPCBX0) hTimingL1_RpcVsDt_noRPCBX0->Fill(0.,0.);
              if (muon->nAllMuons==0)                  hTimingL1_RpcVsDt_NoMu->Fill(ibxRPC, ibxDTCSC);
              else if (muon->isGlobal() && matchingMu) hTimingL1_RpcVsDt_MuOK->Fill(ibxRPC, ibxDTCSC);
              else                                     hTimingL1_RpcVsDt_MuBad->Fill(ibxRPC, ibxDTCSC);
              if (noRPCBX0) hTimingL1_RpcVsDt_noRPCBX0->Fill(ibxRPC, ibxDTCSC);

              if (noRPCBX0 && ibxDTCSC==0 && ibxRPC==2) {
                std::cout << *ev << std::endl;
                      std::cout<<*l1Coll<<std::endl;
                      std::cout <<*muon<<std::endl;
              }
            }
            if (collCSC.isMatching_DRBx_At(0.3, ibxDTCSC, 0., eta,phi) ) {
              if (ibxDTCSC==0 && noRPCBX0) hTimingL1_RpcVsCsc_noRPCBX0->Fill(0.,0.);
              if (muon->nAllMuons==0)                  hTimingL1_RpcVsCsc_NoMu->Fill(ibxRPC, ibxDTCSC);
              else if (muon->isGlobal() && matchingMu) hTimingL1_RpcVsCsc_MuOK->Fill(ibxRPC, ibxDTCSC);
              else                                     hTimingL1_RpcVsCsc_MuBad->Fill(ibxRPC, ibxDTCSC);
              if (noRPCBX0) hTimingL1_RpcVsCsc_noRPCBX0->Fill(ibxRPC, ibxDTCSC);
            }
          } 
        }
      }
    }
  }

  //
  // main 1D histos, BX vs DR and BX vs PT 
  //
  L1ObjColl collGMT = l1Coll->selectByType(L1Obj::GMT).selectByQuality(4,7).selectByDeltaR(l1DRCutForpT).selectByPtMin(l1ptCutForDR);


  if (!muon->isGlobal()) return;
  for (unsigned int itag=0; itag < tags.size(); itag++) {
    L1ObjColl coll;
    if ( tags[itag] == "Rpc"    ) coll = l1Coll->l1RpcColl();  
    if ( tags[itag] == "RpcEmu" ) coll = l1Coll->l1RpcCollEmu();  
    if ( tags[itag] == "Dt"     ) coll = l1Coll->selectByType(L1Obj::DT);
    if ( tags[itag] == "Csc"    ) coll = l1Coll->selectByType(L1Obj::CSC);
    if ( tags[itag] == "Gmt"    ) coll = l1Coll->selectByType(L1Obj::GMT).selectByQuality(4,7);
    if ( tags[itag] == "GmtEmu" ) coll = l1Coll->selectByType(L1Obj::GMT_emu).selectByQuality(4,7);
    if (!coll) continue;

    //
    // main timing histo for paper, for events with GMT ptGMT> threshold (16), dr < drCut (0.3) timing profile off candidates
    //
    if (collGMT) {
      for (int ibx=-2; ibx <=2; ibx++) {
        if (coll.isMatching_DRBx(l1DRCutForpT, ibx, l1ptCutForDR)) {
          if ( tags[itag] == "Gmt"    ) hTimingL1_Gmt->Fill(ibx);
//          if (ibx == -2 && tags[itag] == "Gmt" ) dbg = true;
        }
//      if (coll.isMatching_DRBx_At(l1DRCutForpT, ibx, l1ptCutForDR,muon->eta(),muon->phi())) {
        if (coll.isMatching_DRBx(l1DRCutForpT, ibx, l1ptCutForDR)) {
//          if (ibx==-2 &&  (tags[itag] == "Csc" || tags[itag] == "RpcEmu") ) dbg = true;
          if ( tags[itag] == "Rpc")     hTimingL1_Rpc->Fill(ibx); 
          if ( tags[itag] == "RpcEmu" ) hTimingL1_RpcEmu->Fill(ibx);
          if ( tags[itag] == "Dt"     ) hTimingL1_Dt->Fill(ibx); 
          if ( tags[itag] == "Csc"    ) hTimingL1_Csc->Fill(ibx);
  //        if ( tags[itag] == "Gmt"    ) hTimingL1_Gmt->Fill(ibx);
          if ( tags[itag] == "GmtEmu" ) hTimingL1_GmtEmu->Fill(ibx);
        } 
      }
    }

    
    // timing for different DR
    L1ObjColl collPt = coll.selectByPtMin(l1ptCutForDR);
    if (collPt) {
      for (std::vector<double>::const_iterator idr = drs.begin(); idr != drs.end(); idr++) {
        for (int ibx=-2; ibx <=2; ibx++) {
          if (collPt.isMatching_DRBx(*idr,ibx)) hL1DRForPt[itag][idr-drs.begin()]->Fill(ibx);
        }
      }
    }
    
    L1ObjColl collDR = coll.selectByDeltaR( l1DRCutForpT);
    if (collDR) { 
    //
    // timing in given bx(pre-,post-fire) vs L1pt_bin (ptmu > L1pt_bin) for GmtEmu ( L1performance paper)
    //
    if (tags[itag] == "GmtEmu" ) {
      for (unsigned int iptL1=0; iptL1 < nptBins; iptL1++) {
        if (collDR.isMatching_PtminPtmaxBx(ptBins[iptL1], ptBins[iptL1+1],0, true)) {
          hTimingL1_preFire_GmtEmu_D->Fill(ptBins[iptL1]+epsilon);
          if (muon->pt() > ptBins[iptL1]) hTimingL1_postFire_GmtEmu_D->Fill(ptBins[iptL1]+epsilon);
        } 
        if (collDR.isMatching_PtminPtmaxBx(ptBins[iptL1], ptBins[iptL1+1], -1, true)) {
          hTimingL1_preFire_GmtEmu->Fill(ptBins[iptL1]+epsilon);
        } 
        if (collDR.isMatching_PtminPtmaxBx(ptBins[iptL1], ptBins[iptL1+1],  1, true)) {
          if (muon->pt() > ptBins[iptL1]) hTimingL1_postFire_GmtEmu->Fill(ptBins[iptL1]+epsilon);
        }
      }
    }
    //
    // timing for different L1pt
    //
      for (unsigned int iptL1=0; iptL1 < nptBins; iptL1++) {
        for (int ibx=-2; ibx <=2; ibx++) {
          if (!collDR.isMatching_PtminPtmaxBx(ptBins[iptL1], ptBins[iptL1+1],ibx, false)) continue; 
          for (unsigned int iptMu=0; iptMu < nptCuts; iptMu++) {
            if (muon->pt() < ptBins[iptL1]) continue;
            hL1ptBxForDR[itag][iptL1][iptMu]->Fill(ibx); 
            if (!ev->hasValidBX_Minus2) hL1ptBxForDR_noBXm2[itag][iptL1][iptMu]->Fill(ibx);
            if (!ev->hasValidBX_Plus2)  hL1ptBxForDR_noBXp2[itag][iptL1][iptMu]->Fill(ibx);
//            if (ptBins[iptL1] >= 16. && ibx == -2 ) dbg = true;
//            if (ptBins[iptL1] >= 16. && ibx == -2 ) dbg = true;
//            if (ptBins[iptL1] >= 16. && ibx == 2  ) dbg = true;
//            if (ibx == 1 && tags[itag] == "GmtEmu" && ptBins[iptL1] >= 16. ) dbg = true; 
          }
        }
      } 
    }

    //
    // DR histos
    //
    const std::vector<double> deltaR = coll.selectByBx( 0, 0).getL1ObjDeltaR();
    double minDeltaR = 100.;
    for (unsigned int i=0; i< deltaR.size(); ++i) if (deltaR[i] < minDeltaR) minDeltaR = deltaR[i];
    if (minDeltaR < 1.) {
    if (tags[itag]=="Rpc") hTimingL1_DR_Rpc->Fill(minDeltaR);
    if (tags[itag]=="Dt" ) hTimingL1_DR_Dt->Fill(minDeltaR);
    if (tags[itag]=="Csc") hTimingL1_DR_Csc->Fill(minDeltaR);
    if (tags[itag]=="Gmt") hTimingL1_DR_Gmt->Fill(minDeltaR);
    }
  }

  if (dbg) {
  std::cout << "EVENT: " << " id: "<<ev->id
                         << " bx: "<<ev->bx
                         << " val: "<<ev->hasValidBX
                         << " valm2: "<< ev->hasValidBX_Minus2
                         << " valp2: "<< ev->hasValidBX_Plus2 << std::endl;
  std::cout <<" Muon: pt="<<muon->pt()<<" eta: "<<muon->eta()<<" phi: "<<muon->phi() << std::endl;
  std::cout <<*l1Coll << std::endl;
                        
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

}
