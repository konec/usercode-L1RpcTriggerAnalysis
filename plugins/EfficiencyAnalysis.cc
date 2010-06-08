#include "EfficiencyAnalysis.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"


#include <vector>
#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TProfile.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TH1D.h"


using namespace std;

void EfficiencyAnalysis::beginJob()
{
  TObjArray histos;
  histos.SetOwner();

  //muon kine 
  histos.Add(new TH1D("hMuonPt","hMuonPt",50,2.,15.));
  histos.Add(new TH1D("hMuonEta","hMuonEta",32,-1.6,1.6));
  histos.Add(new TH1D("hMuonPhi","hMuonPhi",50,-M_PI,M_PI));

  //track kine
  histos.Add(new TH1D("hTrackPt","hTrackPt",50,2.,15.));
  histos.Add(new TH1D("hTrackEta","hTrackEta",32,-1.6,1.6));
  histos.Add(new TH1D("hTrackPhi","hTrackPhi",50,-M_PI,M_PI));

  // hit histos;
  histos.Add(new TH1D("hHitsB","hHitsB",7,0.,7.));
  histos.Add(new TH1D("hHitsE","hHitsE",4,0.,4.));

  // efficiency for hits
  histos.Add(new TH1D("hEfficHits_N","hEfficHits_N",32,-1.6,1.6));
  histos.Add(new TH1D("hEfficHits_D","hEfficHits_D",32,-1.6,1.6));

  // efficiency for trigger wrt Mu 
  histos.Add(new TH1D("hEfficMu_N","hEfficMu_N",32,-1.6,1.6));
  histos.Add(new TH1D("hEfficMu_D","hEfficMu_D",32,-1.6,1.6));

  // efficiency for trigger wrt L1Other
  histos.Add(new TH1D("hEfficTk_N","hEfficTk_N",32,-1.6,1.6));
  histos.Add(new TH1D("hEfficTk_D","hEfficTk_D",32,-1.6,1.6));

  // efficiency for  L1rpc vs Pt 
  histos.Add(new TH1D("hEfficMuPt7_N","hEfficMuPt7_N",50,2.,15.));
  histos.Add(new TH1D("hEfficMuPt_N","hEfficMuPt_N",50,2.,15.));
  histos.Add(new TH1D("hEfficMuPt_D","hEfficMuPt_D",50,2.,15.));

  // efficiency for  L1rpc vs Pt 
  histos.Add(new TH1D("hEfficTkPt7_N","hEfficTkPt7_N",50,2.,15.));
  histos.Add(new TH1D("hEfficTkPt_N","hEfficTkPt_N",50,2.,15.));
  histos.Add(new TH1D("hEfficTkPt_D","hEfficTkPt_D",50,2.,15.));

  // simple efficiency variable to be 

  // define chain
  TChain chain("tL1Rpc");
  std::vector<std::string> treeFileNames = theConfig.getParameter<std::vector<std::string> >("treeFileNames");
  for (std::vector<std::string>::const_iterator it = treeFileNames.begin(); it != treeFileNames.end(); ++it)  chain.Add((*it).c_str() );

  std::vector<bool> *hitBarrel = 0;
  std::vector<bool> *hitEndcap = 0;

  L1Obj l1Rpc;
  L1Obj l1Other;
  EventObj event;
  TrackObj track;
  TrackObj muon;

  TBranch *bhitBarrel=0;
  TBranch *bhitEndcap=0;

  chain.SetBranchAddress("evBx",&event.bx);
  chain.SetBranchAddress("evId",&event.id);
  chain.SetBranchAddress("evRun",&event.run);
  chain.SetBranchAddress("evLumi",&event.lumi);

  chain.SetBranchAddress("hitBarrel",&hitBarrel, &bhitBarrel);
  chain.SetBranchAddress("hitEndcap",&hitEndcap, &bhitEndcap);

  chain.SetBranchAddress("muPt",&muon.pt);
  chain.SetBranchAddress("muEta",&muon.eta);
  chain.SetBranchAddress("muPhi",&muon.phi);

  chain.SetBranchAddress("tkPt",&track.pt);
  chain.SetBranchAddress("tkEta",&track.eta);
  chain.SetBranchAddress("tkPhi",&track.phi);

  chain.SetBranchAddress("rpcPt", &l1Rpc.pt);
  chain.SetBranchAddress("rpcEta",&l1Rpc.eta);
  chain.SetBranchAddress("rpcPhi",&l1Rpc.phi);
  chain.SetBranchAddress("rpcBx",&l1Rpc.bx);
  chain.SetBranchAddress("rpcQ",&l1Rpc.q);

  chain.SetBranchAddress("otherPt", &l1Other.pt);
  chain.SetBranchAddress("otherEta",&l1Other.eta);
  chain.SetBranchAddress("otherPhi",&l1Other.phi);
  chain.SetBranchAddress("otherBx",&l1Other.bx);
  chain.SetBranchAddress("otherQ",&l1Other.q);


  Int_t nentries = (Int_t) chain.GetEntries();
  std::cout <<" ENTRIES: " << nentries << std::endl;

//  float l1Cut = theConfig.getParameter<double>("l1Cut");

  for (int ev=0; ev<nentries; ev++) {
    chain.GetEntry(ev);

    bool isMuon = (muon.pt > theConfig.getParameter<double>("ptMin") );
    if(isMuon) {
      //if (fabs(muon.eta) > 0.8) continue;
      if (fabs(muon.eta) < 1.24) continue;

      static_cast<TH1D* >(histos.FindObject("hMuonPt"))->Fill(muon.pt);
      static_cast<TH1D* >(histos.FindObject("hMuonEta"))->Fill(muon.eta);
      static_cast<TH1D* >(histos.FindObject("hMuonPhi"))->Fill(muon.phi);

      int nB = 0; for (int i=0; i<6; i++) if( hitBarrel->at(i) ) nB++;
      int nBL= 0; for (int i=0; i<4; i++) if( hitBarrel->at(i) ) nBL++;
      int nE = 0; for (int i=0; i<3; i++) if( hitEndcap->at(i) ) nE++;
      if (fabs(muon.eta) < 0.8) {
        static_cast<TH1D* >(histos.FindObject("hHitsB"))->Fill(nB);
        static_cast<TH1D* >(histos.FindObject("hEfficHits_D"))->Fill(muon.eta);      
        if (nBL>=3 || (nB>=4 && (hitBarrel->at(4)||hitBarrel->at(5)) ) ) static_cast<TH1D* >(histos.FindObject("hEfficHits_N"))->Fill(muon.eta);
      }
      if (fabs(muon.eta) > 1.25 && fabs(muon.eta) < 1.55) { 
        static_cast<TH1D* >(histos.FindObject("hHitsE"))->Fill(nE);
        static_cast<TH1D* >(histos.FindObject("hEfficHits_D"))->Fill(muon.eta);      
        if (nE==3) static_cast<TH1D* >(histos.FindObject("hEfficHits_N"))->Fill(muon.eta);
      }

      static_cast<TH1D* >(histos.FindObject("hEfficMu_D"))->Fill(muon.eta);
      if (l1Rpc.q >= 0 ) static_cast<TH1D* >(histos.FindObject("hEfficMu_N"))->Fill(muon.eta); 

      static_cast<TH1D* >(histos.FindObject("hEfficMuPt_D"))->Fill(muon.pt);
      if (l1Rpc.q >= 0)      static_cast<TH1D* >(histos.FindObject("hEfficMuPt_N"))->Fill(muon.pt);
      if (l1Rpc.pt > 7.)  static_cast<TH1D* >(histos.FindObject("hEfficMuPt7_N"))->Fill(muon.pt);
    }

    bool isTrack = (track.pt > theConfig.getParameter<double>("ptMin") );
    if (isTrack) {
      //if (fabs(track.eta) > 0.8) continue;
      if (fabs(track.eta) < 1.24) continue;
      static_cast<TH1D* >(histos.FindObject("hTrackPt"))->Fill(track.pt);
      static_cast<TH1D* >(histos.FindObject("hTrackEta"))->Fill(track.eta);
      static_cast<TH1D* >(histos.FindObject("hTrackPhi"))->Fill(track.phi);

      static_cast<TH1D* >(histos.FindObject("hEfficTk_D"))->Fill(track.eta);
      if (l1Rpc.q >= 0 ) static_cast<TH1D* >(histos.FindObject("hEfficTk_N"))->Fill(track.eta); 

      static_cast<TH1D* >(histos.FindObject("hEfficTkPt_D"))->Fill(track.pt);
      if (l1Rpc.q >= 0)      static_cast<TH1D* >(histos.FindObject("hEfficTkPt_N"))->Fill(track.pt);
      if (l1Rpc.pt > 7.)  static_cast<TH1D* >(histos.FindObject("hEfficTkPt7_N"))->Fill(track.pt);
    }
  }

//  chain.ResetBranchAddresses(); 

  std::string histoFile = theConfig.getParameter<std::string>("histoFileName");
  TFile f(histoFile.c_str(),"RECREATE");
  histos.Write();
  f.Close();
 
  cout <<"KUKU"<<endl;
}
