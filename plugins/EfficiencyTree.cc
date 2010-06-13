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
#include "DataFormats/DetId/interface/DetIdCollection.h"


EfficiencyTree::EfficiencyTree(const edm::ParameterSet& cfg)
  : theConfig(cfg), theTree(0), event(0), l1RpcColl(0) , l1OtherColl(0)
{
}

void EfficiencyTree::beginJob()
{
  theFile = new TFile(theConfig.getParameter<std::string>("treeFileName").c_str(),"RECREATE");
  theTree = new TTree("tL1Rpc","L1RpcEfficciency");

  theTree->Branch("event","EventObj",&event,32000,99);

  theTree->Branch("muPt", &muon.pt);
  theTree->Branch("muEta",&muon.eta);
  theTree->Branch("muPhi",&muon.phi);

  theTree->Branch("tkPt", &track.pt);
  theTree->Branch("tkEta",&track.eta);
  theTree->Branch("tkPhi",&track.phi);

  theTree->Branch("hitBarrel",&hitBarrel);
  theTree->Branch("hitEndcap",&hitEndcap);
  theTree->Branch("detBarrel",&detBarrel);
  theTree->Branch("detEndcap",&detEndcap);

  theTree->Branch("l1RpcColl","L1ObjColl",&l1RpcColl,32000,99);
  theTree->Branch("l1OtherColl","L1ObjColl",&l1OtherColl,32000,99);

}

void EfficiencyTree::endJob()
{
   theFile->Write();
   delete theFile;
}

void EfficiencyTree::analyze(const edm::Event &ev, const edm::EventSetup &es)
{
  //clean event
  event = new EventObj;
  event->bx = ev.bunchCrossing();
  event->orbit = ev.orbitNumber();
  event->time = ev.time().value();
  event->id = ev.id().event();
  event->run = ev.run();
  event->lumi = ev.luminosityBlock();

  muon = TrackObj();
  track = TrackObj();

  hitBarrel = std::vector<bool>(6,false);
  hitEndcap = std::vector<bool>(3,false);
  detBarrel = std::vector<unsigned int>(6,0);
  detEndcap = std::vector<unsigned int>(3,0);

  l1RpcColl = new L1ObjColl;
  l1OtherColl = new L1ObjColl;

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

    //rpc dets compatible with muon
    const TrackingGeometry::DetIdContainer & detIds = rpcGeometry->detIds();
    if ( (fabs(muon.eta) > 1.24 && muon.pt > 4) || (muon.pt >7.) ) {
    for (TrackingGeometry::DetIdContainer::const_iterator it = detIds.begin(); it != detIds.end(); ++it) {
      const GeomDet * det = rpcGeometry->idToDet(*it);
      GlobalPoint detPosition = det->position();
      //if (fabs(muon.eta- detPosition.eta()) > 1.) continue;
      if (deltaR(muon.eta, muon.phi, detPosition.eta(), detPosition.phi()) > 3.) continue;
      TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(muTSOS, det->surface());
      if (!trackAtRPC.isValid()) continue;
      if (! (det->surface().bounds().inside(trackAtRPC.localPosition()))) continue;
      RPCDetId rpcDet(*it);
      int region = rpcDet.region();
      if (region==0) {
        int sM = rpcDet.station();
        int sI = rpcDet.layer();
        int layer = (sM <=2)? 2*(sM-1)+sI : sM+2;
        detBarrel[layer-1]++;
      } else {
        detEndcap[rpcDet.station()-1]++;
      }
    } 
    }
    
  }

  //create L1muon helper 
  edm::InputTag l1Tag(theConfig.getParameter<edm::InputTag>("l1MuReadout"));
  L1ObjMaker l1(l1Tag,ev);

  // set L1Others
  std::vector<L1Obj> l1Others=  l1(L1ObjMaker::DT,L1ObjMaker::CSC);
  std::vector<bool> l1OthersMatching(l1Others.size(),false);

  edm::Handle<reco::TrackCollection> tracks;
  ev.getByLabel( theConfig.getParameter<std::string>("trackColl"), tracks);
  typedef reco::TrackCollection::const_iterator IT;
  for (IT it = tracks->begin(); it !=tracks->end(); ++it) {
    const reco::Track & muTrack = *it;
    if (muTrack.pt() < theConfig.getParameter<double>("minPt")) continue;
    if (muTrack.dxy(reference) >  theConfig.getParameter<double>("maxTIP")) continue;
    if (fabs(muTrack.eta()) >  theConfig.getParameter<double>("maxEta")) continue;
    if (muTrack.pt() < track.pt) continue;
    TrajectoryStateOnSurface tTSOS = TrajectoryStateTransform().outerStateOnSurface(muTrack, *globalGeometry, magField.product());
    TrackToL1ObjMatcher matcher(theConfig.getParameter<edm::ParameterSet>("dtcscMatcherPSet"));

    bool isTrackCompatible = false;
    std::vector<bool> l1OthersThisTrackMatching(l1Others.size(),false);

    for (unsigned int i=0; i< l1Others.size(); ++i) {
      if ( matcher(l1Others[i], tTSOS, ev,es) ) {
        isTrackCompatible = true; 
        l1OthersThisTrackMatching[i]=true;
      }
    }
    if (isTrackCompatible) {
      if (!theMuon) tsos = tTSOS;
      track.pt = muTrack.pt();
      track.eta = muTrack.eta();
      track.phi = muTrack.phi();
      l1OthersMatching = l1OthersThisTrackMatching;
    }
  }
  l1OtherColl->set(l1Others);
  l1OtherColl->set(l1OthersMatching);


  // set L1 RPC 
  std::vector<L1Obj> l1Rpcs = l1(L1ObjMaker::RPCB,L1ObjMaker::RPCF);
  std::vector<bool> l1RpcsMatching(l1Rpcs.size(), false);
  TrackToL1ObjMatcher matcher(theConfig.getParameter<edm::ParameterSet>("rpcMatcherPSet"));
  if (tsos.isValid()) for (unsigned int i=0; i< l1Rpcs.size(); ++i)  if (matcher(l1Rpcs[i], tsos, ev,es)) l1RpcsMatching[i]=true;
  l1RpcColl->set(l1Rpcs);
  l1RpcColl->set(l1RpcsMatching);


  // fill ntuple and cleanup
  if (tsos.isValid() || l1Rpcs.size() != 0 ||  l1Others.size() !=0) theTree->Fill();
  delete event;
  delete l1RpcColl;
  delete l1OtherColl;

}
