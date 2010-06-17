#include "UserCode/L1RpcTriggerAnalysis/interface/TrackToL1ObjMatcher.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"

#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "DataFormats/GeometrySurface/interface/BoundCylinder.h"
#include "DataFormats/GeometrySurface/interface/SimpleCylinderBounds.h"
#include "DataFormats/GeometrySurface/interface/BoundDisk.h"
#include "DataFormats/GeometrySurface/interface/SimpleDiskBounds.h"

using namespace edm;
using namespace std;

static const TrackToL1ObjMatcher::LastResult dummy = {false, 100., 100.};

TrackToL1ObjMatcher::TrackToL1ObjMatcher(const edm::ParameterSet & cfg) : theConfig(cfg), theLastResult(dummy) { }

bool TrackToL1ObjMatcher::operator()(float l1ObjEta, float l1ObjPhi, const TrajectoryStateOnSurface & tsos,  const edm::Event&ev, const edm::EventSetup& es) const
{
  theLastResult = dummy;
  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);
  ReferenceCountingPointer<Surface> rpc;

  //propagate and check matching for candidate
  float rpcEta = l1ObjEta;
  float rpcPhi = l1ObjPhi;
  if (rpcEta < -1.04)       rpc = ReferenceCountingPointer<Surface>(new  BoundDisk( GlobalPoint(0.,0.,-790.), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
  else if (rpcEta < -0.72)  rpc = ReferenceCountingPointer<Surface>(new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 520, 520, -700, 700 ) ) );
  else if (rpcEta < 0.72)   rpc = ReferenceCountingPointer<Surface>(new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 500, 500, -700, 700 ) ) );
  else if (rpcEta < 1.04)   rpc = ReferenceCountingPointer<Surface>(new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 520, 520, -700, 700 ) ) );
  else                      rpc = ReferenceCountingPointer<Surface>(new  BoundDisk( GlobalPoint(0.,0.,790.), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
  edm::ESHandle<Propagator> propagator;
  es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAlong", propagator);
  TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(tsos, *rpc);
  if (!trackAtRPC.isValid()) return false;
  float phi = trackAtRPC.globalPosition().phi();
  float dphi = phi-rpcPhi;
  if (dphi < -M_PI) dphi+=2*M_PI;
  if (dphi > M_PI) dphi-=2*M_PI;
  float eta = trackAtRPC.globalPosition().eta();
  float deta = eta-rpcEta;

  double maxDeltaEta = theConfig.getParameter<double>("maxDeltaEta");
  double maxDeltaPhi = theConfig.getParameter<double>("maxDeltaPhi");
  bool matching = ( fabs(dphi) < maxDeltaPhi && fabs(deta) < maxDeltaEta) ? true : false;

  theLastResult.isValid = true;
  theLastResult.deltaEta = deta;
  theLastResult.deltaPhi = dphi;

  if (matching) return true; 
  else return false;
}
