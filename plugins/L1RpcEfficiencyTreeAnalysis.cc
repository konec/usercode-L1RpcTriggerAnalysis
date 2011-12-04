#include "L1RpcEfficiencyTreeAnalysis.h"
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
#include "TF1.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/AnaEff.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaDet.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaEmu.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaRpcMisc.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaRpcVsOth.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaMuonDistribution.h"

void L1RpcEfficiencyTreeAnalysis::beginJob()
{
  TObjArray histos;
  histos.SetOwner();

  //
  // analyses
  //
  AnaMuonDistribution anaMuonDistribution(histos);
  AnaRpcVsOth         anaRpcVsOth(histos);
  AnaRpcMisc          anaRpcMisc(histos);
  AnaEff              anaEff(histos);
  AnaDet              anaDet(histos);
  AnaEmu              anaEmu(histos);

  // 
  // define input chain
  //
  TChain chain("tL1Rpc");
  std::vector<std::string> treeFileNames = theConfig.getParameter<std::vector<std::string> >("treeFileNames");
  for (std::vector<std::string>::const_iterator it = treeFileNames.begin(); it != treeFileNames.end(); ++it)  chain.Add((*it).c_str() );

  std::vector<uint32_t> *detsCrossedByMuon = 0;
  std::vector<uint32_t> *detsCrossedByMuonDeepInside = 0;
  std::vector<uint32_t> *detsHitsCompatibleWithMuon = 0;
  std::vector<uint32_t> *detsSIMU = 0;

  EventObj * event = 0;
  MuonObj * muon = 0;

  TBranch *bdetsCrossedByMuon =0;
  TBranch *bdetsCrossedByMuonDeepInside =0;
  TBranch *bdetsHitsCompatibleWithMuon = 0;
  TBranch *bdetsSIMU =0;

  L1ObjColl* l1RpcColl = 0;
  L1ObjColl* l1OtherColl = 0;
  L1ObjColl* l1RpcCollEmu = 0;

  chain.SetBranchAddress("event",&event);
  chain.SetBranchAddress("muon",&muon);

  chain.SetBranchAddress("detsCrossedByMuon",&detsCrossedByMuon,&bdetsCrossedByMuon);
  chain.SetBranchAddress("detsCrossedByMuonDeepInside",&detsCrossedByMuonDeepInside,&bdetsCrossedByMuonDeepInside);
  chain.SetBranchAddress("detsHitsCompatibleWithMuon",&detsHitsCompatibleWithMuon,&bdetsHitsCompatibleWithMuon);
  chain.SetBranchAddress("detsSIMU",&detsSIMU,&bdetsSIMU);

  chain.SetBranchAddress("l1RpcColl",&l1RpcColl);
  chain.SetBranchAddress("l1OtherColl",&l1OtherColl);
  chain.SetBranchAddress("l1RpcCollEmu",&l1RpcCollEmu);

  Int_t nentries = (Int_t) chain.GetEntries();
  std::cout <<" ENTRIES: " << nentries << std::endl;


  //
  // main loop
  //
  unsigned int lastRun = 0;
  unsigned int lastLumi= 0;
  for (int ev=0; ev<nentries; ev++) {
    chain.GetEntry(ev);
//    if (event->run < 175971) continue;
    if (lastRun != (*event).run) { lastRun = (*event).run; std::cout <<"RUN: " << (*event).run<<std::endl; }
//    if (lastLumi != (*event).lumi) { lastLumi = (*event).lumi; std::cout <<"lumi: " << (*event).lumi<<std::endl; }
    anaEmu.debug = false;

    anaMuonDistribution.run(muon);
    anaRpcVsOth.run(muon,l1RpcColl,l1OtherColl);
    anaEff.run(muon,l1RpcColl,l1OtherColl);
    anaRpcMisc.run(event,muon,l1RpcColl,l1OtherColl);
//    std::cout <<"----------"<<std::endl;
    anaEmu.run ( event, muon, l1RpcCollEmu, l1RpcColl);
//    anaDet.debug = anaEmu.debug; if (anaDet.debug) std::cout <<" Event: "<<(*event).id <<" Lumi: "<<(*event).lumi<< std::endl;
    anaDet.run( muon, *detsHitsCompatibleWithMuon,  *detsCrossedByMuon, *detsCrossedByMuonDeepInside);
//    anaRpcVsOth.run(muon,l1RpcCollEmu,l1OtherColl);
//    anaEff.run(muon,l1RpcCollEmu,l1OtherColl);
//    anaRpcMisc.run(muon,l1RpcCollEmu,l1OtherColl);
  }
  
  TGraph* hGraph_DetEff = anaDet.resume();
  TGraph* hGraph_RunEff = anaRpcMisc.resume();

  std::cout <<" ..processed, terminating...";
  std::string histoFile = theConfig.getParameter<std::string>("histoFileName");
  TFile f(histoFile.c_str(),"RECREATE");
  histos.Write();
  hGraph_DetEff->Write("hGraph_DetEff");
  hGraph_RunEff->Write("hGraph_RunEff");
  f.Close();
  std::cout <<"END"<<std::endl;

}
