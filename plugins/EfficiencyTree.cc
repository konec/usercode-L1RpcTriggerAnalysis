#include "EfficiencyTree.h"
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"


#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjMaker.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackToL1ObjMatcher.h"
#include "DataFormats/Math/interface/deltaR.h"


#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/RPCGeometry/interface/RPCGeomServ.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"


EfficiencyTree::EfficiencyTree(const edm::ParameterSet& cfg)
  : theConfig(cfg), theTree(0)
{
}

void EfficiencyTree::beginJob()
{
  theFile = new TFile(theConfig.getParameter<std::string>("treeFileName").c_str(),"RECREATE");
  theTree = new TTree("tL1Rpc","L1RpcEfficciency");

  theTree->Branch("evBx",&event.bx);
  theTree->Branch("evId",&event.id);
  theTree->Branch("evLumi",&event.lumi);
  theTree->Branch("evRun",&event.run);

  theTree->Branch("muPt", &muon.pt);
  theTree->Branch("muEta",&muon.eta);
  theTree->Branch("muPhi",&muon.phi);

  theTree->Branch("tkPt", &track.pt);
  theTree->Branch("tkEta",&track.eta);
  theTree->Branch("tkPhi",&track.phi);

  theTree->Branch("rpcPt", &l1Rpc.pt);
  theTree->Branch("rpcEta",&l1Rpc.eta);
  theTree->Branch("rpcPhi",&l1Rpc.phi);
  theTree->Branch("rpcBx",&l1Rpc.bx);
  theTree->Branch("rpcQ",&l1Rpc.q);

  theTree->Branch("otherPt", &l1Other.pt);
  theTree->Branch("otherEta",&l1Other.eta);
  theTree->Branch("otherPhi",&l1Other.phi);
  theTree->Branch("otherBx",&l1Other.bx);
  theTree->Branch("otherQ",&l1Other.q);

  theTree->Branch("hitBarrel",&hitBarrel);
  theTree->Branch("hitEndcap",&hitEndcap);

}

void EfficiencyTree::endJob()
{
   theFile->Write();
   delete theFile;
}

void EfficiencyTree::analyze(const edm::Event &ev, const edm::EventSetup &es)
{
  //clean event
  event.bx = ev.bunchCrossing();
  event.id = ev.id().event();
  event.run = ev.run();
  event.lumi = ev.luminosityBlock();
  muon = TrackObj();
  track = TrackObj();
  l1Rpc = L1Obj();
  l1Other = L1Obj();
  hitBarrel = std::vector<bool>(6,false);
  hitEndcap = std::vector<bool>(3,false);

  //getBeamSpot, 
  math::XYZPoint reference(0.,0.,0.);
  edm::InputTag beamSpot =  theConfig.getParameter<edm::InputTag>("beamSpot");
  edm::Handle<reco::BeamSpot> bsHandle;
  ev.getByLabel( beamSpot, bsHandle);
  if (bsHandle.isValid()) reference = math::XYZPoint(bsHandle->x0(), bsHandle->y0(), bsHandle->z0());
  //geometry, magfield
  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);

   //get Muon
  edm::Handle<reco::MuonCollection> muons;
  ev.getByLabel( theConfig.getParameter<std::string>("muonColl"), muons);
  const reco::Muon * theMuon = 0;
  for (reco::MuonCollection::const_iterator im = muons->begin(); im != muons->end(); ++im) {
   if (!im->isTrackerMuon()) continue;
   if (!im->isGlobalMuon()) continue;
   if (im->track()->pt() < theConfig.getParameter<double>("minPt")) continue;
   if (fabs(im->track()->eta()) >  theConfig.getParameter<double>("maxEta")) continue;
   if (im->track()->dxy(reference) >  theConfig.getParameter<double>("maxTIP")) continue;
   if (!theMuon || (im->track()->pt() > theMuon->track()->pt()) ) theMuon = &(*im); 
  }
   // best RPC muon
  TrajectoryStateOnSurface tsos;
  if (theMuon) { 
    muon.pt = theMuon->track()->pt(); 
    muon.eta = theMuon->track()->eta();
    muon.phi = theMuon->track()->phi();

    // get muon tsos for muon matching to RPC
    TrajectoryStateOnSurface muTSOS = TrajectoryStateTransform().outerStateOnSurface(*(theMuon->track()), *globalGeometry, magField.product());
    tsos = muTSOS;


    //rpc Hits compatible with muon tsts
    edm::Handle<RPCRecHitCollection> recHits;
    ev.getByLabel("rpcRecHits", recHits);
    edm::ESHandle<RPCGeometry> rpcGeometry;
    es.get<MuonGeometryRecord>().get(rpcGeometry);
    edm::ESHandle<Propagator> propagator;
    es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAny", propagator);
    typedef RPCRecHitCollection::const_iterator IH;
    for (IH ih=recHits->begin(); ih != recHits->end(); ++ih) {
      RPCDetId rpcDet = ih->rpcId();
      LocalPoint hitPoint = ih->localPosition();
      LocalError hitError = ih->localPositionError();
      TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(muTSOS, globalGeometry->idToDet(rpcDet)->surface());
      if (!trackAtRPC.isValid()) continue;
      LocalPoint trackAtRPCPoint = trackAtRPC.localPosition() ;
      LocalError trackAtRPCError = trackAtRPC.localError().positionError();
      float distX = hitPoint.x()-trackAtRPCPoint.x();
      float distY = hitPoint.y()-trackAtRPCPoint.y();
      float pullX = distX/ sqrt( trackAtRPCError.xx()+hitError.xx());
      float pullY = distY/ sqrt( trackAtRPCError.yy()+hitError.yy());
      bool hitCompatible = (fabs(pullX) < 3 && fabs(pullY) <3);
      if (hitCompatible) {
        int region = rpcDet.region();
        if (region==0) {
          int sM = rpcDet.station();
          int sI = rpcDet.layer(); 
          int layer = (sM <=2)? 2*(sM-1)+sI : sM+2;
          hitBarrel[layer-1]=true;
        } else {
          hitEndcap[rpcDet.station()-1]=true;
        }
      }
    }
  }

  //create L1muon helper 
  edm::InputTag l1Tag(theConfig.getParameter<edm::InputTag>("l1MuReadout"));
  L1ObjMaker l1(l1Tag,ev);

  //get all tracks, check compatibility with DT and CSC
  edm::Handle<reco::TrackCollection> tracks;
  ev.getByLabel( theConfig.getParameter<std::string>("trackColl"), tracks);
  typedef reco::TrackCollection::const_iterator IT;
  for (IT it = tracks->begin(); it !=tracks->end(); ++it) {
    const reco::Track & muTrack = *it;
    if (muTrack.pt() < theConfig.getParameter<double>("minPt")) continue;
    if (muTrack.dxy(reference) >  theConfig.getParameter<double>("maxTIP")) continue;
    if (fabs(muTrack.eta()) >  theConfig.getParameter<double>("maxEta")) continue;
    std::vector<L1Obj> l1Others=  l1(L1ObjMaker::DT,L1ObjMaker::CSC);
    TrajectoryStateOnSurface tTSOS = TrajectoryStateTransform().outerStateOnSurface(muTrack, *globalGeometry, magField.product());
    int minBx = 100;
    TrackToL1ObjMatcher matcher(theConfig.getParameter<edm::ParameterSet>("dtcscMatcherPSet"));
    for (std::vector<L1Obj>::const_iterator it = l1Others.begin(); it != l1Others.end(); ++it) {
      bool isCompatible = matcher(*it, tTSOS, ev,es);
      if (isCompatible && it->bx < minBx) {
        if (!theMuon) tsos = tTSOS;
        l1Other = *it;
        minBx = l1Other.bx;
        track.pt = muTrack.pt();
        track.eta = muTrack.eta();
        track.phi = muTrack.phi(); 
      } 
    }
  }

  if (tsos.isValid()) { 

  // get rpc candidates, check comp. with muon or track matching DT/CSSC 
  std::vector<L1Obj> l1Rpcs = l1(L1ObjMaker::RPCB,L1ObjMaker::RPCF);
  int minBx = 100;
  TrackToL1ObjMatcher matcher(theConfig.getParameter<edm::ParameterSet>("rpcMatcherPSet"));
  for (std::vector<L1Obj>::const_iterator it = l1Rpcs.begin(); it != l1Rpcs.end(); ++it) {
    bool isCompatible = matcher(*it, tsos, ev,es);
    if (isCompatible && it->bx < minBx) {
      l1Rpc = *it;
      minBx = l1Rpc.bx;
    }
  } 
  }

  //finally take RPC or other

  std::vector<L1Obj> l1Rpcs =  l1(L1ObjMaker::RPCB,L1ObjMaker::RPCF);
  int maxQ = -1;
  for (std::vector<L1Obj>::const_iterator it = l1Rpcs.begin(); it != l1Rpcs.end(); ++it) {
    if (it->q > maxQ) {
      l1Rpc = *it;
      maxQ = l1Rpc.q;
    }
  }

  std::vector<L1Obj> l1Others = l1(L1ObjMaker::DT,L1ObjMaker::CSC);
  maxQ=-1;
  for (std::vector<L1Obj>::const_iterator it = l1Others.begin(); it != l1Others.end(); ++it) {
    if (it->q > maxQ) {
      l1Other = *it;
      maxQ = l1Other.q;
    }
  }

  if (!tsos.isValid() && l1Rpcs.size()==0 && l1Others.size()==0) return;
  theTree->Fill();
   
}
