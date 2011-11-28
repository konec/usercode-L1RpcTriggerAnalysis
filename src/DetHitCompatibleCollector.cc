#include "UserCode/L1RpcTriggerAnalysis/interface/DetHitCompatibleCollector.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"

//#include "Geometry/RPCGeometry/interface/RPCRoll.h"
//#include "Geometry/RPCGeometry/interface/RPCGeomServ.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"
#include "DataFormats/DetId/interface/DetIdCollection.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"

#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
//#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
//#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
//#include "MagneticField/Engine/interface/MagneticField.h"
//#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "DataFormats/TrajectoryState/interface/LocalTrajectoryParameters.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"

#include "SimDataFormats/CrossingFrame/interface/CrossingFrame.h"
#include "SimDataFormats/CrossingFrame/interface/MixCollection.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"

#include "DataFormats/Math/interface/deltaR.h"

#include "TObjArray.h"
#include "TH1F.h"
#include <sstream>

#include "UserCode/L1RpcTriggerAnalysis/interface/TrackAtSurface.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"


DetHitCompatibleCollector::DetHitCompatibleCollector(const edm::ParameterSet& cfg) 
{
}

std::vector<uint32_t> DetHitCompatibleCollector::compatibleSIMU( const reco::Muon* muon, const edm::Event &ev, const edm::EventSetup &es)
{
  std::vector<uint32_t> detsSIMU;
  edm::Handle< edm::PSimHitContainer > simHits;
  ev.getByLabel("g4SimHits","MuonRPCHits",simHits);
  std::cout <<" SIZE OF SIM HITS IS: "<<simHits->size() << std::endl;
  for (edm::PSimHitContainer::const_iterator hitItr = simHits->begin(); hitItr != simHits->end(); ++hitItr) {
    uint32_t rpcRawId = hitItr->detUnitId();
    if ( !(13 == abs(hitItr->particleType()) ) ) continue;
    if (detsSIMU.end() == find(detsSIMU.begin(),detsSIMU.end(),rpcRawId) ) detsSIMU.push_back(rpcRawId);
  }
  return detsSIMU;
}

std::vector<uint32_t> DetHitCompatibleCollector::compatibleHits( const reco::Muon* muon, const edm::Event &ev, const edm::EventSetup &es)
{
  std::vector<uint32_t> detsHitsCompatibleWithMuon;
  edm::Handle<RPCRecHitCollection> recHits;
  ev.getByLabel("rpcRecHits", recHits);

  edm::ESHandle<RPCGeometry> rpcGeometry;
  es.get<MuonGeometryRecord>().get(rpcGeometry);

  TrackAtSurface trackAtSurface(muon, ev,es);

  typedef RPCRecHitCollection::const_iterator IH;
  for (IH ih=recHits->begin(); ih != recHits->end(); ++ih) {

    RPCDetId rpcDet = ih->rpcId();
    GlobalPoint detPosition = rpcGeometry->idToDet(rpcDet)->position();
    GlobalPoint hitPosition = rpcGeometry->idToDet(rpcDet)->toGlobal(ih->localPosition());
    if (fabs(reco::deltaPhi(detPosition.phi(),muon->phi())) > M_PI/2.) continue;

/*
    TrajectoryStateOnSurface trackAtHit= trackAtSurface.atPoint(hitPosition);
    if(!trackAtHit.isValid()) std::cout <<" TRAJ NOT VALID! "<< std::endl;
    if (!trackAtHit.isValid()) continue;
    GlobalPoint hitPoint = rpcGeometry->idToDet(rpcDet)->toGlobal(ih->localPosition());
    LocalError hitError = ih->localPositionError();
    GlobalPoint trackAtHitPoint = trackAtHit.globalPosition();
    LocalError trackAtHitError = trackAtHit.localError().positionError();
    float distX = hitPoint.perp()*deltaPhi((float) hitPoint.phi(), (float) trackAtHitPoint.phi());
    float distY = RPCDetIdUtil(rpcDet).isBarrel() ? hitPoint.z()-trackAtHitPoint.z() : hitPoint.perp()-trackAtHitPoint.perp() ;
*/
//
    TrajectoryStateOnSurface trackAtHit= trackAtSurface.atDetFromClose(rpcDet,hitPosition);
    if(!trackAtHit.isValid()) std::cout <<" TRAJ NOT VALID! "<< std::endl;
    if (!trackAtHit.isValid()) continue;
    LocalPoint hitPoint = ih->localPosition();
    LocalError hitError = ih->localPositionError();
    LocalPoint trackAtHitPoint = trackAtHit.localPosition() ;
    LocalError trackAtHitError = trackAtHit.localError().positionError();
    float distX = hitPoint.x()-trackAtHitPoint.x();
    float distY = hitPoint.y()-trackAtHitPoint.y();
//

    float pullX = distX/ sqrt( trackAtHitError.xx()+hitError.xx());
    float pullY = distY/ sqrt( trackAtHitError.yy()+hitError.yy());

    bool hitCompatible = (fabs(pullX) < 3.5 || fabs(distX) < 10.)  && fabs(pullY) < 3.5 ;
    if (hitCompatible && (detsHitsCompatibleWithMuon.end() == find(detsHitsCompatibleWithMuon.begin(),detsHitsCompatibleWithMuon.end(),rpcDet.rawId()) ) )
        detsHitsCompatibleWithMuon.push_back(rpcDet.rawId());

    RPCDetIdUtil place(rpcDet);
    if (place.isBarrel()) {
      hPullX_B[place.layer()-1]->Fill(pullX);
      hDistX_B[place.layer()-1]->Fill(distX);
      hPullY_B[place.layer()-1]->Fill(pullY);
      hDistY_B[place.layer()-1]->Fill(distY);
    } else {
      hPullX_E[place.layer()-1]->Fill(pullX);
      hDistX_E[place.layer()-1]->Fill(distX);
      hPullY_E[place.layer()-1]->Fill(pullY);
      hDistY_E[place.layer()-1]->Fill(distY);
    }
    hPullX->Fill(pullX);
    hPullY->Fill(pullY);
  }

  return detsHitsCompatibleWithMuon;
}

std::vector<uint32_t> DetHitCompatibleCollector::compatibleDets( const reco::Muon* muon, const edm::Event &ev, const edm::EventSetup &es, bool deepInside)
{
  std::vector<uint32_t> detsCrossedByMuon;
  TrackAtSurface trackAtSurface(muon, ev,es);
  edm::ESHandle<RPCGeometry> rpcGeometry;
  es.get<MuonGeometryRecord>().get(rpcGeometry);
  const TrackingGeometry::DetIdContainer & detIds = rpcGeometry->detIds();
  for (TrackingGeometry::DetIdContainer::const_iterator it = detIds.begin(); it != detIds.end(); ++it) {

    const GeomDet * det = rpcGeometry->idToDet(*it);
    GlobalPoint detPosition = det->position();
    RPCDetId rpcDet(*it);
    if (rpcDet.roll()==0) continue;
    if (deltaR(muon->eta(), muon->phi(), detPosition.eta(), detPosition.phi()) > 0.5) continue;

//    TrajectoryStateOnSurface stateAtDet = deepInside ?  trackAtSurface.atDetFromClose(rpcDet,detPosition) : trackAtSurface.atDetFromTrack(rpcDet);
    TrajectoryStateOnSurface stateAtDet = trackAtSurface.atDetFromClose(rpcDet,detPosition);
// : trackAtSurface.atDetFromTrack(rpcDet);
    if (!stateAtDet.isValid()) continue;
//  std::cout <<" det:width="<< det->surface().bounds().width()/2.<<" det:lengths="<<det->surface().bounds().length()/2.<<std::endl;
    if (! (det->surface().bounds().inside(stateAtDet.localPosition()))) continue;
    if (deepInside) {
      LocalError stateAtDetError = stateAtDet.localError().positionError();
      LocalPoint farEdge( fabs(stateAtDet.localPosition().x())+3.5*sqrt(stateAtDetError.xx())+5.,  fabs(stateAtDet.localPosition().y())+3.5*sqrt(stateAtDetError.yy())+5.); 
      if (! det->surface().bounds().inside(farEdge) ) continue;
    }
    if (detsCrossedByMuon.end() == find(detsCrossedByMuon.begin(),detsCrossedByMuon.end(),rpcDet.rawId()) )
            detsCrossedByMuon.push_back(rpcDet.rawId());

    hPropToDetDeltaR->Fill(deltaR(muon->eta(), muon->phi(), detPosition.eta(), detPosition.phi()));
  }
  return detsCrossedByMuon;
}


void DetHitCompatibleCollector::initHistos( TObjArray & histos)
{
  for (unsigned int i=1; i<=6; ++i) {
    std::stringstream namePullX_B; namePullX_B <<"hPullX_B"<< i;
    std::stringstream nameDistX_B; nameDistX_B <<"hDistX_B"<< i;
    hPullX_B[i-1]= new TH1F(namePullX_B.str().c_str(),namePullX_B.str().c_str(),50,-5.,5.);  histos.Add( hPullX_B[i-1]);
    hDistX_B[i-1]= new TH1F(nameDistX_B.str().c_str(),nameDistX_B.str().c_str(),100,-20.,20.);  histos.Add( hDistX_B[i-1]);
    std::stringstream namePullY_B; namePullY_B <<"hPullY_B"<< i;
    std::stringstream nameDistY_B; nameDistY_B <<"hDistY_B"<< i;
    hPullY_B[i-1]= new TH1F(namePullY_B.str().c_str(),namePullY_B.str().c_str(),50,-5.,5.);  histos.Add( hPullY_B[i-1]);
    hDistY_B[i-1]= new TH1F(nameDistY_B.str().c_str(),nameDistY_B.str().c_str(),100,-100.,100.);  histos.Add( hDistY_B[i-1]);
  }
  for (unsigned int i=1; i<=3; ++i) {
    std::stringstream namePullX_E; namePullX_E <<"hPullX_E" << i;
    std::stringstream nameDistX_E; nameDistX_E <<"hDistX_E"<< i;
    hPullX_E[i-1]= new TH1F(namePullX_E.str().c_str(),namePullX_E.str().c_str(),50,-5.,5.);  histos.Add( hPullX_E[i-1]);
    hDistX_E[i-1]= new TH1F(nameDistX_E.str().c_str(),nameDistX_E.str().c_str(),100,-20.,20.);  histos.Add( hDistX_E[i-1]);
    std::stringstream namePullY_E; namePullY_E <<"hPullY_E" << i;
    std::stringstream nameDistY_E; nameDistY_E <<"hDistY_E"<< i;
    hPullY_E[i-1]= new TH1F(namePullY_E.str().c_str(),namePullY_E.str().c_str(),50,-5.,5.);  histos.Add( hPullY_E[i-1]);
    hDistY_E[i-1]= new TH1F(nameDistY_E.str().c_str(),nameDistY_E.str().c_str(),100,-100.,100.);  histos.Add( hDistY_E[i-1]);
  }
  hPullX = new TH1F("hPullX","hPullX",50,-5.,5.);  histos.Add( hPullX);
  hPullY = new TH1F("hPullY","hPullY",50,-5.,5.);  histos.Add( hPullY);

  hPropToDetDeltaR = new TH1F("hPropToDetDeltaR","DR good Mu-DU before propagation",100,0.,3.);  histos.Add(hPropToDetDeltaR);
}
