#ifndef TrackToL1ObjMatcher_H
#define TrackToL1ObjMatcher_H

#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
namespace edm { class Event; class EventSetup; }
class TrajectoryStateOnSurface;

class TrackToL1ObjMatcher {
public:
  TrackToL1ObjMatcher(const edm::ParameterSet & cfg) : theConfig(cfg) {}
  bool operator()(const L1Obj& l1, const TrajectoryStateOnSurface & tsos,  const edm::Event&ev, const edm::EventSetup& es) const;
private:
  edm::ParameterSet theConfig;

};
#endif 
