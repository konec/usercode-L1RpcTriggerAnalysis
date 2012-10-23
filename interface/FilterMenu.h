#ifndef FilterMenu_H
#define FilterMenu_H

#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

#include <vector>
#include <string>


namespace edm {class ParameterSet; class Event; class EventSetup; class Run; }

class FilterMenu :  public edm::EDFilter {
public:
  explicit FilterMenu(const edm::ParameterSet&);
  virtual ~FilterMenu();
  const std::vector<std::string> & namesAlgoHLT() const { return theNamesAlgoHLT; }
  const std::vector<std::string> & namesAlgoL1() const { return theNamesAlgoL1; }
  std::vector<unsigned int> firedAlgosHLT(const edm::Event&, const edm::EventSetup&) ;
  std::vector<unsigned int> firedAlgosL1(const edm::Event&, const edm::EventSetup&) ;

  virtual bool checkRun(const edm::Run&, const edm::EventSetup &es);

private:
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual bool beginRun(edm::Run& r, const edm::EventSetup & es) { return checkRun(r,es); }
  virtual bool filterL1(edm::Event&, const edm::EventSetup&);
  virtual bool filterHLT(edm::Event&, const edm::EventSetup&);

  unsigned int theCounterIN, theCounterL1, theCounterHLT;

  L1GtUtils theL1GtUtils;
  HLTConfigProvider theHltConfig;

  std::vector<std::string> theNamesAlgoHLT, theNamesAlgoL1;

};
#endif

