#include "EfficiencyAnalysis.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"


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

  histos.Add(new TH1D("hDetsB_100","hDetsB_100",7,0.,7.));
  histos.Add(new TH1D("hDetsE_100","hDetsE_100",7,0.,7.));

  // efficiency for hits
  histos.Add(new TH1D("hEfficHits_N","hEfficHits_N",32,-1.6,1.6));
  histos.Add(new TH1D("hEfficHits_H","hEfficHits_H",32,-1.6,1.6));
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

  histos.Add(new TH2D("hDistL1Rpc","hDistL1Rpc",144,0,2*M_PI,33,-1.6,1.6));
  histos.Add(new TH2D("hDistL1Other","hDistL1Other",144,0,2*M_PI,33,-1.6,1.6));

  // simple efficiency variable to be 
  histos.Add(new TH1D("hL1RpcBX","hL1RpcBX",5,-2.5,2.5));

  // define chain
  TChain chain("tL1Rpc");
  std::vector<std::string> treeFileNames = theConfig.getParameter<std::vector<std::string> >("treeFileNames");
  for (std::vector<std::string>::const_iterator it = treeFileNames.begin(); it != treeFileNames.end(); ++it)  chain.Add((*it).c_str() );

  std::vector<bool> *hitBarrel = 0;
  std::vector<bool> *hitEndcap = 0;
  std::vector<unsigned int> *detBarrel = 0;
  std::vector<unsigned int> *detEndcap = 0;

  EventObj * event = 0;
  TrackObj * track = 0;
  MuonObj * muon = 0;

  TBranch *bhitBarrel=0;
  TBranch *bhitEndcap=0;
  TBranch *bdetBarrel=0;
  TBranch *bdetEndcap=0;

  L1ObjColl* l1RpcColl = 0;
  L1ObjColl* l1OtherColl = 0;
  
  chain.SetBranchAddress("event",&event);

  chain.SetBranchAddress("muon",&muon);
  chain.SetBranchAddress("track",&track);

  chain.SetBranchAddress("hitBarrel",&hitBarrel, &bhitBarrel);
  chain.SetBranchAddress("hitEndcap",&hitEndcap, &bhitEndcap);
  chain.SetBranchAddress("detBarrel",&detBarrel, &bdetBarrel);
  chain.SetBranchAddress("detEndcap",&detEndcap, &bdetEndcap);

  chain.SetBranchAddress("l1RpcColl",&l1RpcColl);
  chain.SetBranchAddress("l1OtherColl",&l1OtherColl);

  Int_t nentries = (Int_t) chain.GetEntries();
  std::cout <<" ENTRIES: " << nentries << std::endl;

//  float l1Cut = theConfig.getParameter<double>("l1Cut");

  for (int ev=0; ev<nentries; ev++) {
    chain.GetEntry(ev);
    std::vector<L1Obj> l1Rpcs = l1RpcColl->getL1ObjsMatched(); 
    if (l1Rpcs.size() > 0) {
      int firstBX = 100;
      for (std::vector<L1Obj>::const_iterator it=l1Rpcs.begin(); it!= l1Rpcs.end(); ++it) if ( (it->bx) < firstBX) firstBX = it->bx;
      static_cast<TH1D* >(histos.FindObject("hL1RpcBX"))->Fill(firstBX);
     }
      

    std::cout<< "EV: "<<event->id<<" run: "<< event->run<< " RPC: "<< l1Rpcs.size()<<" rand: "<<(float) rand()/RAND_MAX;
    std::cout << std::endl;

    bool isMuon = (muon->pt() > theConfig.getParameter<double>("ptMin") );
    if(isMuon) {
      //if (fabs(muon.eta) > 0.8) continue;
      //if (fabs(muon.eta) < 1.24) continue;

      static_cast<TH1D* >(histos.FindObject("hMuonPt"))->Fill(muon->pt());
      static_cast<TH1D* >(histos.FindObject("hMuonEta"))->Fill(muon->eta());
      static_cast<TH1D* >(histos.FindObject("hMuonPhi"))->Fill(muon->phi());

      int nHitsB = 0; for (int i=0; i<6; i++) if( hitBarrel->at(i) ) nHitsB++;
      int nHitsBL= 0; for (int i=0; i<4; i++) if( hitBarrel->at(i) ) nHitsBL++;
      int nHitsE = 0; for (int i=0; i<3; i++) if( hitEndcap->at(i) ) nHitsE++;

      int nDetsB = 0;  for (int i=0; i<6; i++) if( detBarrel->at(i) ) nDetsB++;
      int nDetsE = 0;  for (int i=0; i<3; i++) if( detEndcap->at(i) ) nDetsE++;

      if (fabs(muon->eta()) < 0.8) {
        static_cast<TH1D* >(histos.FindObject("hHitsB"))->Fill(nHitsB);
        static_cast<TH1D* >(histos.FindObject("hDetsB_100"))->Fill(nDetsB);
        static_cast<TH1D* >(histos.FindObject("hEfficHits_D"))->Fill(muon->eta());      
        if (nHitsBL>=3 || (nHitsB>=4 && (hitBarrel->at(4)||hitBarrel->at(5)) ) ) {
          static_cast<TH1D* >(histos.FindObject("hEfficHits_N"))->Fill(muon->eta());
          if (l1Rpcs.size()>0) static_cast<TH1D* >(histos.FindObject("hEfficHits_H"))->Fill(muon->eta());
        }
      }
      if (fabs(muon->eta()) > 1.25 && fabs(muon->eta()) < 1.55) { 
        static_cast<TH1D* >(histos.FindObject("hHitsE"))->Fill(nHitsE);
        static_cast<TH1D* >(histos.FindObject("hDetsE_100"))->Fill(nDetsE);
        static_cast<TH1D* >(histos.FindObject("hEfficHits_D"))->Fill(muon->eta());      
        if (nHitsE==3) {
          static_cast<TH1D* >(histos.FindObject("hEfficHits_N"))->Fill(muon->eta());
          if (l1Rpcs.size()) static_cast<TH1D* >(histos.FindObject("hEfficHits_H"))->Fill(muon->eta());
        }
      }

      static_cast<TH1D* >(histos.FindObject("hEfficMu_D"))->Fill(muon->eta());
      if (l1Rpcs.size()) static_cast<TH1D* >(histos.FindObject("hEfficMu_N"))->Fill(muon->eta()); 

      static_cast<TH1D* >(histos.FindObject("hEfficMuPt_D"))->Fill(muon->pt());
      if (l1Rpcs.size())      static_cast<TH1D* >(histos.FindObject("hEfficMuPt_N"))->Fill(muon->pt());
      if (l1RpcColl->getL1ObjsMatched(7.).size()) static_cast<TH1D* >(histos.FindObject("hEfficMuPt7_N"))->Fill(muon->pt()); 
    }

    bool isTrack = (track->pt() > theConfig.getParameter<double>("ptMin") );
    if (isTrack) {
      //if (fabs(track.eta) > 0.8) continue;
      //if (fabs(track.eta) < 1.24) continue;
      static_cast<TH1D* >(histos.FindObject("hTrackPt"))->Fill(track->pt());
      static_cast<TH1D* >(histos.FindObject("hTrackEta"))->Fill(track->eta());
      static_cast<TH1D* >(histos.FindObject("hTrackPhi"))->Fill(track->phi());

      static_cast<TH1D* >(histos.FindObject("hEfficTk_D"))->Fill(track->eta());
      if (l1Rpcs.size()) static_cast<TH1D* >(histos.FindObject("hEfficTk_N"))->Fill(track->eta()); 

      static_cast<TH1D* >(histos.FindObject("hEfficTkPt_D"))->Fill(track->pt());
      if (l1Rpcs.size())     static_cast<TH1D* >(histos.FindObject("hEfficTkPt_N"))->Fill(track->pt());
      if (l1RpcColl->getL1ObjsMatched(7.).size()) static_cast<TH1D* >(histos.FindObject("hEfficTkPt7_N"))->Fill(track->pt()); 
    }

  
    if (l1Rpcs.size()) static_cast<TH2D* >(histos.FindObject("hDistL1Rpc"))->Fill(l1Rpcs[0].phi, l1Rpcs[0].eta);
    std::vector<L1Obj> l1Others = l1OtherColl->getL1Objs(); 
    if (l1Others.size()) static_cast<TH2D* >(histos.FindObject("hDistL1Other"))->Fill(l1Others[0].phi, l1Others[0].eta);
  }


   

//  chain.ResetBranchAddresses(); 

  std::string histoFile = theConfig.getParameter<std::string>("histoFileName");
  TFile f(histoFile.c_str(),"RECREATE");
  histos.Write();
  f.Close();
 
  cout <<"KUKU"<<endl;
}
