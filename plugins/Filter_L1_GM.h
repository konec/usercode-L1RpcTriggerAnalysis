#ifndef Filter_L1_GM_H
#define Filter_L1_GM_H

#include "FWCore/Framework/interface/EDFilter.h"
#include "FilterL1.h"
#include "FilterGM.h"

namespace edm {class ParameterSet; class Event; class EventSetup; }

class Filter_L1_GM :  public edm::EDFilter {
public:

  explicit Filter_L1_GM(const Filter_L1_GM &) {;}
  explicit Filter_L1_GM(const edm::ParameterSet&p) : theL1(FilterL1(p)), theGM(FilterGM(p)) {}
  virtual ~Filter_L1_GM() {}
private:
  FilterL1 theL1;
  FilterGM theGM;
  virtual void endJob() {} 
  virtual bool filter(edm::Event& ev, const edm::EventSetup& es) { 
    return (theL1.filter(ev,es) || theGM.filter(ev,es));  
  }
};
#endif
