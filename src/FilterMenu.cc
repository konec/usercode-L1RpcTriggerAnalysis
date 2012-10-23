#include "UserCode/L1RpcTriggerAnalysis/interface/FilterMenu.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <bitset>

#include "CondFormats/L1TObjects/interface/L1GtTriggerMenuFwd.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"

#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"

#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerRecord.h"


FilterMenu::FilterMenu(const edm::ParameterSet& cfg)
  : theCounterIN(0), theCounterL1(0), theCounterHLT(0)
{ }

bool FilterMenu::checkRun(const edm::Run& run, const edm::EventSetup & es)
{
  std::cout <<"FilterMenu::beginRun CALLED"<<std::endl;

  //
  // L1
  //
  theL1GtUtils.getL1GtRunCache(run,es, false, true);
  int errorCode = -1;
  const L1GtTriggerMenuLite* menu = theL1GtUtils.ptrL1GtTriggerMenuLite(errorCode);
  if (errorCode) return false;
  theNamesAlgoL1.clear();
  for (unsigned int  idx = 0; idx <128; ++idx) { 
    const std::string *pname = menu->gtAlgorithmName(idx,errorCode);
    if (errorCode==0) theNamesAlgoL1.push_back(*pname); else theNamesAlgoL1.push_back("");
  }


  //
  // HLT
  //
  bool changed(true);
  if (theHltConfig.init(run,es,"HLT",changed)) {
    if (changed) {
//      theHltConfig.dump("Streams");
//      theHltConfig.dump("Datasets");
//      theHltConfig.dump("Triggers");
//      theHltConfig.dump("PrescaleTable");
//      theHltConfig.dump("ProcessPSet");
      theNamesAlgoHLT.clear();
      for (unsigned int idx =0;  idx< theHltConfig.size()-1; idx++) theNamesAlgoHLT.push_back( theHltConfig.triggerName(idx) );
    }
  } 
  return true; 
}

FilterMenu::~FilterMenu()
{
  std::cout <<"FilterMenu, IN: "<<theCounterIN<<"  AfterL1: "<< theCounterL1 << " AfterHLT: "<<theCounterHLT<< std::endl;
}

bool FilterMenu::filter(edm::Event&ev, const edm::EventSetup&es)
{
  std::cout <<"-------------------------------------------------------------------"<<std::endl;
  theCounterIN++;
  if ( !filterL1(ev,es) ) return false;
  theCounterL1++;
  if ( !filterHLT(ev,es) ) return false;
  theCounterHLT++;
  return true;
}




std::vector<unsigned int>  FilterMenu::firedAlgosL1(const edm::Event&ev, const edm::EventSetup&es) 
{
  std::vector<unsigned int> result;

   //
  // get mask set
  //
  theL1GtUtils.getL1GtRunCache(ev,es, false, true);
  int errorCode = -1;
  const std::vector<unsigned int>& triggerMaskSet =  theL1GtUtils.triggerMaskSet( L1GtUtils::AlgorithmTrigger, errorCode);
  if (errorCode) return result;

  //
  // get decsion
  //
  edm::Handle<L1GlobalTriggerReadoutRecord> gtReadoutRecord;
  edm::InputTag l1GtDaqReadoutRecordInputTag("gtDigis");
  ev.getByLabel(l1GtDaqReadoutRecordInputTag, gtReadoutRecord);
  if (!gtReadoutRecord.isValid()) { std::cout <<" PROBLEM, record not OK" << std::endl; return result;}
  DecisionWord gtDecisionWord = gtReadoutRecord->decisionWord();

  //
  // save only not masked bits
  //
  for (unsigned int i=0; i<128;++i) if (gtDecisionWord[i] && ! triggerMaskSet[i]) result.push_back(i);

//  for (unsigned int i=0; i<128;++i) if (gtDecisionWord[i] && ! triggerMaskSet[i]) std::cout << i<<" ";
//  unsigned int ntrig=0;  for (unsigned int i=0; i<128;++i) if (gtDecisionWord[i]  && ! triggerMaskSet[i]) ntrig++;
//  std::cout <<" --->  TOTAL NUMBER OF L1 TRIGGERS: " <<  ntrig << std::endl;

//  std::bitset<128> bs(0ul);
//  for (unsigned int i=0; i<128;++i) std::cout <<gtDecisionWord[i]; std::cout <<std::endl;
//  for (unsigned int i=0; i<64; ++i) { bs1[i] = gtDecisionWord[i]; bs2[i]=gtDecisionWord[i+64]; }
//  for (unsigned int i=0; i<128;++i) bs[i] = gtDecisionWord[i];
//  std::stringstream str;
//  gtReadoutRecord->printGtDecision(str);
//  std::cout <<str.str()<<std::endl;

  return result;
}

std::vector<unsigned int> FilterMenu::firedAlgosHLT(const edm::Event&ev, const edm::EventSetup&es) 
{
  std::vector<unsigned int> result;
  edm::Handle<edm::TriggerResults>   triggerResultsHandle;
  ev.getByLabel(edm::InputTag("TriggerResults","","HLT"), triggerResultsHandle);
  if (!triggerResultsHandle.isValid()) { std::cout << "firedAlgosHLT, PROBLEM, record not OK"<< std::endl; return result; }
  assert(triggerResultsHandle->size()==theHltConfig.size());

  //unsigned int ntrig=0;
  for (unsigned int triggerIndex =0; triggerIndex < theHltConfig.size()-1; ++triggerIndex) {   //skip "Final" decision indes
    std::string triggerName = theHltConfig.triggerName(triggerIndex);
    unsigned int triggerIndex = theHltConfig.triggerIndex(triggerName);
    assert(triggerIndex==ev.triggerNames(*triggerResultsHandle).triggerIndex(triggerName));
    bool isAccept = triggerResultsHandle->accept(triggerIndex);
    if (isAccept) result.push_back(triggerIndex);
//    if (isAccept) std::cout <<  triggerIndex <<" ";
//    if (isAccept) ntrig++;
  }
//std::cout <<"  --->  TOTAL NUMBER OF HLT TRIGGERS: " <<  ntrig << std::endl;

  
  return result;
}




bool FilterMenu::filterL1(edm::Event&ev, const edm::EventSetup&es)
{
//  bool result = false;
  bool result = true;

  // reinitialiast Gt Menu,
  // not clear why it needs ev.
  int errorCode = 0;
  int  l1ConfCode = 0;
  static edm::RunNumber_t lastRun = 0; 
  theL1GtUtils.getL1GtRunCache(ev,es, false, true);

  if (ev.run() != lastRun) {
    lastRun = ev.run();
    edm::InputTag tag("l1GtTriggerMenuLite");
//    theL1GtUtils.retrieveL1GtTriggerMenuLite(ev.getRun(), tag);
    bool status = theL1GtUtils.availableL1Configuration(errorCode,l1ConfCode);
    std::cout <<" GOT status: " <<status<<  " errorCode: " << errorCode<<" l1ConfCode: " << l1ConfCode<<std::endl;
    const L1GtTriggerMenuLite* menu = theL1GtUtils.ptrL1GtTriggerMenuLite(errorCode);
    if (errorCode) std::cout <<" ******Error CODE "<<errorCode<<std::endl;
    std::cout <<"NAME: " << menu->gtTriggerMenuName() << std::endl;
    //  for (L1GtTriggerMenuLite::CItL1Trig algo = menu->gtAlgorithmMap().begin(); algo!=menu->gtAlgorithmMap().end(); ++algo) 
    //      std::cout << "Id: " << algo->first << " Name: " << algo->second << std::endl;
    std::cout << *menu << std::endl;
  }


  //std::cout <<"Fired L1 Algorithms: " << std::endl;
  const L1GtTriggerMenuLite* menu = theL1GtUtils.ptrL1GtTriggerMenuLite(errorCode);
  bool hasMuSeed = false;
  unsigned int ntrig = 0;
  for (unsigned int  i= 0; i<128; ++i) {
    const std::string * pname = menu->gtAlgorithmName(i,errorCode);
    if (errorCode) continue; // {std::cout<<"idx: "<<i<<" ERROR code(1) not 0, skip"<<std::endl; continue; }
    bool decision = theL1GtUtils.decisionAfterMask(ev, *pname, errorCode);
    if (errorCode) continue; //{std::cout<<"idx: "<<i<<" ERROR code(2) not 0, skip"<<std::endl; continue; }
    if (decision) {
     ntrig++;
     bool isMu = ( (*pname).find("Mu") != std::string::npos);
      std::cout<<"L1 idx: "<<i<<" "<<*pname; //<<" decision: "<<gtDecisionWord[i];
      if (!isMu) ;
      if (isMu) hasMuSeed = true;
      if (!isMu) result = true;
      int triggerMask,prescaleFactor;
      bool decisionBeforeMask, decisionAfterMask;
      //theL1GtUtils.l1Results(ev, *pname, decisionBeforeMask, decisionAfterMask, prescaleFactor, triggerMask);
      //theL1GtUtils.prescaleFactorSetIndex(ev, L1GtUtils::AlgorithmTrigger, errorCode);;
      std::cout <<std::endl;
    }
  }
  std::cout <<" total number of L1 triggers: "<<ntrig<<std::endl;
  //std::cout << "TRIGGER MENU NAME: "<<  l1GtUtils.l1TriggerMenu() << std::endl;
  //result = !hasMuSeed;
  //result=true;

  
  return result;
}

bool FilterMenu::filterHLT(edm::Event&ev, const edm::EventSetup&es)
{
  //bool result = false;
  bool result = true;

  // get event products
  edm::Handle<edm::TriggerResults>   triggerResultsHandle;
  ev.getByLabel(edm::InputTag("TriggerResults","","HLT"), triggerResultsHandle);
  if (!triggerResultsHandle.isValid()) {
    std::cout << "HLTEventAnalyzerAOD::analyze: Error in getting TriggerResults product from Event!" << std::endl;
    return 0;
  }
  assert(triggerResultsHandle->size()==theHltConfig.size());

  bool hasMuSeed = false;
  std::cout <<"Number of HLT algorithms: "<<theHltConfig.size() << std::endl;
  unsigned int ntrig = 0;
  for (unsigned int triggerIndex =0; triggerIndex < theHltConfig.size()-1; ++triggerIndex) {   //skip "Final" decision indes
    std::string triggerName = theHltConfig.triggerName(triggerIndex);
    unsigned int triggerIndex = theHltConfig.triggerIndex(triggerName);
    assert(triggerIndex==ev.triggerNames(*triggerResultsHandle).triggerIndex(triggerName));
    bool isAccept = triggerResultsHandle->accept(triggerIndex);
//    if (true) {
    if (isAccept) {
      ntrig++;
      bool isMu = ( (triggerName.find("Mu") != std::string::npos) && (triggerName.find("Multi") == std::string::npos) );
//      std::cout <<triggerName;  if (!isMu) std::cout <<" <---- "; std::cout << std::endl;
      if (isMu)  hasMuSeed = true;
      if (!isMu)  result = true;
       std::cout <<" HLT idx: "<<triggerIndex <<" "<<triggerName<< " Trigger path status:" 
              //<< " WasRun=" << triggerResultsHandle->wasrun(triggerIndex)
              << " Accept=" << triggerResultsHandle->accept(triggerIndex);
//              << " Error =" << triggerResultsHandle->error(triggerIndex)
//      if (isAccept) std::cout <<" <--";
      std::cout << std::endl;
    }
  }
  std::cout <<" total number of HLT triggers: "<<ntrig<<std::endl;

  return result;
}

