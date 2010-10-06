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

#include "DataFormats/GeometrySurface/interface/BoundCylinder.h"
#include "DataFormats/GeometrySurface/interface/SimpleCylinderBounds.h"
#include "DataFormats/GeometrySurface/interface/BoundDisk.h"
#include "DataFormats/GeometrySurface/interface/SimpleDiskBounds.h"


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
#include "DataFormats/TrackReco/interface/HitPattern.h"

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


TrajectoryStateOnSurface EfficiencyTree::trackAtSurface(const reco::Muon* mu, const GlobalPoint& point, const edm::Event &ev, const edm::EventSetup &es) const
{

  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);

  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);

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

  TrajectoryStateOnSurface muTSOS  = muTrajectory.empty() ?
         TrajectoryStateTransform().outerStateOnSurface(*(mu->track()), *globalGeometry, &*magField)
      :  muTrajectory.closestMeasurement(point).updatedState();


  bool barrel = fabs(point.z()) < 700. ? true : false;
  ReferenceCountingPointer<Surface> surface = barrel ?
      ReferenceCountingPointer<Surface>( new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( point.perp(),  point.perp(), -700., 700. ) ))
    : ReferenceCountingPointer<Surface>( new  BoundDisk( GlobalPoint(0.,0.,point.z()), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
       
  bool along = true;
  if (barrel) {
    if (muTSOS.globalPosition().perp() > point.perp()) along = false;
  } else {
    if ( fabs(muTSOS.globalPosition().z()) > fabs(point.z()) ) along = false;
  }
  std::string propagatorName = along? "SteppingHelixPropagatorAlong" : "SteppingHelixPropagatorOpposite";

  es.get<TrackingComponentsRecord>().get(propagatorName, propagator);
  TrajectoryStateOnSurface result =  propagator->propagate(muTSOS, *surface);
  
  return result;
}

TrajectoryStateOnSurface EfficiencyTree::trackAtSurface(const reco::Muon* mu, const RPCDetId& rpc, const edm::Event &ev, const edm::EventSetup &es) const
{

  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);
  edm::ESHandle<Propagator> propagator;

  const GeomDet * det = globalGeometry->idToDet(rpc);
  TrajectoryStateOnSurface muTSOS = TrajectoryStateTransform().outerStateOnSurface(*(mu->track()), *globalGeometry, &*magField);
  es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAlong",propagator);
  TrajectoryStateOnSurface result =  propagator->propagate(muTSOS, det->surface());

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

  theTree->Branch("hitBarrelHP",&hitBarrelHP);
  theTree->Branch("hitEndcapHP",&hitEndcapHP);
  theTree->Branch("detBarrelHP",&detBarrelHP);
  theTree->Branch("detEndcapHP",&detEndcapHP);

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

  hitBarrelHP = std::vector<bool>(6,false);
  hitEndcapHP = std::vector<bool>(3,false);
  detBarrelHP = std::vector<unsigned int>(6,0);
  detEndcapHP = std::vector<unsigned int>(3,0);

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
//    if (im->track()->charge()!=-1) continue;
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

    if (nTrackerHits<=0) continue;
    if ( nRPCHits==0 &&nDTHits==0 &&nCSCHits==0  ) continue;
static int nHits = 0;
    nHits += nRPCHits;
    std::cout <<" RPC HITS: " << nHits << std:: endl;
    if (!im->globalTrack().isNonnull()) continue;

    if (!theMuon || (im->track()->pt() > theMuon->track()->pt()) ) theMuon = &(*im); 
    
  }

static int nMuons = 0;
if (theMuon) std::cout <<"Muons: "<< ++nMuons << std::endl;

   // best RPC muon
int compHit = 0;
  TrajectoryStateOnSurface tsos;
  if (theMuon) { 
    if(theMuon->globalTrack().isNonnull()) std::cout <<"global muon (pt,eta,phi) "
                                                     <<theMuon->globalTrack()->pt()<<" "
                                                     <<theMuon->globalTrack()->eta()<<" "
                                                     <<theMuon->globalTrack()->phi()
                                                     <<" number of RPC hits: " <<theMuon->globalTrack()->hitPattern().numberOfValidMuonRPCHits() 
                                                     <<std::endl;
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
    
//int compDet = 0;
    typedef RPCRecHitCollection::const_iterator IH;
    for (IH ih=recHits->begin(); ih != recHits->end(); ++ih) {
static int nRPCRecHits = 0;
   std::cout <<" nRPCRecHits: "<< ++nRPCRecHits << std::endl;

      RPCDetId rpcDet = ih->rpcId();
//     const GeomDet * det = globalGeometry->idToDet(rpcDet);
      GlobalPoint detPosition = globalGeometry->idToDet(rpcDet)->position();
      GlobalPoint hitPosition = globalGeometry->idToDet(rpcDet)->toGlobal(ih->localPosition());
if (fabs(reco::deltaPhi(detPosition.phi(),theMuon->phi())) < M_PI/2.) { 
  std::cout <<" CHECKING HIT, det "<<rpcDet.rawId()<<"  r="<<detPosition.perp()<<" phi="<<detPosition.phi()<<" z="<< detPosition.z()
                                   <<"Barrel: "<<BarrelAndLayer(rpcDet).isBarrel()<<" layer: "<<BarrelAndLayer(rpcDet).layer();
            
}
      
//std::cout <<" position r: " << detPosition.perp()<<" phi"<<detPosition.phi()<<" z:"<<detPosition.z()<<" delta: " << 
//reco::deltaPhi(detPosition.phi(), theMuon->momentum().phi())<< std::endl;
//const GeomDet * det = globalGeometry->idToDet(rpcDet);
//GlobalPoint detPosition = det->position();
//std::cout <<"checking for hit in Det: r= "<<detPosition.perp()<<" phi="<<detPosition.phi()<<" z="<< detPosition.z()<<std::endl;
//std::cout <<"Barrel: "<<BarrelAndLayer(rpcDet).isBarrel()<<" layer: "<<BarrelAndLayer(rpcDet).layer()<<std::endl;
//      TrajectoryStateOnSurface trackAtHit= trackAtSurface(theMuon, rpcDet, ev, es);
      TrajectoryStateOnSurface trackAtHit= trackAtSurface(theMuon, hitPosition, ev, es);
//if (fabs(reco::deltaPhi(detPosition.phi(),theMuon->phi())) < M_PI/2.) {
   if(!trackAtHit.isValid()) std::cout <<" TRAJ NOT VALID! "<< std::endl;
//}
      if (!trackAtHit.isValid()) continue;

/*
      LocalPoint HitPoint = ih->localPosition();
      LocalError hitError = ih->localPositionError();
      LocalPoint trackAtHitPoint = trackAtHit.localPosition() ;
      LocalError trackAtHitError = trackAtHit.localError().positionError();
      float distX = HitPoint.x()-trackAtHitPoint.x();
      float distY = HitPoint.y()-trackAtHitPoint.y();
*/
      GlobalPoint hitPoint = globalGeometry->idToDet(rpcDet)->toGlobal(ih->localPosition());
      LocalError hitError = ih->localPositionError();
      GlobalPoint trackAtHitPoint = trackAtHit.globalPosition();
      LocalError trackAtHitError = trackAtHit.localError().positionError();
 //     float distX = HitPoint.x()-trackAtHitPoint.x();
 //     float distY = HitPoint.y()-trackAtHitPoint.y();
      float distX = hitPoint.perp()*deltaPhi((float) hitPoint.phi(), (float) trackAtHitPoint.phi());
      float distY = BarrelAndLayer(rpcDet).isBarrel() ? hitPoint.z()-trackAtHitPoint.z() : hitPoint.perp()-trackAtHitPoint.perp() ;
      float pullX = distX/ sqrt( trackAtHitError.xx()+hitError.xx());
      float pullY = distY/ sqrt( trackAtHitError.yy()+hitError.yy());

      //bool hitCompatible = det->surface().bounds().inside(trackAtHit.localPosition());
      //bool hitCompatible = fabs(pullX) < 3.5 && fabs(pullY) < 3.5 ;
// std::cout <<"HIT  compatible!, det is: " << rpcDet.rawId() << std::endl;
 compHit++;
//      bool hitCompatible = (fabs(pullX) < 3.5 || fabs(distX<10.))  && fabs(pullY) < 3.5 ;
      bool hitCompatible = true;
      //bool hitCompatible = (fabs(distX<10.))  && fabs(pullY) < 3.5 ;
//std::cout <<" DISTX: "<<distX<<" DISTY: "<< distY<<" PULLX: "<<pullX<<" pullY: "<<pullY<<std::endl; 
//if (hitCompatible) std::cout <<" HIT COMPATIBLE" << std::endl;
      BarrelAndLayer place(rpcDet);
      if (hitCompatible) std::cout <<" COMPATIBLE HIT "
<<" in: "<<globalGeometry->idToDet(rpcDet)->surface().bounds().inside(trackAtHit.localPosition())
<<" is BARREL: " << place.isBarrel() <<" layer: " << place.layer(); 
      std::cout << std::endl; 
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
      RPCDetId rpcDet(*it);
//std::cout << std::endl<<"checking Det: " << rpcDet.rawId();
//    std::cout <<" position r: " << detPosition.perp()<<" phi"<<detPosition.phi()<<" z:"<<detPosition.z()<<" delta: " << 
//    reco::deltaPhi(detPosition.phi(), theMuon->momentum().phi()) << std::endl;
      if (deltaR(muon->eta(), muon->phi(), detPosition.eta(), detPosition.phi()) > 0.7) continue;
//      std::cout <<" CHECKING DET, id:"<<rpcDet.rawId()<<std::endl;
      TrajectoryStateOnSurface trackAtDet= trackAtSurface(theMuon, rpcDet, ev, es);
      if (!trackAtDet.isValid()) continue;
      if (! (det->surface().bounds().inside(trackAtDet.localPosition()))) continue;
      hPropToDetDeltaR->Fill(deltaR(muon->eta(), muon->phi(), detPosition.eta(), detPosition.phi()));
      BarrelAndLayer place(rpcDet);
      std::cout <<" COMPATIBLE DET, isbarrel: " << place.isBarrel() <<" layrer: " << place.layer() << std::endl;
      if (place.isBarrel()) detBarrel[place.layer()-1]++; else detEndcap[place.layer()-1]++;
    }
    }
  }

  //
  //additional way of matching hits and dets from HitPattern;
  //
  std::cout <<"HIT PATTERN"<< std::endl;
  if (theMuon && theMuon->isGlobalMuon()) {
    const reco::HitPattern& hp = theMuon->globalTrack()->hitPattern(); 
    for(int i=0; i < hp.numberOfHits(); i++) {
      uint32_t hit = hp.getHitPattern(i);
      if (!hp.validHitFilter(hit)) continue;
      if (hp.muonDTHitFilter(hit)) {
        int layer = -1;
        if (hp.getMuonStation(hit) <=2) {
           if (hp.getDTSuperLayer(hit) ==1) layer=2*(hp.getMuonStation(hit)-1)+1;
           if (hp.getDTSuperLayer(hit) ==3) layer=2*(hp.getMuonStation(hit)-1)+2;
        } else {
           if (hp.getDTSuperLayer(hit) ==1) layer=hp.getMuonStation(hit)+2;
        }
        //  std::cout <<"DT station:"<< hp.getMuonStation(hit)<<" sublayer: " <<hp.getDTSuperLayer(hit)<<" layer: "<<layer<<std::endl;
        if (layer>0 ) detBarrelHP[layer-1]++;
      }
      if (hp.muonCSCHitFilter(hit)) {
        if (hp.getCSCRing(hit) >=2) {
        int layer = hp.getMuonStation(hit);   
        //std::cout <<"CSC tation:"<< hp.getMuonStation(hit)<<" ring "<<hp.getCSCRing(hit)<<" layer: "<<layer<<std::endl;
         if (layer>0 && layer <=3) detEndcapHP[layer-1]++; else if (layer >0) std::cout <<"KUKU" << std::endl;
        }
      }
      if (hp.muonRPCHitFilter(hit)) {
        if (!hp.getRPCregion(hit)) {
          int layer = (hp.getMuonStation(hit) <=2) ?  2*(hp.getMuonStation(hit)-1)+hp.getRPCLayer(hit):  hp.getMuonStation(hit)+2;
          hitBarrelHP[layer-1] = true;
          std::cout <<"HP RPC barrel, layer: " << layer << std::endl;
        } 
        else {
          int layer=hp.getMuonStation(hit);
          hitEndcapHP[layer-1] = true; 
          std::cout <<"HP RPC endcap, layer: "<< layer << std::endl;
        }
      } 
    }
    if (compHit != theMuon->globalTrack()->hitPattern().numberOfValidMuonRPCHits()) std::cout << "PROBLEM" << std::endl;
  }

  for (unsigned int i=0; i<=5;++i) if (!hitBarrel[i] && hitBarrelHP[i]) std::cout <<" HitHP but no Hit, barrel layer: "<<i+1<<std::endl;
  
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
