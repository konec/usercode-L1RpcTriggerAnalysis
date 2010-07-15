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
#include "TGraphErrors.h"


using namespace std;

void EfficiencyAnalysis::beginJob()
{
  TObjArray histos;
  histos.SetOwner();

  //muon kine 
  TGraphErrors * hGraph = new TGraphErrors(); 
  TH1D * hMuonPt = new TH1D("hMuonPt","hMuonPt",50,2.,15.);  histos.Add(hMuonPt);
  TH1D * hMuonEta = new TH1D("hMuonEta","hMuonEta",32,-1.6,1.6);  histos.Add(hMuonEta);
    TH1D * hMuonPhi = new TH1D("hMuonPhi","hMuonPhi",50,-M_PI,M_PI);  histos.Add(hMuonPhi);
    //TH1D * hMuonPhi = new TH1D("hMuonPhi","hMuonPhi",200,-M_PI,M_PI);  histos.Add(hMuonPhi);

  //track kine
  TH1D * hTrackPt = new TH1D("hTrackPt","hTrackPt",50,2.,15.);  histos.Add(hTrackPt);
  TH1D * hTrackEta = new TH1D("hTrackEta","hTrackEta",32,-1.6,1.6);  histos.Add(hTrackEta);
   TH1D * hTrackPhi = new TH1D("hTrackPhi","hTrackPhi",50,-M_PI,M_PI);  histos.Add(hTrackPhi);
   // TH1D * hTrackPhi = new TH1D("hTrackPhi","hTrackPhi",200,-M_PI,M_PI);  histos.Add(hTrackPhi);

  // hit histos;
  TH1D * hHitsB = new TH1D("hHitsB","hHitsB",7,0.,7.);  histos.Add(hHitsB);
  TH1D * hHitsE = new TH1D("hHitsE","hHitsE",4,0.,4.);  histos.Add(hHitsE);

  TH1D * hDetsB_100 = new TH1D("hDetsB_100","hDetsB_100",7,0.,7.);  histos.Add(hDetsB_100);
  TH1D * hDetsE_100 = new TH1D("hDetsE_100","hDetsE_100",7,0.,7.);  histos.Add(hDetsE_100);

  // efficiency for hits
  TH1D * hEfficHits_N  = new TH1D("hEfficHits_N","hEfficHits_N",32,-1.6,1.6);  histos.Add(hEfficHits_N);
  TH1D * hEfficHits_H  = new TH1D("hEfficHits_H","hEfficHits_H",32,-1.6,1.6);  histos.Add(hEfficHits_H);
  TH1D * hEfficHits_D = new TH1D("hEfficHits_D","hEfficHits_D",32,-1.6,1.6);  histos.Add(hEfficHits_D);

  TH1D * hEfficChambBar_N = new TH1D("hEfficChambBar_N","hEfficChambBar_N",6,0.5,6.5); histos.Add(hEfficChambBar_N); 
  TH1D * hEfficChambBar_D = new TH1D("hEfficChambBar_D","hEfficChambBar_D",6,0.5,6.5); histos.Add(hEfficChambBar_D); 
  TH1D * hEfficChambEnd_N = new TH1D("hEfficChambEnd_N","hEfficChambEnd_N",3,0.5,3.5); histos.Add(hEfficChambEnd_N);
  TH1D * hEfficChambEnd_D = new TH1D("hEfficChambEnd_D","hEfficChambEnd_D",3,0.5,3.5); histos.Add(hEfficChambEnd_D); 

  // efficiency for trigger wrt Mu 
  TH1D * hEfficMu_N =  new TH1D("hEfficMu_N","hEfficMu_N",32,-1.6,1.6);  histos.Add(hEfficMu_N);
  TH1D * hEfficMu_D =  new TH1D("hEfficMu_D","hEfficMu_D",32,-1.6,1.6);  histos.Add(hEfficMu_D);

  // efficiency for trigger wrt L1Other
  TH1D * hEfficTk_N =  new TH1D("hEfficTk_N","hEfficTk_N",32,-1.6,1.6);  histos.Add(hEfficTk_N);
  TH1D * hEfficTk_D  = new TH1D("hEfficTk_D","hEfficTk_D",32,-1.6,1.6);  histos.Add(hEfficTk_D);

  // efficiency for  L1rpc vs Pt 
  TH1D * hEfficMuPt7_N = new TH1D("hEfficMuPt7_N","hEfficMuPt7_N",50,2.,15.);  histos.Add(hEfficMuPt7_N);
  TH1D * hEfficMuPt_N = new TH1D("hEfficMuPt_N","hEfficMuPt_N",50,2.,15.);  histos.Add(hEfficMuPt_N);
  TH1D * hEfficMuPt_D = new TH1D("hEfficMuPt_D","hEfficMuPt_D",50,2.,15.);  histos.Add(hEfficMuPt_D);

  // efficiency for  L1rpc vs Pt 
  TH1D * hEfficTkPt7_N = new TH1D("hEfficTkPt7_N","hEfficTkPt7_N",50,2.,15.);  histos.Add(hEfficTkPt7_N);
  TH1D * hEfficTkPt_N = new TH1D("hEfficTkPt_N","hEfficTkPt_N",50,2.,15.);  histos.Add(hEfficTkPt_N);
  TH1D * hEfficTkPt_D = new TH1D("hEfficTkPt_D","hEfficTkPt_D",50,2.,15.);  histos.Add(hEfficTkPt_D);

  TH2D* hDistL1Rpc   = new TH2D("hDistL1Rpc","hDistL1Rpc",145,0,2*M_PI,132,-1.6,1.6); histos.Add(hDistL1Rpc);
  TH2D* hDistL1Other = new TH2D("hDistL1Other","hDistL1Other",145,0,2*M_PI,132,-1.6,1.6); histos.Add(hDistL1Other);

  TH1D* hEffLumi = new TH1D("hEffLumi","hEffLumi",100,0.,1.); histos.Add(hEffLumi);

  TH1D* hL1RpcBX = new TH1D("hL1RpcBX","hL1RpcBX",5,-2.5,2.5); histos.Add(hL1RpcBX);
  TH1D* hL1RpcBX_only = new TH1D("hL1RpcBX_only","hL1RpcBX_only",5,-2.5,2.5); histos.Add(hL1RpcBX_only);
  TH1D* hL1RpcBX_all = new TH1D("hL1RpcBX_all","hL1RpcBX_all",5,-2.5,2.5); histos.Add(hL1RpcBX_all);

  TH1D* hL1RpcEta = new TH1D("hL1RpcEta","hL1RpcEta",132,1.6,1.6); histos.Add(hL1RpcEta);
  TH1D* hL1RpcPhi = new TH1D("hL1RpcPhi","hL1RpcPhi",290,0,2*M_PI); histos.Add(hL1RpcPhi);

  TH1D* hL1RpcEta_q0 = new TH1D("hL1RpcEta_q0","hL1RpcEta_q0",132,1.6,1.6); histos.Add(hL1RpcEta_q0);
  TH1D* hL1RpcPhi_q0 = new TH1D("hL1RpcPhi_q0","hL1RpcPhi_q0",290,0,2*M_PI); histos.Add(hL1RpcPhi_q0);
  TH1D* hL1RpcEta_q1 = new TH1D("hL1RpcEta_q1","hL1RpcEta_q1",132,1.6,1.6); histos.Add(hL1RpcEta_q1);
  TH1D* hL1RpcPhi_q1 = new TH1D("hL1RpcPhi_q1","hL1RpcPhi_q1",290,0,2*M_PI); histos.Add(hL1RpcPhi_q1);
  TH1D* hL1RpcEta_q2 = new TH1D("hL1RpcEta_q2","hL1RpcEta_q2",132,1.6,1.6); histos.Add(hL1RpcEta_q2);
  TH1D* hL1RpcPhi_q2 = new TH1D("hL1RpcPhi_q2","hL1RpcPhi_q2",290,0,2*M_PI); histos.Add(hL1RpcPhi_q2);
  TH1D* hL1RpcEta_q3 = new TH1D("hL1RpcEta_q3","hL1RpcEta_q3",132,1.6,1.6); histos.Add(hL1RpcEta_q3);
  TH1D* hL1RpcPhi_q3 = new TH1D("hL1RpcPhi_q3","hL1RpcPhi_q3",290,0,2*M_PI); histos.Add(hL1RpcPhi_q3);

  TH2D* h2L1RpcBX1   = new TH2D("h2L1RpcBX1","h2L1RpcBX1",132,-1.6,1.6,145,0,2*M_PI); histos.Add(h2L1RpcBX1);
  TH2D* h2L1RpcBX1_q0   = new TH2D("h2L1RpcBX1_q0","h2L1RpcBX1_q0",132,-1.6,1.6,145,0,2*M_PI); histos.Add(h2L1RpcBX1_q0);
  TH2D* h2L1RpcBX1_q1   = new TH2D("h2L1RpcBX1_q1","h2L1RpcBX1_q1",132,-1.6,1.6,145,0,2*M_PI); histos.Add(h2L1RpcBX1_q1);
  TH2D* h2L1RpcBX1_q2   = new TH2D("h2L1RpcBX1_q2","h2L1RpcBX1_q2",132,-1.6,1.6,145,0,2*M_PI); histos.Add(h2L1RpcBX1_q2);
  TH2D* h2L1RpcBX1_q3   = new TH2D("h2L1RpcBX1_q3","h2L1RpcBX1_q3",132,-1.6,1.6,145,0,2*M_PI); histos.Add(h2L1RpcBX1_q3);

  TH2D* h2L1RpcBX   = new TH2D("h2L1RpcBX","h2L1RpcBX",132,-1.6,1.6,145,0,2*M_PI); histos.Add(h2L1RpcBX);
  TH2D* h2L1RpcBX_q0   = new TH2D("h2L1RpcBX_q0","h2L1RpcBX_q0",132,-1.6,1.6,145,0,2*M_PI); histos.Add(h2L1RpcBX_q0);
  TH2D* h2L1RpcBX_q1   = new TH2D("h2L1RpcBX_q1","h2L1RpcBX_q1",132,-1.6,1.6,145,0,2*M_PI); histos.Add(h2L1RpcBX_q1);
  TH2D* h2L1RpcBX_q2   = new TH2D("h2L1RpcBX_q2","h2L1RpcBX_q2",132,-1.6,1.6,145,0,2*M_PI); histos.Add(h2L1RpcBX_q2);
  TH2D* h2L1RpcBX_q3   = new TH2D("h2L1RpcBX_q3","h2L1RpcBX_q3",132,-1.6,1.6,145,0,2*M_PI); histos.Add(h2L1RpcBX_q3);



  typedef std::map< std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> > EffLumiMap;
  EffLumiMap effLumiMap;


  // define chain
  TChain chain("tL1Rpc");
  std::vector<std::string> treeFileNames = theConfig.getParameter<std::vector<std::string> >("treeFileNames");
  for (std::vector<std::string>::const_iterator it = treeFileNames.begin(); it != treeFileNames.end(); ++it)  chain.Add((*it).c_str() );

  std::vector<bool> *hitBarrel = 0;
  std::vector<bool> *hitEndcap = 0;
  std::vector<unsigned int> *detBarrel = 0;
  std::vector<unsigned int> *detEndcap = 0;
  std::vector<unsigned int> *validPRCEndcap = 0;
  std::vector<unsigned int> *validDTEndcap = 0;
  std::vector<unsigned int> *validCSCEndcap = 0;


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

    event->lumi=0;
    std::cout <<" BITS: "<<muon->isGlobal()<<muon->isTracker()<<muon->isOuter()<<muon->isMatched()<<std::endl;
    if (effLumiMap.find(std::make_pair(event->run,event->lumi)) == effLumiMap.end()) 
          effLumiMap[make_pair(event->run,event->lumi)] = make_pair(0,0);
    if (!muon->isGlobal() || !muon->isTracker()||!muon->isOuter()|| muon->pt() < 5.5 || fabs(muon->eta()) > 1.5 ) continue;
    //if (!muon->isGlobal() || !muon->isTracker()||!muon->isOuter()|| muon->pt() < 2   || fabs(muon->eta()) > 1.5 ) continue;
    //if (!muon->isGlobal() || !muon->isTracker()||!muon->isOuter() ) continue;

    effLumiMap[make_pair(event->run,event->lumi)].second++;


    std::vector<L1Obj> l1Rpcs = l1RpcColl->getL1ObjsMatched(); 
    if (l1Rpcs.size() > 0) {
      effLumiMap[make_pair(event->run,event->lumi)].first++;
      int firstBX = 100;
      for (std::vector<L1Obj>::const_iterator it=l1Rpcs.begin(); it!= l1Rpcs.end(); ++it) {
	if ( (it->bx) < firstBX) {firstBX = it->bx;hL1RpcBX_only->Fill(firstBX);
	hL1RpcBX->Fill(firstBX);
	hL1RpcBX_all->Fill(it->bx);
	hL1RpcEta->Fill(it->eta);
	hL1RpcPhi->Fill(it->phi);
	h2L1RpcBX->Fill(it->eta,it->phi);
	if (it->q == 0){ hL1RpcEta_q0->Fill(it->eta); hL1RpcPhi_q0->Fill(it->phi);h2L1RpcBX_q0->Fill(it->eta,it->phi); }
	if (it->q == 1){ hL1RpcEta_q1->Fill(it->eta); hL1RpcPhi_q1->Fill(it->phi);h2L1RpcBX_q1->Fill(it->eta,it->phi); }
	if (it->q == 2){ hL1RpcEta_q2->Fill(it->eta); hL1RpcPhi_q2->Fill(it->phi);h2L1RpcBX_q2->Fill(it->eta,it->phi); }
	if (it->q == 3){ hL1RpcEta_q3->Fill(it->eta); hL1RpcPhi_q3->Fill(it->phi);h2L1RpcBX_q3->Fill(it->eta,it->phi); }
	
	if (it->bx){
	  h2L1RpcBX1->Fill(it->eta,it->phi);
	  if (it->q == 0){ h2L1RpcBX1_q0->Fill(it->eta,it->phi);}
	  if (it->q == 1){ h2L1RpcBX1_q1->Fill(it->eta,it->phi);}
	  if (it->q == 2){ h2L1RpcBX1_q2->Fill(it->eta,it->phi);}
	  if (it->q == 3){ h2L1RpcBX1_q3->Fill(it->eta,it->phi);}
	}

	}


//	if ( (it->bx) < firstBX) firstBX = it->bx;
//	hL1RpcBX->Fill(firstBX);
 
     }
    }
      

    std::cout<< "EV: "<<event->id<<" run: "<< event->run<< " RPC: "<< l1Rpcs.size()<<" rand: "<<(float) rand()/RAND_MAX;
    std::cout << std::endl;

    bool isMuon = (muon->pt() > theConfig.getParameter<double>("ptMin") );
    if(isMuon) {
      //if (fabs(muon.eta) > 0.8) continue;
      //if (fabs(muon.eta) < 1.24) continue;
      

      hMuonPt->Fill(muon->pt());
      hMuonEta->Fill(muon->eta());
      hMuonPhi->Fill(muon->phi());

//       cout<< "mk  " <<hitBarrel->size()<<endl;

//       cout<<"muon "<<muon->pt()
// 	  <<" "<<muon->eta()
// 	  <<" "<<muon->phi();
//       cout <<" gl.tr.sta.m "<<muon->isGlobal()<<muon->isTracker()<<muon->isOuter()<<muon->isMatched()<<endl;


      int nHitsB = 0; for (int i=0; i<6; i++) if( hitBarrel->at(i) ) nHitsB++;
      int nHitsBL= 0; for (int i=0; i<4; i++) if( hitBarrel->at(i) ) nHitsBL++;
      int nHitsE = 0; for (int i=0; i<3; i++) if( hitEndcap->at(i) ) nHitsE++;

      int nDetsB = 0;  for (int i=0; i<6; i++) if( detBarrel->at(i) ) nDetsB++;
      int nDetsE = 0;  for (int i=0; i<3; i++) if( detEndcap->at(i) ) nDetsE++;

      for (int i=0; i<6;++i) {
        if (detBarrel->at(i)) hEfficChambBar_D->Fill(i+1.);
        if (hitBarrel->at(i)) hEfficChambBar_N->Fill(i+1.);
//        if (!detBarrel->at(i) && hitBarrel->at(i)) hChambEffBar_V->Fill(i+1.);
      }
      for (int i=0; i<3;++i) {
        if (detEndcap->at(i)) hEfficChambEnd_D->Fill(i+1.);
        if (hitEndcap->at(i)) hEfficChambEnd_N->Fill(i+1.);
      }

      if (fabs(muon->eta()) < 0.8) {
        hHitsB->Fill(nHitsB);
        hDetsB_100->Fill(nDetsB);
        hEfficHits_D->Fill(muon->eta());      
        if (nHitsBL>=3 || (nHitsB>=4 && (hitBarrel->at(4)||hitBarrel->at(5)) ) ) {
          hEfficHits_N->Fill(muon->eta());
          if (l1Rpcs.size()>0) hEfficHits_H->Fill(muon->eta());
        }
      }
      if (fabs(muon->eta()) > 1.25 && fabs(muon->eta()) < 1.55) { 
        hHitsE->Fill(nHitsE);
        hDetsE_100->Fill(nDetsE);
        hEfficHits_D->Fill(muon->eta());      
        if (nHitsE==3) {
          hEfficHits_N->Fill(muon->eta());
          if (l1Rpcs.size()) hEfficHits_H->Fill(muon->eta());
        }
      }

      hEfficMu_D->Fill(muon->eta());
      if (l1Rpcs.size()) hEfficMu_N->Fill(muon->eta()); 

      hEfficMuPt_D->Fill(muon->pt());
      if (l1Rpcs.size())  hEfficMuPt_N->Fill(muon->pt());
      if (l1RpcColl->getL1ObjsMatched(7.).size()) hEfficMuPt7_N->Fill(muon->pt()); 
    }

    bool isTrack = (track->pt() > theConfig.getParameter<double>("ptMin") );
    if (isTrack) {
      //if (fabs(track.eta) > 0.8) continue;
      //if (fabs(track.eta) < 1.24) continue;
      hTrackPt->Fill(track->pt());
      hTrackEta->Fill(track->eta());
      hTrackPhi->Fill(track->phi());

      hEfficTk_D->Fill(track->eta());
      if (l1Rpcs.size()) hEfficTk_N->Fill(track->eta()); 

      hEfficTkPt_D->Fill(track->pt());
      if (l1Rpcs.size())     hEfficTkPt_N->Fill(track->pt());
      if (l1RpcColl->getL1ObjsMatched(7.).size()) hEfficTkPt7_N->Fill(track->pt()); 
    }

  
    if (l1Rpcs.size()) hDistL1Rpc->Fill(l1Rpcs[0].phi, l1Rpcs[0].eta);
    std::vector<L1Obj> l1Others = l1OtherColl->getL1Objs(); 
    if (l1Others.size()) hDistL1Other->Fill(l1Others[0].phi, l1Others[0].eta);
  }


  hGraph->Set(effLumiMap.size());
  unsigned int iPoint=0;
  for( EffLumiMap::const_iterator im = effLumiMap.begin(); im != effLumiMap.end(); ++im) {
    ++iPoint;
    float eff = 0.;
    if (im->second.second != 0) eff = float(im->second.first)/float(im->second.second);
    float effErr = sqrt( (1-eff)*im->second.first)/im->second.second;; 
    std::cout <<" RUN: "<<im->first.first
              <<" Effic: "<< eff <<" ("<<im->second.first<<"/"<<im->second.second<<")"<<std::endl; 
    hEffLumi->Fill(eff);
//    hGraph->SetPoint(iPoint, im->first.first, eff);
    hGraph->SetPoint(iPoint, iPoint, eff);
    hGraph->SetPointError(iPoint, 0., effErr);
  } 

//  chain.ResetBranchAddresses(); 

  std::string histoFile = theConfig.getParameter<std::string>("histoFileName");
  TFile f(histoFile.c_str(),"RECREATE");
  histos.Write();
  hGraph->Write("hGraph");
  f.Close();
 
  cout <<"KUKU"<<endl;
}
