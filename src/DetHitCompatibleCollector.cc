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
#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"
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
#include "TH2F.h"
#include <sstream>
#include <cmath>

#include "UserCode/L1RpcTriggerAnalysis/interface/TrackAtSurface.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"


DetHitCompatibleCollector::DetHitCompatibleCollector(const edm::ParameterSet& cfg) 
  : theNoDigiWarning(false)
{ }

DetHitCompatibleCollector::~ DetHitCompatibleCollector()
{
  if (theNoDigiWarning) std::cout <<" **** DetHitCompatibleCollector **** WARNING - NoDigiWarning was set!" << std::endl;
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

std::vector<DetCluDigiObj> DetHitCompatibleCollector::compatibleHits( const reco::Muon* muon, const edm::Event &ev, const edm::EventSetup &es)
{
  std::vector<DetCluDigiObj> detsHitsCompatibleWithMuon;

  edm::Handle<RPCRecHitCollection> recHits;
  ev.getByLabel("rpcRecHits", recHits);

  edm::Handle<RPCDigiCollection> rpcDigis;
//  ev.getByLabel("muonRPCDigis", rpcDigis);
  ev.getByLabel("rpcunpacker", rpcDigis);

  edm::ESHandle<RPCGeometry> rpcGeometry;
  es.get<MuonGeometryRecord>().get(rpcGeometry);

  TrackAtSurface trackAtSurface(muon, ev,es);

  std::map< uint32_t, std::pair<uint32_t,uint32_t> > aMap;
  typedef RPCRecHitCollection::const_iterator IH;
  for (IH ih=recHits->begin(); ih != recHits->end(); ++ih) {

    if (ih->BunchX() != 0) continue;
    if (! ih->isValid() ) continue;
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
//    if(!trackAtHit.isValid()) std::cout <<" TRAJ NOT VALID! "<< std::endl;
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
    if (hitCompatible) {
      if (aMap.find(rpcDet.rawId()) == aMap.end() ) aMap[rpcDet.rawId()] = std::make_pair(0,0);

      unsigned int clusterSize = ih->clusterSize();
      if ( aMap[rpcDet.rawId()].first < clusterSize)  aMap[rpcDet.rawId()].first = clusterSize;

      if (rpcDigis.isValid()) {
        const RPCDigiCollection::Range range = rpcDigis->get( rpcDet.rawId() );
        std::map<int, bool> strips;
        for (RPCDigiCollection::const_iterator id = range.first; id != range.second; ++id) if (id->bx() == 0) strips[id->strip()] = true;
        if ( strips.size() == 0 ) std::cout <<"WARNING ***************"<<std::endl;
        aMap[rpcDet.rawId()].second = strips.size(); 
      } else {
        theNoDigiWarning = true;
        std::cout <<"Hit compatible but no valid digis found! Det: "<< ih->rpcId()<<"is valid: "<< ih->isValid()<< "BX= "<<ih->BunchX() << std::endl;
      }
    }

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
      { 
        double ilayer = rpcDet.station() * rpcDet.region();
        double phi    =  (rpcGeometry->idToDet(rpcDet)->toGlobal(ih->localPosition())).phi(); 
        if (hitCompatible) hPlaceComp->Fill(ilayer, phi); else hPlaceDiff->Fill(ilayer, phi);
      }
    }
    hPullX->Fill(pullX);
    hPullY->Fill(pullY);

    if (!place.isBarrel()) {
    }
  }


  for ( std::map< uint32_t, std::pair<uint32_t,uint32_t> >::const_iterator it = aMap.begin(); it != aMap.end(); ++it) {
    DetCluDigiObj obj;
    obj.det         = (*it).first;
    obj.clusterSize = (*it).second.first;
    obj.nDigis      = (*it).second.second;    
    detsHitsCompatibleWithMuon.push_back(obj); 
  }
  return detsHitsCompatibleWithMuon;
}

/*
std::vector<uint32_t> DetHitCompatibleCollector::clSizeCompDets(const std::vector<uint32_t> & detIds, const edm::Event &ev, const edm::EventSetup &es) 
{
  std::vector<uint32_t> cluInDets;
  edm::Handle<RPCRecHitCollection> recHits;
  ev.getByLabel("rpcRecHits", recHits);
  for (std::vector<uint32_t>::const_iterator it = detIds.begin(); it != detIds.end(); ++it) {
    RPCRecHitCollection::range range = recHits->get(*it);
//    std::cout <<"DET: "<<*it << std::endl;
      for(RPCRecHitCollection::const_iterator ih= range.first; ih != range.second; ++ih) {
        if (ih->BunchX() == 0) { cluInDets.push_back(ih->clusterSize()); break; }
      }
//    for(RPCRecHitCollection::const_iterator ih= range.first; ih != range.second; ++ih)  
//        std::cout <<" Hit: cluster:"<<ih->clusterSize() << "first strip: "<<ih->firstClusterStrip()<<std::endl; 
//    if (range.second-range.first) cluInDets.push_back(range.first->clusterSize());
  }
  return cluInDets;
}

std::vector<uint32_t> DetHitCompatibleCollector::nDigisCompDets(const std::vector<uint32_t> & detIds, const edm::Event &ev, const edm::EventSetup &es) 
{
  std::vector<uint32_t> digisInDets;
  edm::Handle<RPCDigiCollection> rpcDigis;
  ev.getByLabel("muonRPCDigis", rpcDigis);
  for (std::vector<uint32_t>::const_iterator it = detIds.begin(); it != detIds.end(); ++it) {
//  std::cout <<"DET: "<<*it << std::endl;
    const RPCDigiCollection::Range range = rpcDigis->get(*it);
    std::map<int, bool> strips;
//  for (RPCDigiCollection::const_iterator id = range.first; id != range.second; ++id) id->print() ;
    for (RPCDigiCollection::const_iterator id = range.first; id != range.second; ++id) if (id->bx() == 0) strips[id->strip()] = true;
    if ( strips.size() == 0 ) std::cout <<"WARNING ***************"<<std::endl;
    digisInDets.push_back( strips.size());
//    std::cout <<"SIZE is : "<<strips.size() << std::endl;
  }
  return digisInDets;
}
*/

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
  for (unsigned int i=1; i<=4; ++i) {
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
  hPlaceComp = new TH2F("hPlaceComp","hPlaceComp",9, -4.5,4.5,  144, -M_PI, M_PI);  histos.Add(hPlaceComp);
  hPlaceDiff = new TH2F("hPlaceDiff","hPlaceDiff",9, -4.5,4.5,  144, -M_PI, M_PI);  histos.Add(hPlaceDiff); 

  hPropToDetDeltaR = new TH1F("hPropToDetDeltaR","DR good Mu-DU before propagation",100,0.,3.);  histos.Add(hPropToDetDeltaR);
}
