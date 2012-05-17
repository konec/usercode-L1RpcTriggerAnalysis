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

#include "CondFormats/L1TObjects/interface/L1GtTriggerMenuFwd.h"
#include "CondFormats/L1TObjects/interface/L1GtTriggerMenu.h"
#include "CondFormats/DataRecord/interface/L1GtTriggerMenuRcd.h"

#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Common/interface/TriggerResultsByName.h"


FilterMenu::FilterMenu(const edm::ParameterSet& cfg)
  : theCounterIN(0), theCounterL1(0), theCounterHLT(0)
{ }

bool FilterMenu::beginRun(edm::Run& run, edm::EventSetup const& es)
{
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
  theCounterIN++;
  if ( !filterL1(ev,es) ) return false;
  theCounterL1++;
  if ( !filterHLT(ev,es) ) return false;
  theCounterHLT++;
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
  if (ev.run() != lastRun) {
    lastRun = ev.run();
    //l1GtUtils.retrieveL1EventSetup(es);
    edm::InputTag tag("l1GtTriggerMenuLite");
    theL1GtUtils.retrieveL1GtTriggerMenuLite(ev.getRun(), tag);
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
  for (unsigned int  i= 0; i<128; ++i) {
    const std::string * pname = menu->gtAlgorithmName(i,errorCode);
    if (errorCode) continue;
    bool decision = theL1GtUtils.decisionAfterMask(ev, *pname, errorCode);
    if (errorCode) continue;
    if (decision) {
      bool isMu = ( (*pname).find("Mu") != std::string::npos);
//      std::cout << *pname; if (!isMu) std::cout <<" <---- "; std::cout << std::endl;
      if (isMu) hasMuSeed = true;
      if (!isMu) result = true;
      //int triggerMask,prescaleFactor;
      //bool decisionBeforeMask, decisionAfterMask;
      //theL1GtUtils.l1Results(ev, *pname, decisionBeforeMask, decisionAfterMask, prescaleFactor, triggerMask);
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
  for (unsigned int triggerIndex =0; triggerIndex < theHltConfig.size()-1; ++triggerIndex) {   //skip "Final" decision indes
    std::string triggerName = theHltConfig.triggerName(triggerIndex);
    unsigned int triggerIndex = theHltConfig.triggerIndex(triggerName);
    assert(triggerIndex==ev.triggerNames(*triggerResultsHandle).triggerIndex(triggerName));
    bool isAccept = triggerResultsHandle->accept(triggerIndex);
    if (true) {
//    if (isAccept) {
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
