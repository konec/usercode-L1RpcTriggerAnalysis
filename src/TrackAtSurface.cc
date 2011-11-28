#include "UserCode/L1RpcTriggerAnalysis/interface/TrackAtSurface.h"

#include <vector>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"


#include "DataFormats/GeometrySurface/interface/BoundCylinder.h"
#include "DataFormats/GeometrySurface/interface/SimpleCylinderBounds.h"
#include "DataFormats/GeometrySurface/interface/BoundDisk.h"
#include "DataFormats/GeometrySurface/interface/SimpleDiskBounds.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"




TrackAtSurface::TrackAtSurface(const reco::Muon* mu,const edm::Event &ev, const edm::EventSetup &es)
  : theEv(ev), theEs(es)
{
  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);


//refit global muons, take tracjctory colsest to Mu. 
  typedef std::vector<Trajectory> Trajectories;
  edm::Handle<Trajectories> trajectories;
  edm::InputTag refitTag("globalMuons","Refitted");
  ev.getByLabel(refitTag,trajectories);
  double minDR =  0.3;
  for (Trajectories::const_iterator it=trajectories->begin(); it != trajectories->end(); ++it) {
    double diff = deltaR(mu->track()->eta(), mu->track()->phi(), it->geometricalInnermostState().globalMomentum().eta(), it->geometricalInnermostState().globalMomentum().phi());
    if (diff < minDR) { minDR = diff; theTrajectory = *it; }
  }
  theState = TrajectoryStateTransform().outerStateOnSurface(*(mu->track()), *globalGeometry, &*magField);

}

TrajectoryStateOnSurface TrackAtSurface::atPoint( const GlobalPoint& point) const
{
  TrajectoryStateOnSurface muTSOS  = theTrajectory.empty() ? theState : theTrajectory.closestMeasurement(point).updatedState();

  bool barrel = fabs(point.z()) < 680. ? true : false;
  ReferenceCountingPointer<Surface> surface = barrel ?
      ReferenceCountingPointer<Surface>( new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( point.perp(),  point.perp(), -680., 680. ) ))
    : ReferenceCountingPointer<Surface>( new  BoundDisk( GlobalPoint(0.,0.,point.z()), TkRotation<float>(), SimpleDiskBounds( 260., 720., -0.0001, 0.0001 ) ) );

  edm::ESHandle<Propagator> propagator;
  theEs.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAny", propagator);
  TrajectoryStateOnSurface result =  propagator->propagate(muTSOS, *surface);

  return result;
}

TrajectoryStateOnSurface TrackAtSurface::atDetFromClose( const RPCDetId& rpc, const GlobalPoint& point) const
{

  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  theEs.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  edm::ESHandle<Propagator> propagator;

  const GeomDet * det = globalGeometry->idToDet(rpc);
  TrajectoryStateOnSurface muTSOS;
  if (!theTrajectory.empty()) muTSOS = theTrajectory.closestMeasurement(point).updatedState();
  theEs.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAny",propagator);
  Plane::PlanePointer surface = Plane::build(det->position(), det->rotation());
  TrajectoryStateOnSurface result;
  if (muTSOS.isValid()) result =  propagator->propagate(muTSOS, *surface);

  return result;
}

TrajectoryStateOnSurface TrackAtSurface::atDetFromTrack( const RPCDetId& rpc) const
{

  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  theEs.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  edm::ESHandle<Propagator> propagator;

  const GeomDet * det = globalGeometry->idToDet(rpc);
  TrajectoryStateOnSurface muTSOS = theState;
  theEs.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAlong",propagator);
  Plane::PlanePointer surface = Plane::build(det->position(), det->rotation());
  TrajectoryStateOnSurface result;
  if (muTSOS.isValid()) result =  propagator->propagate(muTSOS, *surface);

  return result;
}

TrajectoryStateOnSurface TrackAtSurface::atStation2( float eta) const
{
  ReferenceCountingPointer<Surface> rpc;
  if (eta < -1.04)       rpc = ReferenceCountingPointer<Surface>(new  BoundDisk( GlobalPoint(0.,0.,-790.), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
  else if (eta < -0.72)  rpc = ReferenceCountingPointer<Surface>(new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 520, 520, -700, 700 ) ) );
  else if (eta < 0.72)   rpc = ReferenceCountingPointer<Surface>(new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 500, 500, -700, 700 ) ) );
  else if (eta < 1.04)   rpc = ReferenceCountingPointer<Surface>(new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 520, 520, -700, 700 ) ) );
  else                      rpc = ReferenceCountingPointer<Surface>(new  BoundDisk( GlobalPoint(0.,0.,790.), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
  edm::ESHandle<Propagator> propagator;
  theEs.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAlong", propagator);
  TrajectoryStateOnSurface trackAtRPC =  propagator->propagate( theState, *rpc);
  return trackAtRPC;
}
