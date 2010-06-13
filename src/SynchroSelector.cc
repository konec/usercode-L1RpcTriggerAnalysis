#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroSelector.h"
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
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"

#include "TH1.h"
#include <cmath>
using namespace edm;
using namespace std;

SynchroSelector::SynchroSelector(const edm::ParameterSet & cfg) 
  : theConfig(cfg), hDxy(0),hNum(0),hDeltaEta(0),hDeltaPhi(0), 
                    hPullX(0),hDistX(0)
{ }

bool SynchroSelector::checkL1RpcMatching( const TrajectoryStateOnSurface & tsos,  const edm::Event&ev, const edm::EventSetup& es) 
{
  edm::Handle<L1MuGMTReadoutCollection> pCollection;
  InputTag l1MuReadout = theConfig.getParameter<edm::InputTag>("l1MuReadout");
  ev.getByLabel(l1MuReadout,pCollection);
  L1MuGMTReadoutCollection const* gmtrc = pCollection.product();
  if (!gmtrc) return false;

  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);
  ReferenceCountingPointer<Surface> rpc;

  vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  vector<L1MuGMTReadoutRecord>::const_iterator RRItr;
  typedef vector<L1MuRegionalCand>::const_iterator ITC;
  for( RRItr = gmt_records.begin() ; RRItr != gmt_records.end() ; RRItr++ ) {
    for (int i=1; i<=2; ++i) {
      vector<L1MuRegionalCand> Cands = (i==1) ? RRItr->getBrlRPCCands() : RRItr->getFwdRPCCands();
      for(ITC it = Cands.begin() ; it != Cands.end() ; ++it ) {
        if (it->empty()) continue;
        //propagate and check matching for candidate
        float rpcEta = it->etaValue();
        float rpcPhi = it->phiValue();
        if (rpcEta < -1.04)      rpc = ReferenceCountingPointer<Surface>(new  BoundDisk( GlobalPoint(0.,0.,-800.), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
        else if (rpcEta < 1.04)  rpc = ReferenceCountingPointer<Surface>(new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 500, 520, -700, 700 ) ) );
        else                     rpc = ReferenceCountingPointer<Surface>(new  BoundDisk( GlobalPoint(0.,0.,800.), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
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
        if (fabs(dphi) < mindphi ) mindphi = fabs(dphi);
        if (fabs(deta) < mindeta ) mindeta = fabs(deta);

        double maxDeltaEta = theConfig.getParameter<double>("maxDeltaEta");  
        double maxDeltaPhi = theConfig.getParameter<double>("maxDeltaPhi");  
        bool matching = ( fabs(dphi) < maxDeltaPhi && fabs(deta) < maxDeltaEta) ? true : false; 
        if (matching) return true;
      }
    }
  }
  return false;
} 

bool SynchroSelector::takeIt(const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es) 
{
  bool result = false;
  float theMaxTIP = theConfig.getParameter<double>("maxTIP");
  float theMinPt = theConfig.getParameter<double>("minPt");

  edm::Handle<reco::TrackCollection> trackCollection;
  ev.getByLabel(InputTag( theConfig.getParameter<std::string>("collection")),trackCollection);

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

  mindeta = 100.;
  mindphi = 100.;

  typedef reco::TrackCollection::const_iterator IT;
  for (IT it = tracks.begin(); it !=tracks.end(); ++it) {
    const reco::Track & track = *it;
    if (track.pt() < theMinPt ) continue;
    double dxy = track.dxy(reference);
    if (dxy > theMaxTIP) continue;
  
    TrajectoryStateOnSurface aTSOS = TrajectoryStateTransform().outerStateOnSurface(track, *globalGeometry, magField.product());
    if (theConfig.getParameter<bool>("matchL1RPC") && !checkL1RpcMatching(aTSOS, ev,es) ) continue; 
    if (!checkRpcDetMatching(aTSOS,det,ev,es)) continue;
    if (theConfig.getParameter<bool>("checkUniqueRecHitMatching") && !checkUniqueRecHitMatching(aTSOS,det,ev,es)) continue;

    if (hDxy) hDxy->Fill(dxy);
    result = true;
  }

  if (result && mindphi < 99.) hDeltaPhi->Fill(mindphi);
  if (result && mindeta < 99.) hDeltaEta->Fill(mindeta);

  return result;
} 

bool SynchroSelector::checkRpcDetMatching( const TrajectoryStateOnSurface & tsos,  const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es)
{
  edm::ESHandle<Propagator> propagator;
  es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAlong", propagator);

  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);

  GlobalPoint detPos, trackPos;
  TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(tsos, globalGeometry->idToDet(det)->surface());
  if (!trackAtRPC.isValid()) return false;
  //globalGeometry->idToDet(det)->specificSurface();
  detPos = globalGeometry->idToDet(det)->position();
  trackPos = trackAtRPC.globalPosition();

/*
  std::cout <<" **** "<<theConfig.getParameter<std::string>("collection")<<" position at RPC det:"<<det.rawId()
                      //<< is :"<globalGeometry->idToDet(det)->position()
                      <<", r= "<<trackAtRPC.globalPosition().perp()
                      <<", z= "<<trackAtRPC.globalPosition().z()
                      <<", phi= "<<trackAtRPC.globalPosition().phi()
                      <<", eta= "<<trackAtRPC.globalPosition().eta()
                      //<<" localPosition: "<<trackAtRPC.localPosition()
                      //<<" localError: (xx:"<<trackAtRPC.localError().positionError()
                     <<" isInside: "<< globalGeometry->idToDet(det)->surface().bounds().inside( trackAtRPC.localPosition())
                     << std::endl;
*/
  float scale = 0.; 
  if (     trackAtRPC.localError().positionError().xx() > 2500 || trackAtRPC.localError().positionError().yy() > 2500) return false;
  else if (trackAtRPC.localError().positionError().xx() > 1000 || trackAtRPC.localError().positionError().yy() > 1000)  scale = 0;
  else if (trackAtRPC.localError().positionError().xx() > 500  || trackAtRPC.localError().positionError().yy() > 500)   scale = 1;
  else if (trackAtRPC.localError().positionError().xx() > 100  || trackAtRPC.localError().positionError().yy() > 100)   scale = 2;
  else scale = 3.;

   
  bool inside = globalGeometry->idToDet(det)->surface().bounds().inside( trackAtRPC.localPosition(), trackAtRPC.localError().positionError(), scale);
//if (inside) { std::cout <<" detector: " << detPos <<" track: "<< trackPos << std::endl; thePos.push_back(trackPos);
  return inside;
}

bool SynchroSelector::checkUniqueRecHitMatching( const TrajectoryStateOnSurface & tsos,  const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es)
{
  //propagate to det
  edm::ESHandle<Propagator> propagator;
  es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAlong", propagator);
  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(tsos, globalGeometry->idToDet(det)->surface());
  if (!trackAtRPC.isValid()) return false;
  
  LocalPoint trackAtRPCPoint = trackAtRPC.localPosition() ;
  LocalError trackAtRPCError = trackAtRPC.localError().positionError();

  edm::Handle<RPCRecHitCollection> rpcHits;
  ev.getByType(rpcHits);
  bool matched = false;
  bool unique = true;
  pair<RPCRecHitCollection::const_iterator, RPCRecHitCollection::const_iterator> recHitsInDet =  rpcHits->get(det);
  for (RPCRecHitCollection::const_iterator ih = recHitsInDet.first; ih != recHitsInDet.second; ++ih) {
    LocalPoint hitPoint = ih->localPosition();
    LocalError hitError = ih->localPositionError();
    float distX = hitPoint.x()-trackAtRPCPoint.x();
    float pullX = distX/ sqrt( trackAtRPCError.xx()+hitError.xx()); 

    if (fabs(pullX) < 2. && fabs(distX) < 20.) {
      matched = true;
    } else {
      unique = false;
    } 
    if (hPullX) hPullX->Fill(pullX);
    if (hDistX) hDistX->Fill(distX);
  }
  
  return (matched && unique);
}
