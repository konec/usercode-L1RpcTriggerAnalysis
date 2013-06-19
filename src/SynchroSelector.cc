#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroSelector.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"


#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"

#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"

#include "TH1.h"
#include <cmath>
using namespace edm;
using namespace std;

SynchroSelector::SynchroSelector(const edm::ParameterSet & cfg) 
  : theConfig(cfg), hPullX(0),hDistX(0)
{ }

bool SynchroSelector::checkRpcDetMatching( const TrajectoryStateOnSurface & tsos,  const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es)
{
  edm::ESHandle<Propagator> propagator;
  es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAlong", propagator);

  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);

  GlobalPoint detPos, trackPos;
//  std::cout <<"BEFORE PROPAGATION POSITION:"<< tsos.globalPosition().perp()<<"  GLOBAL MOMENTUM:"<<tsos.globalMomentum().mag()<< std::endl;
  TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(tsos, globalGeometry->idToDet(det)->surface());
  if (!trackAtRPC.isValid()) return false;
//  std::cout <<"AFTER PROPAGATION POSITION:"<< trackAtRPC.globalPosition().perp()<<"  GLOBAL MOMENTUM:"<<trackAtRPC.globalMomentum().mag()<< std::endl;
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
  float propQuality = 0.; 
  if (     trackAtRPC.localError().positionError().xx() > 2500 || trackAtRPC.localError().positionError().yy() > 2500) return false;
  else if (trackAtRPC.localError().positionError().xx() > 1000 || trackAtRPC.localError().positionError().yy() > 1000)  propQuality = 0;
  else if (trackAtRPC.localError().positionError().xx() > 500  || trackAtRPC.localError().positionError().yy() > 500)   propQuality = 1;
  else if (trackAtRPC.localError().positionError().xx() > 100  || trackAtRPC.localError().positionError().yy() > 100)   propQuality = 2;
  else propQuality = 3.;

  if (propQuality < theConfig.getParameter<int>("checkRpcDetMatching_minPropagationQuality") ) return false;
  double scale = theConfig.getParameter<bool>("checkRpcDetMatching_matchingScaleAuto") ? propQuality : theConfig.getParameter<double>("checkRpcDetMatching_matchingScaleValue") ;
   
  bool inside = globalGeometry->idToDet(det)->surface().bounds().inside( trackAtRPC.localPosition(), trackAtRPC.localError().positionError(), scale);
  //std::cout<<"In:"<<inside <<" detector r:" << detPos.perp()<<" phi:"<<detPos.phi()<<" z:"<<detPos.z() 
  // <<" track r:"<< trackPos.perp()<<" phi:"<<trackPos.phi()<<" z:"<<trackPos.z() <<"Error: "<<trackAtRPC.localError().positionError()<< std::endl; 
  return inside;
}

bool SynchroSelector::checkUniqueRecHitMatching( const TrajectoryStateOnSurface & tsos,  const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es)
{
  //propagate to det
  edm::ESHandle<Propagator> propagator;
  es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAny", propagator);
  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(tsos, globalGeometry->idToDet(det)->surface());
  if (!trackAtRPC.isValid()) return false;
  
  LocalPoint trackAtRPCPoint = trackAtRPC.localPosition() ;
  LocalError trackAtRPCError = trackAtRPC.localError().positionError();

  edm::Handle<RPCRecHitCollection> rpcHits;
  std::vector<edm::Handle<RPCRecHitCollection> > handle_vec;
  ev.getManyByType(handle_vec);
  if(handle_vec.size()) rpcHits = handle_vec[0];
  bool matched = false;
  bool unique = true;
  pair<RPCRecHitCollection::const_iterator, RPCRecHitCollection::const_iterator> recHitsInDet =  rpcHits->get(det);
  for (RPCRecHitCollection::const_iterator ih = recHitsInDet.first; ih != recHitsInDet.second; ++ih) {
    LocalPoint hitPoint = ih->localPosition();
    LocalError hitError = ih->localPositionError();
    float distX = hitPoint.x()-trackAtRPCPoint.x();
    float pullX = distX/ sqrt( trackAtRPCError.xx()+hitError.xx()); 

    if (    fabs(pullX) < theConfig.getParameter<double>("checkUniqueRecHitMatching_maxPull") 
         && fabs(distX) < theConfig.getParameter<double>("checkUniqueRecHitMatching_maxDist") ) {
      matched = true;
    } else {
      unique = false;
    } 
    if (hPullX) hPullX->Fill(pullX);
    if (hDistX) hDistX->Fill(distX);
  }
  
  return (matched && unique);
}
