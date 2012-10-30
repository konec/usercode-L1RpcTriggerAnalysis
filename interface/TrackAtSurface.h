#ifndef L1RpcTriggerAnalysis_TrackAtSurface_H
#define L1RpcTriggerAnalysis_TrackAtSurface_H

#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

namespace edm {class Event; class EventSetup; }
namespace reco { class Muon; }
class RPCDetId;

class TrackAtSurface {
public:
  //use that if possible
  TrackAtSurface( const reco::Muon* mu, const edm::Event &ev, const edm::EventSetup &es);

  //note: if possible use the constructor with muon (here no access to trajectory) 
  TrackAtSurface( const TrajectoryStateOnSurface &state, const edm::Event &ev, const edm::EventSetup &es);

  TrajectoryStateOnSurface atDetFromTrack( const RPCDetId& rpcDet) const;
  TrajectoryStateOnSurface atDetFromClose( const RPCDetId& rpcDet,  const GlobalPoint& point) const;
  TrajectoryStateOnSurface atPoint( const GlobalPoint& point) const;

  //note: propagation "Along from tsos"
  TrajectoryStateOnSurface atStation2( float eta) const;      
private:
   Trajectory               theTrajectory;
   TrajectoryStateOnSurface theState; 
   const edm::Event & theEv;
   const edm::EventSetup & theEs;
};
#endif
