#include "FilterMenu.h"

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

bool FilterMenu::beginRun(edm::Run& run, edm::EventSetup const& es)
{
  theL1GtUtils.getL1GtRunCache(run,es, false, true);
  bool changed(true);
  if (theHltConfig.init(run,es,"HLT",changed)) {
    if (changed) {
      theHltConfig.dump("Streams");
      theHltConfig.dump("Datasets");
      theHltConfig.dump("Triggers");
      theHltConfig.dump("PrescaleTable");
//      theHltConfig.dump("ProcessPSet");
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
/*
  std::cout <<"-------------------------------------------------------------------"<<std::endl;
  theCounterIN++;
  if ( !filterL1(ev,es) ) return false;
  theCounterL1++;
  if ( !filterHLT(ev,es) ) return false;
  theCounterHLT++;
*/
  return true;
}

bool FilterMenu::filterL1(edm::Event&ev, const edm::EventSetup&es)
{
  bool result = false;

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

/*
  edm::Handle<L1GlobalTriggerReadoutRecord> gtReadoutRecord;
  edm::InputTag l1GtDaqReadoutRecordInputTag("gtDigis");
  ev.getByLabel(l1GtDaqReadoutRecordInputTag, gtReadoutRecord);
  if (!gtReadoutRecord.isValid()) std::cout <<" PROBLEM, record not OK" << std::endl;
  DecisionWord gtDecisionWord = gtReadoutRecord->decisionWord();
  std::bitset<128> bs(0ul);
  std::bitset<64> bs1, bs2;
  for (unsigned int i=0; i<128;++i) std::cout <<gtDecisionWord[i]; std::cout <<std::endl;
  for (unsigned int i=0; i<64; ++i) { bs1[i] = gtDecisionWord[i]; bs2[i]=gtDecisionWord[i+64]; }
  for (unsigned int i=0; i<128;++i) bs[i] = gtDecisionWord[i];
  std::cout <<bs << std::endl;
  std::stringstream str;
  gtReadoutRecord->printGtDecision(str);
  std::cout <<str.str()<<std::endl;
  std::cout << std::hex << bs2.to_ulong() <<  "   "<<std::hex <<bs1.to_ulong() << std::endl;
//    std::cout <<" GT decision: "<<gtDecision<<" print DecisionWord: "<<gtDecisionWord<<std::endl;
//  std::cout <<str <<std::endl;
//  std::cout <<bs <<std::endl;
//  for (unsigned int i=0; i<128;++i) std::cout <<bs[i]; std::cout<<std::endl;
*/

  //std::cout <<"Fired L1 Algorithms: " << std::endl;
  const L1GtTriggerMenuLite* menu = theL1GtUtils.ptrL1GtTriggerMenuLite(errorCode);
  bool hasMuSeed = false;
  for (unsigned int  i= 0; i<128; ++i) {
    const std::string * pname = menu->gtAlgorithmName(i,errorCode);
    if (errorCode) continue; // {std::cout<<"idx: "<<i<<" ERROR code(1) not 0, skip"<<std::endl; continue; }
    bool decision = theL1GtUtils.decisionAfterMask(ev, *pname, errorCode);
    if (errorCode) continue; //{std::cout<<"idx: "<<i<<" ERROR code(2) not 0, skip"<<std::endl; continue; }
    if (decision) {
     bool isMu = ( (*pname).find("Mu") != std::string::npos);
      std::cout<<"algo: "<<i<<" "<<*pname; //<<" decision: "<<gtDecisionWord[i];
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
  //std::cout << "TRIGGER MENU NAME: "<<  l1GtUtils.l1TriggerMenu() << std::endl;
  //result = !hasMuSeed;
  //result=true;



  
  return result;
}


bool FilterMenu::filterHLT(edm::Event&ev, const edm::EventSetup&es)
{
  bool result = false;

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
  for (unsigned int triggerIndex =0; triggerIndex < theHltConfig.size()-1; ++triggerIndex) {   //skip "Final" decision indes
    std::string triggerName = theHltConfig.triggerName(triggerIndex);
    unsigned int triggerIndex = theHltConfig.triggerIndex(triggerName);
    assert(triggerIndex==ev.triggerNames(*triggerResultsHandle).triggerIndex(triggerName));
    bool isAccept = triggerResultsHandle->accept(triggerIndex);
//    if (true) {
    if (isAccept) {
      bool isMu = ( (triggerName.find("Mu") != std::string::npos) && (triggerName.find("Multi") == std::string::npos) );
//      std::cout <<triggerName;  if (!isMu) std::cout <<" <---- "; std::cout << std::endl;
      if (isMu)  hasMuSeed = true;
      if (!isMu)  result = true;
       std::cout <<triggerName<< " Trigger path status:" 
              //<< " WasRun=" << triggerResultsHandle->wasrun(triggerIndex)
              << " Accept=" << triggerResultsHandle->accept(triggerIndex);
//              << " Error =" << triggerResultsHandle->error(triggerIndex)

      if (isAccept) std::cout <<" <--";
      std::cout << std::endl;
    }
  }
  return result;
}
