
#include "SynchroSelector.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"


#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/print.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"

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
#include "DataFormats/BeamSpot/interface/BeamSpot.h"


#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "TH1.h"
#include <cmath>
using namespace edm;
using namespace std;

SynchroSelector::SynchroSelector(const edm::ParameterSet & cfg) 
  : theConfig(cfg), hDxy(0),hNum(0),hDeltaEta(0),hDeltaPhi(0) 
{ }

bool SynchroSelector::checkMatching( const TrajectoryStateOnSurface & tsos, 
    float rpcEta, float rpcPhi, const edm::Event&ev, const edm::EventSetup& es) 
{
  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);
  ReferenceCountingPointer<Surface> rpc;
  if (rpcEta < -1.04)      rpc = ReferenceCountingPointer<Surface>(new  BoundDisk( GlobalPoint(0.,0.,-800.), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
  else if (rpcEta < 1.04)  rpc = ReferenceCountingPointer<Surface>(new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 500, 520, -700, 700 ) ) );
  else                     rpc = ReferenceCountingPointer<Surface>(new  BoundDisk( GlobalPoint(0.,0.,800.), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
  edm::ESHandle<Propagator> propagator;
  es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAny", propagator);
  TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(tsos, *rpc);
  if (!trackAtRPC.isValid()) return false;
  float phi = trackAtRPC.globalPosition().phi();
  float dphi = phi-rpcPhi;
  if (dphi < -M_PI) dphi+=2*M_PI;
  if (dphi > M_PI) dphi-=2*M_PI;
  float eta = trackAtRPC.globalPosition().eta();
  float deta = eta-rpcEta;
  if (fabs(dphi) < mindphi ) mindphi = fabs(dphi);
  if (fabs(deta) < mindeta ) mindeta = fabs(deta);


  double maxDeltaEta = theConfig.getParameter<double>("maxDeltaEta");  
  double maxDeltaPhi = theConfig.getParameter<double>("maxDeltaPhi");  
  return ( fabs(dphi) < maxDeltaPhi && fabs(deta) < maxDeltaEta) ? true : false; 
}

bool SynchroSelector::checkTriggerMatching( const TrajectoryStateOnSurface & tsos,  const edm::Event&ev, const edm::EventSetup& es) 
{
  edm::Handle<L1MuGMTReadoutCollection> pCollection;
  InputTag l1MuReadout = theConfig.getParameter<edm::InputTag>("l1MuReadout");
  ev.getByLabel(l1MuReadout,pCollection);
  L1MuGMTReadoutCollection const* gmtrc = pCollection.product();
  if (!gmtrc) return false;

  vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  vector<L1MuGMTReadoutRecord>::const_iterator RRItr;
  typedef vector<L1MuRegionalCand>::const_iterator ITC;
  for( RRItr = gmt_records.begin() ; RRItr != gmt_records.end() ; RRItr++ ) {
    for (int i=1; i<=2; ++i) {
      vector<L1MuRegionalCand> Cands = (i==1) ? RRItr->getBrlRPCCands() : RRItr->getFwdRPCCands();
      for(ITC it = Cands.begin() ; it != Cands.end() ; ++it ) {
        if (it->empty()) continue;
        if (checkMatching(tsos, it->etaValue(), it->phiValue(),ev,es)) return true; 
      }
    }
  }
  return false;
} 

bool SynchroSelector::takeIt(const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es) 
{
  std::string theOption = theConfig.getParameter<std::string>("collection");
  bool theCheckL1Matching = theConfig.getParameter<bool>("matchL1RPC");
  float theMaxTIP = theConfig.getParameter<double>("maxTIP");
  float theMinPt = theConfig.getParameter<double>("minPt");

  edm::Handle<reco::TrackCollection> trackCollection;
  ev.getByLabel(InputTag(theOption),trackCollection);

  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);

  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);

  math::XYZPoint reference(0.,0.,0.);
  if (theConfig.exists("beamSpot")) {
    edm::InputTag beamSpot =  theConfig.getParameter<edm::InputTag>("beamSpot");
    edm::Handle<reco::BeamSpot> bsHandle;
    ev.getByLabel( beamSpot, bsHandle);
    if (bsHandle.isValid()) reference = math::XYZPoint(bsHandle->x0(), bsHandle->y0(), bsHandle->z0());
  }


  reco::TrackCollection tracks = *(trackCollection.product());
//cout <<"#RECONSTRUCTED tracks: " << tracks.size() << endl;
  if (hNum) hNum->Fill(tracks.size());

  bool inside = false;
  mindeta = 100.;
  mindphi = 100.;

  typedef reco::TrackCollection::const_iterator IT;
  for (IT it = tracks.begin(); it !=tracks.end(); ++it) {
    const reco::Track & track = *it;
    if (track.pt() < theMinPt ) continue;
    double dxy = track.dxy(reference);

    TrajectoryStateOnSurface aTSOS = TrajectoryStateTransform().outerStateOnSurface(track, *globalGeometry, magField.product());
    if (theCheckL1Matching && !checkTriggerMatching(aTSOS, ev,es) ) continue; 

    edm::ESHandle<Propagator> propagator;
    es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAny", propagator);

    TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(aTSOS, globalGeometry->idToDet(det)->surface());
    if (!trackAtRPC.isValid()) continue;
    globalGeometry->idToDet(det)->specificSurface();
    std::cout <<" **** "<<theOption<<" position at RPC det:"<<det.rawId()
                        //<< is :"<globalGeometry->idToDet(det)->position()
                        <<", r= "<<trackAtRPC.globalPosition().perp()
                        <<", z= "<<trackAtRPC.globalPosition().z()
                        <<", phi= "<<trackAtRPC.globalPosition().phi()
                        <<", eta= "<<trackAtRPC.globalPosition().eta()
                        //<<" localPosition: "<<trackAtRPC.localPosition()
                        //<<" localError: (xx:"<<trackAtRPC.localError().positionError()
                       <<" isInside: "<< globalGeometry->idToDet(det)->surface().bounds().inside( trackAtRPC.localPosition())
                       << std::endl;
    float scale = 0.; 
    if (     trackAtRPC.localError().positionError().xx() > 2500 || trackAtRPC.localError().positionError().yy() > 2500) continue;
    else if (trackAtRPC.localError().positionError().xx() > 1000 || trackAtRPC.localError().positionError().yy() > 1000)  scale = 0;
    else if (trackAtRPC.localError().positionError().xx() > 500  || trackAtRPC.localError().positionError().yy() > 500)   scale = 1;
    else if (trackAtRPC.localError().positionError().xx() > 100  || trackAtRPC.localError().positionError().yy() > 100)   scale = 2;
    else scale = 3.;

    if (globalGeometry->idToDet(det)->surface().bounds().inside( trackAtRPC.localPosition(), trackAtRPC.localError().positionError(), scale)) {
      if (hDxy) hDxy->Fill(dxy);
      if (dxy < theMaxTIP) inside = true;
    }
  }
  if (mindphi < 99.) hDeltaPhi->Fill(mindphi);
  if (mindeta < 99.) hDeltaEta->Fill(mindeta);

  return inside;
} 
