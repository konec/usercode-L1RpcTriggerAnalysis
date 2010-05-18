#ifndef FilterL1_H
#define FilterL1_H

#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/Utilities/interface/InputTag.h"

namespace edm {class ParameterSet; class Event; class EventSetup; }

class FilterL1 :  public edm::EDFilter {
public:
  explicit FilterL1(const edm::ParameterSet&);
  virtual ~FilterL1();
  virtual bool filter(edm::Event&, const edm::EventSetup&);
private:
  virtual void endJob() {} 
  unsigned int theCounter;
  edm::InputTag l1MuReadout; 
};
#endif
