#include "EfficiencyTree.h"
#include <vector>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"


#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
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
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"


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

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"

template <class T> T sqr( T t) {return t*t;}

EfficiencyTree::BarrelAndLayer::BarrelAndLayer(const RPCDetId & rpcDet)
{
  theBarrel = (rpcDet.region()==0) ? true : false;
  theLayer = theBarrel ?
       ( rpcDet.station() <=2  ? 2*( rpcDet.station()-1)+ rpcDet.layer() : rpcDet.station()+2 )
      : rpcDet.station();
}


TrajectoryStateOnSurface EfficiencyTree::trackAtSurface(const reco::Muon* mu, const RPCDetId& rpc, const edm::Event &ev, const edm::EventSetup &es) const
{

  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);

  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);

//  edm::ESHandle<RPCGeometry> rpcGeometry;
//  es.get<MuonGeometryRecord>().get(rpcGeometry);

  edm::ESHandle<Propagator> propagator;


  typedef std::vector<Trajectory> Trajectories;
  edm::Handle<Trajectories> trajectories;
  edm::InputTag refitTag("globalMuons","Refitted");
  ev.getByLabel(refitTag,trajectories);
  Trajectory muTrajectory;
  double minDR =  0.3;
  for (Trajectories::const_iterator it=trajectories->begin(); it != trajectories->end(); ++it) {
    double diff = deltaR(mu->track()->eta(), 
                         mu->track()->phi(),
                         it->geometricalInnermostState().globalMomentum().eta(), 
                         it->geometricalInnermostState().globalMomentum().phi());
    if (diff < minDR) { minDR = diff; muTrajectory = *it; }
  }
  hMinDeltaRTrajMu->Fill(minDR);

  const GeomDet * det = globalGeometry->idToDet(rpc);
  GlobalPoint detPosition = det->position();

  TrajectoryStateOnSurface muTSOS = muTrajectory.empty() ?
       //TrajectoryStateTransform().innerStateOnSurface(*(mu->track()), *globalGeometry, &*magField);
       TrajectoryStateTransform().outerStateOnSurface(*(mu->track()), *globalGeometry, &*magField)
       //TrajectoryStateTransform().outerStateOnSurface(*(mu->globalTrack()), *globalGeometry, &*magField);
//    :  muTrajectory.closestMeasurement(detPosition).forwardPredictedState();
//    :  muTrajectory.closestMeasurement(detPosition).backwardPredictedState();
      :  muTrajectory.closestMeasurement(detPosition).updatedState();

  bool along = true;
  if (BarrelAndLayer(rpc).isBarrel()) {
    if (muTSOS.globalPosition().perp() > detPosition.perp()) along = false;
  } else {
    if ( fabs(muTSOS.globalPosition().z()) > fabs(detPosition.z()) ) along = false;
  }

  std::string propagatorName = along? "SteppingHelixPropagatorAlong" : "SteppingHelixPropagatorOpposite";
  es.get<TrackingComponentsRecord>().get(propagatorName, propagator);

  TrajectoryStateOnSurface result =  propagator->propagate(muTSOS, det->surface());

  //temporary hack!!!!
  if (!result.isValid() && !muTrajectory.empty()) {
    muTSOS = TrajectoryStateTransform().outerStateOnSurface(*(mu->track()), *globalGeometry, &*magField);
    es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAlong",propagator);
    result =  propagator->propagate(muTSOS, det->surface());
  }
  
  return result;
}

EfficiencyTree::EfficiencyTree(const edm::ParameterSet& cfg)
  : theConfig(cfg), theTree(0), event(0), muon(0), track(0), l1RpcColl(0) , l1OtherColl(0)
{ }

void EfficiencyTree::beginJob()
{
  theFile = new TFile(theConfig.getParameter<std::string>("treeFileName").c_str(),"RECREATE");
  theTree = new TTree("tL1Rpc","L1RpcEfficciency");

  theTree->Branch("event","EventObj",&event,32000,99);

  theTree->Branch("muon","MuonObj",&muon,32000,99);
  theTree->Branch("track", "TrackObj",&track,32000,99);

  theTree->Branch("hitBarrel",&hitBarrel);
  theTree->Branch("hitEndcap",&hitEndcap);
  theTree->Branch("detBarrel",&detBarrel);
  theTree->Branch("detEndcap",&detEndcap);

  theTree->Branch("l1RpcColl","L1ObjColl",&l1RpcColl,32000,99);
  theTree->Branch("l1OtherColl","L1ObjColl",&l1OtherColl,32000,99);

  histos.SetOwner();

  for (unsigned int i=1; i<=6; ++i) {
    std::ostringstream namePullX_B; namePullX_B <<"hPullX_B"<< i;
    std::ostringstream nameDistX_B; nameDistX_B <<"hDistX_B"<< i;
    hPullX_B[i-1]= new TH1F(namePullX_B.str().c_str(),namePullX_B.str().c_str(),50,-5.,5.);  histos.Add( hPullX_B[i-1]);
    hDistX_B[i-1]= new TH1F(nameDistX_B.str().c_str(),nameDistX_B.str().c_str(),100,-20.,20.);  histos.Add( hDistX_B[i-1]);
  }
  for (unsigned int i=1; i<=3; ++i) {
    std::ostringstream namePullX_E; namePullX_E <<"hPullX_E" << i;
    std::ostringstream nameDistX_E; nameDistX_E <<"hDistX_E"<< i;
    hPullX_E[i-1]= new TH1F(namePullX_E.str().c_str(),namePullX_E.str().c_str(),50,-5.,5.);  histos.Add( hPullX_E[i-1]);
    hDistX_E[i-1]= new TH1F(nameDistX_E.str().c_str(),nameDistX_E.str().c_str(),100,-20.,20.);  histos.Add( hDistX_E[i-1]);
  }
  hPullY = new TH1F("hPullY","hPullY",50,-5.,5.);  histos.Add( hPullY);

  hDeltaR_Mu = new TH1F("hDeltaR_Mu"," deltaR L1RPC-muon",100,0.,1.); histos.Add( hDeltaR_Mu);
  hDeltaR_Tk = new TH1F("hDeltaR_Tk"," deltaR L1RPC-track ",100,0.,1.); histos.Add( hDeltaR_Tk);
  
  hPropToDetDeltaR = new TH1F("hPropToDetDeltaR","DR good Mu-DU before propagation",100,0.,3.);  histos.Add(hPropToDetDeltaR);
  hMinDeltaRTrajMu = new TH1F("hMinDeltaRTrajMu","hMinDeltaRTrajMu",100,0.,5.);histos.Add(hMinDeltaRTrajMu);
}

void EfficiencyTree::endJob()
{
   theFile->Write();
   delete theFile;

  std::string histoFile = theConfig.getParameter<std::string>("histoFileName");
  TFile f(histoFile.c_str(),"RECREATE");
  histos.Write();
  f.Close();

}

void EfficiencyTree::analyze(const edm::Event &ev, const edm::EventSetup &es)
{
//if (ev.id().event() != 44) return;
//std::cout <<"EVENT IS: " << ev.id().event() << std::endl;
  //clean event
  event = new EventObj;
  event->bx = ev.bunchCrossing();
  event->orbit = ev.orbitNumber();
  event->time = ev.time().value();
  event->id = ev.id().event();
  event->run = ev.run();
  event->lumi = ev.luminosityBlock();


  muon = new MuonObj();
  track = new TrackObj();

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

  //geometry, 
  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);

  //magfield
  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);

   //get Muon
  edm::Handle<reco::MuonCollection> muons;
  ev.getByLabel( theConfig.getParameter<std::string>("muonColl"), muons);
  const reco::Muon * theMuon = 0;
  for (reco::MuonCollection::const_iterator im = muons->begin(); im != muons->end(); ++im) {
    if (!im->isTrackerMuon()) continue;
    if (im->track()->pt() < theConfig.getParameter<double>("minPt")) continue;
    if (fabs(im->track()->eta()) >  theConfig.getParameter<double>("maxEta")) continue;
    if (im->track()->dxy(reference) >  theConfig.getParameter<double>("maxTIP")) continue;

    //remove muons without valid hits in tk and mu system
    int nTrackerHits =-99;
    int nRPCHits =-99;
    int nDTHits =-99;
    int nCSCHits =-99;
    if(im->isGlobalMuon()){
      const reco::HitPattern& hp = (im->globalTrack())->hitPattern();
      nTrackerHits = hp.numberOfValidTrackerHits();
    } 
    if(im->isStandAloneMuon()){
      const reco::HitPattern& hp = (im->standAloneMuon())->hitPattern();
      nRPCHits = hp.numberOfValidMuonRPCHits();
      nDTHits = hp.numberOfValidMuonDTHits();
      nCSCHits = hp.numberOfValidMuonCSCHits();
    }
    if(im->globalTrack().isNonnull()) std::cout <<"global muon (pt,eta,phi) "
                                                <<im->globalTrack()->pt()<<" "
                                                <<im->globalTrack()->eta()<<" "
                                                <<im->globalTrack()->phi()
                                                <<" number of RPC hits: " <<im->globalTrack()->hitPattern().numberOfValidMuonRPCHits() 
                                                <<std::endl;
    else std::cout <<"global brak " <<" Hits:Tk "<< nTrackerHits <<" RPC "<< nRPCHits <<" DT "<< nDTHits <<" CSC "<< nCSCHits <<std::endl;

    if (nTrackerHits<=0) continue;
    if ( nRPCHits==0 &&nDTHits==0 &&nCSCHits==0  ) continue;
    if (!im->globalTrack().isNonnull()) continue;

    if (!theMuon || (im->track()->pt() > theMuon->track()->pt()) ) theMuon = &(*im); 
  }


   // best RPC muon
  TrajectoryStateOnSurface tsos;
  if (theMuon) { 
    muon->setKine(theMuon->track()->pt(), theMuon->track()->eta(), theMuon->track()->phi(), theMuon->track()->charge());
    muon->setBits(theMuon->isGlobalMuon(), theMuon->isTrackerMuon(), theMuon->isStandAloneMuon(), theMuon->isCaloMuon(), theMuon->isMatchesValid());

    // get muon tsos for muon matching to RPC
    TrajectoryStateOnSurface muTSOS;
    muTSOS = TrajectoryStateTransform().outerStateOnSurface(*(theMuon->track()), *globalGeometry, &*magField);
    tsos = muTSOS;

    //
    //rpc Hits compatible with muon tsts
    //
    edm::Handle<RPCRecHitCollection> recHits;
    ev.getByLabel("rpcRecHits", recHits);
    typedef RPCRecHitCollection::const_iterator IH;
    for (IH ih=recHits->begin(); ih != recHits->end(); ++ih) {

      RPCDetId rpcDet = ih->rpcId();

//const GeomDet * det = globalGeometry->idToDet(rpcDet);
//GlobalPoint detPosition = det->position();
//std::cout <<"checking for hit in Det: r= "<<detPosition.perp()<<" phi="<<detPosition.phi()<<" z="<< detPosition.z()<<std::endl;
//std::cout <<"Barrel: "<<BarrelAndLayer(rpcDet).isBarrel()<<" layer: "<<BarrelAndLayer(rpcDet).layer()<<std::endl;
      TrajectoryStateOnSurface trackAtHit= trackAtSurface(theMuon, rpcDet, ev, es);
//if(!trackAtHit.isValid()) std::cout <<" TRAJ NOT VALID! "<< std::endl;
      if (!trackAtHit.isValid()) continue;
      LocalPoint HitPoint = ih->localPosition();
      LocalError hitError = ih->localPositionError();
      LocalPoint trackAtHitPoint = trackAtHit.localPosition() ;
      LocalError trackAtHitError = trackAtHit.localError().positionError();
      float distX = HitPoint.x()-trackAtHitPoint.x();
      float distY = HitPoint.y()-trackAtHitPoint.y();
      float pullX = distX/ sqrt( trackAtHitError.xx()+hitError.xx());
      float pullY = distY/ sqrt( trackAtHitError.yy()+hitError.yy());

      BarrelAndLayer place(rpcDet);
      //bool hitCompatible = (fabs(pullX) < 3.5 || fabs(distX<10.))  && fabs(pullY) < 3.5 ;
      bool hitCompatible = (fabs(distX<10.))  && fabs(pullY) < 3.5 ;
//std::cout <<" DISTX: "<<distX<<" DISTY: "<< distY<<" PULLX: "<<pullX<<" pullY: "<<pullY<<std::endl; 
//if (hitCompatible) std::cout <<" HIT COMPATIBLE" << std::endl;
      if (place.isBarrel()) {
        if (hitCompatible) hitBarrel[place.layer()-1]=true;
        hPullX_B[place.layer()-1]->Fill(pullX);
        hDistX_B[place.layer()-1]->Fill(distX);
      } else {
        if(hitCompatible) hitEndcap[place.layer()-1]=true;
        hPullX_E[place.layer()-1]->Fill(pullX);
        hDistX_E[place.layer()-1]->Fill(distX);
      }
      hPullY->Fill(pullY);
    }

    //
    //rpc dets compatible with muon
    // 
    edm::ESHandle<RPCGeometry> rpcGeometry;
    es.get<MuonGeometryRecord>().get(rpcGeometry);
    const TrackingGeometry::DetIdContainer & detIds = rpcGeometry->detIds();
    if ( muon->pt() >7. ) {
    for (TrackingGeometry::DetIdContainer::const_iterator it = detIds.begin(); it != detIds.end(); ++it) {

      const GeomDet * det = rpcGeometry->idToDet(*it);
      GlobalPoint detPosition = det->position();
      if (deltaR(muon->eta(), muon->phi(), detPosition.eta(), detPosition.phi()) > 0.7) continue;

      RPCDetId rpcDet(*it);
      TrajectoryStateOnSurface trackAtDet= trackAtSurface(theMuon, rpcDet, ev, es);
      if (!trackAtDet.isValid()) continue;
      if (! (det->surface().bounds().inside(trackAtDet.localPosition()))) continue;
      hPropToDetDeltaR->Fill(deltaR(muon->eta(), muon->phi(), detPosition.eta(), detPosition.phi()));
      BarrelAndLayer place(rpcDet);
      if (place.isBarrel()) detBarrel[place.layer()-1]++; else detEndcap[place.layer()-1]++;
    }
    }
  }

  //create L1muon helper 
  edm::InputTag l1Tag(theConfig.getParameter<edm::InputTag>("l1MuReadout"));
  L1ObjMaker l1(l1Tag,ev);

  // set L1Others
  std::vector<L1Obj> l1Others=  l1(L1ObjMaker::DT,L1ObjMaker::CSC);
  std::vector<bool> l1OthersMatching(l1Others.size(),false);

  //
  // among tracks find the highest pt compatible with any DT/CSC trigger
  //
  edm::Handle<reco::TrackCollection> tracks;
  ev.getByLabel( theConfig.getParameter<std::string>("trackColl"), tracks);
  typedef reco::TrackCollection::const_iterator IT;
  for (IT it = tracks->begin(); it !=tracks->end(); ++it) {
    const reco::Track & muTrack = *it;
    if (muTrack.pt() < theConfig.getParameter<double>("minPt")) continue;
    if (muTrack.dxy(reference) >  theConfig.getParameter<double>("maxTIP")) continue;
    if (fabs(muTrack.eta()) >  theConfig.getParameter<double>("maxEta")) continue;
    if (muTrack.pt() < track->pt()) continue;
    TrajectoryStateOnSurface tTSOS = TrajectoryStateTransform().outerStateOnSurface(muTrack, *globalGeometry, magField.product());
    TrackToL1ObjMatcher matcher(theConfig.getParameter<edm::ParameterSet>("dtcscMatcherPSet"));

    bool isTrackCompatible = false;
    std::vector<bool> l1OthersThisTrackMatching(l1Others.size(),false);
    for (unsigned int i=0; i< l1Others.size(); ++i) {
      if ( matcher(l1Others[i].eta, l1Others[i].phi, tTSOS, ev,es) ) {
        isTrackCompatible = true; 
        l1OthersThisTrackMatching[i]=true;
      }
    }
    if (isTrackCompatible) {
      if (!theMuon) tsos = tTSOS;
      track->setKine( muTrack.pt(), muTrack.eta(), muTrack.phi(), muTrack.charge());
      l1OthersMatching = l1OthersThisTrackMatching;
    }
  }
  l1OtherColl->set(l1Others);
  l1OtherColl->set(l1OthersMatching);

  //
  // set L1 RPC. Check if it is compatible with theMuon (or if no muon found) theTrack 
  //
  std::vector<L1Obj> l1Rpcs = l1(L1ObjMaker::RPCB,L1ObjMaker::RPCF);
  std::vector<bool> l1RpcsMatching(l1Rpcs.size(), false);
  TrackToL1ObjMatcher matcher(theConfig.getParameter<edm::ParameterSet>("rpcMatcherPSet"));
  double minDeltaR = 100.;
  if (tsos.isValid()) for (unsigned int i=0; i< l1Rpcs.size(); ++i) {
     if (matcher(l1Rpcs[i].eta, l1Rpcs[i].phi, tsos, ev,es)) l1RpcsMatching[i]=true;
     double dR = sqrt( sqr(matcher.lastResult().deltaEta) + sqr(matcher.lastResult().deltaPhi)); 
     if (dR < minDeltaR) minDeltaR = dR;
  }
  l1RpcColl->set(l1Rpcs);
  l1RpcColl->set(l1RpcsMatching);
  if (theMuon)  hDeltaR_Mu->Fill(minDeltaR); else   hDeltaR_Tk->Fill(minDeltaR); 


  // fill ntuple and cleanup
  if (tsos.isValid() || l1Rpcs.size() != 0 ||  l1Others.size() !=0) theTree->Fill();
  delete event; event = 0;
  delete muon;  muon = 0;
  delete track; track = 0;
  delete l1RpcColl; l1RpcColl = 0;
  delete l1OtherColl; l1OtherColl = 0;

}
