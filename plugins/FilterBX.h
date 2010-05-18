#ifndef FilterBX_H
#define FilterBX_H

#include "FWCore/Framework/interface/EDFilter.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"

namespace edm {class EventSetup; }

class FilterBX :  public edm::EDFilter {
public:
  typedef std::vector<uint32_t> BEAMBX;
  explicit FilterBX(const edm::ParameterSet&p) : beamBX(p.getParameter<BEAMBX>("beamBX")) {}
  virtual ~FilterBX() {}
private:
  BEAMBX beamBX;
  virtual bool filter(edm::Event& ev, const edm::EventSetup& es) {
    return (beamBX.end()==find(beamBX.begin(), beamBX.end(), ev.bunchCrossing()))? false: true;
  }
};
#endif
