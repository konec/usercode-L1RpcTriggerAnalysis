#include "L1RpcTreeAnalysis.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TriggerMenuResultObj.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/ConverterRPCRawSynchroSynchroCountsObj.h"


void L1RpcTreeAnalysis::beginJob()
{
  theHistos.SetOwner();

  theAnaMuonDistribution.init(theHistos);
  theAnaRpcVsOth.init(theHistos);
  theAnaRpcMisc.init(theHistos);
  theAnaEff.init(theHistos);
  theAnaDet.init(theHistos);
  theAnaEmu.init(theHistos);
  theAnaSynch.init(theHistos);
  theAnaClu.init(theHistos);
  theAnaTimingL1.init(theHistos);

}

void L1RpcTreeAnalysis::beginRun(const edm::Run& ru, const edm::EventSetup& es)
{
  theAnaSynch.beginRun(ru,es);
}

void L1RpcTreeAnalysis::analyze(const edm::Event&, const edm::EventSetup&)
{
  //
  // prevent  multievent execution
  //
  static bool the_EMERGENCY_STOP = false;
  if (the_EMERGENCY_STOP) {
    std::cout <<"**  L1RpcTreeAnalysis::analyze EMERGENCY_STOP fired, skip" << std::endl;
    return;
  }
  the_EMERGENCY_STOP = true;

  // 
  // define input chain
  //
  TChain chain("tL1Rpc");
  std::vector<std::string> treeFileNames = theConfig.getParameter<std::vector<std::string> >("treeFileNames");
  for (std::vector<std::string>::const_iterator it = treeFileNames.begin(); it != treeFileNames.end(); ++it)  chain.Add((*it).c_str() );


  //
  // prepare datastructures and branches
  //
  std::vector<SynchroCountsObj> *counts= 0;
  std::vector<uint32_t> *detsCrossedByMuon = 0;
  std::vector<uint32_t> *detsCrossedByMuonDeepInside = 0;
  std::vector<DetCluDigiObj> *detsHitsCompatibleWithMuon = 0;
  std::vector<uint32_t> *detsSIMU = 0;

  EventObj * event = 0;
  MuonObj * muon = 0;

  TriggerMenuResultObj *bitsL1  = 0;
  TriggerMenuResultObj *bitsHLT = 0;

  TBranch *bcounts=0;
  TBranch *bdetsCrossedByMuon =0;
  TBranch *bdetsCrossedByMuonDeepInside =0;
  TBranch *bdetsHitsCompatibleWithMuon = 0;
  TBranch *bdetsSIMU =0;

  L1ObjColl* l1RpcColl = 0;
  L1ObjColl* l1OtherColl = 0;
  L1ObjColl* l1RpcCollEmu = 0;
  L1ObjColl* l1GmtColl = 0;

  chain.SetBranchAddress("event",&event);
  chain.SetBranchAddress("muon",&muon);

  chain.SetBranchAddress("bitsL1",&bitsL1);
  chain.SetBranchAddress("bitsHLT",&bitsHLT);
  
  chain.SetBranchAddress("counts",&counts,&bcounts);
  chain.SetBranchAddress("detsCrossedByMuon",&detsCrossedByMuon,&bdetsCrossedByMuon);
  chain.SetBranchAddress("detsCrossedByMuonDeepInside",&detsCrossedByMuonDeepInside,&bdetsCrossedByMuonDeepInside);
  chain.SetBranchAddress("detsHitsCompatibleWithMuon",&detsHitsCompatibleWithMuon,&bdetsHitsCompatibleWithMuon);
  chain.SetBranchAddress("detsSIMU",&detsSIMU,&bdetsSIMU);

  chain.SetBranchAddress("l1RpcColl",&l1RpcColl);
  chain.SetBranchAddress("l1OtherColl",&l1OtherColl);
  chain.SetBranchAddress("l1RpcCollEmu",&l1RpcCollEmu);
  chain.SetBranchAddress("l1GmtColl",&l1GmtColl);


  //
  // number of events
  //
  Int_t nentries = (Int_t) chain.GetEntries();
  std::cout <<" ENTRIES: " << nentries << std::endl;


  //
  // main loop
  //
  unsigned int lastRun = 0;
  for (int ev=0; ev<nentries; ev++) {
    chain.GetEntry(ev);

    if (lastRun != (*event).run) { 
      lastRun = (*event).run; 
      std::cout <<"RUN:"    << std::setw(7) << (*event).run
                <<" event:" << std::setw(8) << ev
                <<" done:"  << std::setw(6)<< std::setiosflags(std::ios::fixed) << std::setprecision(2) << ev*100./nentries<<"%"<<std::endl; 
    }

//    if (ev > 10) break;

//    if (event->run != 178854) continue;
//    if (lastLumi != (*event).lumi) { lastLumi = (*event).lumi; std::cout <<"lumi: " << (*event).lumi<<std::endl; }
//   theAnaDet.debug = false;
//    if ((*event).id==60422922)theAnaRpcMisc.debug = true;

   static std::vector<std::string>   namesL1;
   if (bitsL1->names.size() != 0)    namesL1=bitsL1->names;
   static std::vector<std::string>   namesHLT;
   if (bitsHLT->names.size() != 0)   namesHLT=bitsHLT->names;
   const std::vector<unsigned int> & algosL1 = bitsL1->firedAlgos;
   const std::vector<unsigned int> & algosHLT = bitsHLT->firedAlgos;

/*
   std::cout <<" Number of bits L1: " <<  bitsL1->names.size()<<"/"<<namesL1.size()<<"/"<<bitsL1->firedAlgos.size()
                           <<" HLT: "<< bitsHLT->names.size()<<"/"<<namesHLT.size()<<"/"<<bitsHLT->firedAlgos.size()<<std::endl;
   std::cout <<"----------------------- L1: "<<std::endl;
   for (std::vector<unsigned int>::const_iterator it=algosL1.begin(); it< algosL1.end(); ++it) std::cout <<"L1 idx: "<<*it<<" "<<namesL1[*it]<< std::endl;
   std::cout <<"----------------------- HLT: "<<std::endl;
   for (std::vector<unsigned int>::const_iterator it=algosHLT.begin(); it< algosHLT.end(); ++it) std::cout<<" HLT idx: "<<*it<<" "<<namesHLT[*it]<< std::endl;
*/

   theAnaMuonDistribution.run(muon);
   theAnaRpcVsOth.run(muon,l1RpcColl,l1OtherColl);
   theAnaEff.run(muon,l1RpcColl,l1OtherColl);
   theAnaRpcMisc.run(event,muon,l1RpcColl,l1OtherColl);
   theAnaDet.run( muon, *detsHitsCompatibleWithMuon,  *detsCrossedByMuon, *detsCrossedByMuonDeepInside);
   theAnaEmu.run ( event, muon, l1RpcCollEmu, l1RpcColl);
   theAnaSynch.run( event, muon, ConverterRPCRawSynchroSynchroCountsObj::toRawSynchro( *counts));
   theAnaClu.run( event, muon, l1RpcColl, *detsHitsCompatibleWithMuon);
   theAnaTimingL1.run(event,muon,l1RpcColl,l1OtherColl);

/*
   std::cout <<"----------------"<<std::endl;
   std::cout <<"GmtColl:"<<std::endl; l1GmtColl->print(); 
   std::cout <<"RpcColl:"<<std::endl; l1RpcColl->print(); 
   std::cout <<"OtherColl:"<<std::endl; l1OtherColl->print(); 
*/
//   theAnaEmu.debug =theAnaDet.debug;
//    std::cout <<"----------"<<std::endl;
//   theAnaDet.debug =theAnaEmu.debug; 
//    if (anaDet.debug) std::cout <<" Event: "<<(*event).id <<" Lumi: "<<(*event).lumi<< std::endl;
  
  }
}

void L1RpcTreeAnalysis::endJob()
{
  std::cout <<"ENDJOB, summaries:"<<std::endl;
  TGraph* hGraph_DetEff = theAnaDet.resume();
  theAnaRpcMisc.resume(theHistos);
  TGraph* hGraph_RunClu = theAnaClu.resume();
  theAnaTimingL1.resume(theHistos);

  theAnaSynch.endJob();

  std::string histoFile = theConfig.getParameter<std::string>("histoFileName");
  TFile f(histoFile.c_str(),"RECREATE");
  theHistos.Write();
  hGraph_DetEff->Write("hGraph_DetEff");
  hGraph_RunClu->Write("hGraph_RunClu");
  f.Close();
  std::cout <<"END"<<std::endl;
}
