#ifndef SynchroSelector_h
#define SynchroSelector_h

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

class TH1F;
class RPCDetId;
class TrajectoryStateOnSurface;

namespace edm { class Event; class EventSetup; }
 

class SynchroSelector {
public:
  public: SynchroSelector( const edm::ParameterSet & cfg = edm::ParameterSet() );
  virtual ~SynchroSelector(){}

  virtual bool takeIt(const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es) {return false; }

protected:
  bool checkRpcDetMatching( const TrajectoryStateOnSurface & tsos,  const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es);
  bool checkUniqueRecHitMatching( const TrajectoryStateOnSurface & tsos,  const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es);
  
protected:
  edm::ParameterSet theConfig;
  TH1F *hPullX, *hDistX;

};

#endif
