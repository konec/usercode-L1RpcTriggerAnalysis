#ifndef FilterMenu_H
#define FilterMenu_H

#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "L1Trigger/GlobalTriggerAnalyzer/interface/L1GtUtils.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"


namespace edm {class ParameterSet; class Event; class EventSetup; class Run; }

class FilterMenu :  public edm::EDFilter {
public:
  explicit FilterMenu(const edm::ParameterSet&);
  virtual ~FilterMenu();

private:
  virtual bool filter(edm::Event&, const edm::EventSetup&);
  virtual bool beginRun(edm::Run&, edm::EventSetup const&);

private:
  virtual bool filterL1(edm::Event&, const edm::EventSetup&);
  virtual bool filterHLT(edm::Event&, const edm::EventSetup&);

  unsigned int theCounterIN, theCounterL1, theCounterHLT;

  L1GtUtils theL1GtUtils;
  HLTConfigProvider theHltConfig;

};
#endif

