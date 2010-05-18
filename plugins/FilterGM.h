#ifndef FilterGM_H
#define FilterGM_H

#include "FWCore/Framework/interface/EDFilter.h"

namespace edm {class ParameterSet; class Event; class EventSetup; }
class TH1D;

class FilterGM :  public edm::EDFilter {
public:
  explicit FilterGM(const edm::ParameterSet&);
  virtual ~FilterGM();
  virtual bool filter(edm::Event&, const edm::EventSetup&);
private:
  virtual void endJob() {} 
};
#endif
