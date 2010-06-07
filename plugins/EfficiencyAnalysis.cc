#include "EfficiencyAnalysis.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"


#include <vector>
#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TProfile.h"
#include "TH1D.h"

using namespace std;

void EfficiencyAnalysis::beginJob()
{
  TFile *file = TFile::Open(theConfig.getParameter<std::string>("treeFileName").c_str(),"READ");
  if (!file) { return; }

  TTree *theTree; 
  file->GetObject("tL1Rpc",theTree);

  std::vector<bool> *hitBarrel = 0;
  std::vector<bool> *hitEndcap = 0;

  L1Obj l1Rpc;
  L1Obj l1Other;
  EventObj event;
  TrackObj track;
  TrackObj muon;

  TBranch *bhitBarrel=0;
  TBranch *bhitEndcap=0;

  theTree->SetBranchAddress("evBx",&event.bx);
  theTree->SetBranchAddress("evId",&event.id);
  theTree->SetBranchAddress("evRun",&event.run);
  theTree->SetBranchAddress("evLumi",&event.lumi);

  theTree->SetBranchAddress("hitBarrel",&hitBarrel, &bhitBarrel);
  theTree->SetBranchAddress("hitEndcap",&hitEndcap, &bhitEndcap);

  theTree->SetBranchAddress("muPt",&muon.pt);
  theTree->SetBranchAddress("muEta",&muon.eta);
  theTree->SetBranchAddress("muPhi",&muon.phi);

  theTree->SetBranchAddress("tkPt",&track.pt);
  theTree->SetBranchAddress("tkEta",&track.eta);
  theTree->SetBranchAddress("tkPhi",&track.phi);

  theTree->SetBranchAddress("rpcPt", &l1Rpc.pt);
  theTree->SetBranchAddress("rpcEta",&l1Rpc.eta);
  theTree->SetBranchAddress("rpcPhi",&l1Rpc.phi);
  theTree->SetBranchAddress("rpcBx",&l1Rpc.bx);
  theTree->SetBranchAddress("rpcQ",&l1Rpc.q);

  theTree->SetBranchAddress("otherPt", &l1Other.pt);
  theTree->SetBranchAddress("otherEta",&l1Other.eta);
  theTree->SetBranchAddress("otherPhi",&l1Other.phi);
  theTree->SetBranchAddress("otherBx",&l1Other.bx);
  theTree->SetBranchAddress("otherQ",&l1Other.q);



  Int_t nentries = (Int_t) theTree->GetEntries();
  std::cout <<" ENTRIES: " << nentries << std::endl;

  for (int i=0; i<10; i++) {

    theTree->GetEntry(i);

    std::cout <<"event : "<<event.id<<" run: "<<event.run<<" lumi: "<<event.lumi<<"muonpt:"<<muon.pt<<" trackpt:"<<track.pt
    <<" muonEta "<<muon.eta<<" trackEta "<<track.eta
    <<" rpcPt: "<<l1Rpc.pt<<" otherPt:"<<l1Other.pt
    <<" hits: ";
    int nhits = hitBarrel->size();
    for (int j=0;j<nhits;++j) std::cout <<hitBarrel->at(j);
    cout <<" endcap:";
    nhits = hitEndcap->size();
    for (int j=0;j<nhits;++j) std::cout <<hitEndcap->at(j);
    std::cout << std::endl;
  }

  theTree->ResetBranchAddresses(); 

  cout <<"KUKU"<<endl;
}
