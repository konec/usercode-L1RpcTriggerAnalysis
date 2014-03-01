#include "UserCode/L1RpcTriggerAnalysis/interface/DetHitDigiGrabber.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"

////AK
#include "MagneticField/Engine/interface/MagneticField.h"
#include "TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/GeomPropagators/interface/StraightLineBarrelCylinderCrossing.h"
#include "TrackingTools/GeomPropagators/interface/HelixBarrelCylinderCrossing.h"
#include "TrackingTools/GeomPropagators/interface/HelixForwardPlaneCrossing.h"
#include "DataFormats/TrajectoryState/interface/TrackCharge.h"

#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "DataFormats/GeometrySurface/interface/BoundCylinder.h"
#include "DataFormats/GeometrySurface/interface/SimpleCylinderBounds.h"
#include "DataFormats/GeometrySurface/interface/BoundDisk.h"
#include "DataFormats/GeometrySurface/interface/SimpleDiskBounds.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
//////

#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"


#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"
#include "DataFormats/DetId/interface/DetIdCollection.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"

#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
#include "DataFormats/MuonDetId/interface/CSCTriggerNumbering.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/L1CSCTrackFinder/interface/CSCTriggerContainer.h"

#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambPhContainer.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambThContainer.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambPhDigi.h"
#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambThDigi.h"

#include "DataFormats/Math/interface/deltaPhi.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include <sstream>
#include <bitset>

namespace {
  TH2D* hDetHitDigiGrabber_DphiVsPt_Bar;
  TH2D* hDetHitDigiGrabber_DphiVsPt_End;
}

DetHitDigiGrabber::DetHitDigiGrabber(const edm::ParameterSet& cfg)
  : theConfig(cfg) { }

DetHitDigiGrabber::~DetHitDigiGrabber() {}

HitSpecObj DetHitDigiGrabber::rpcDetHits(const edm::Event &ev, 
					 const edm::EventSetup &es, 
					 const TrackObj * simu,
					 int station) const
{
  HitSpecObj result;
  std::vector<uint32_t> simuHits;
  edm::Handle< edm::PSimHitContainer > simHits;
  static std::string rpcHitsCollName = theConfig.getParameter<std::string>("rpcHitsCollName");
  ev.getByLabel("g4SimHits", rpcHitsCollName, simHits);
  static bool warnNoColl = theConfig.getUntrackedParameter<bool>("warnNoColl" , true);
  if (!simHits.isValid()) {
    if (warnNoColl) std::cout <<"** WARNING: not collection: "<< rpcHitsCollName << std::endl;
    return result;
  }

  edm::ESHandle<RPCGeometry> rpcGeometry;
  es.get<MuonGeometryRecord>().get(rpcGeometry);

  GlobalPoint hitPoint;
  GlobalVector glbMom;

  for (edm::PSimHitContainer::const_iterator hitItr = simHits->begin(); hitItr != simHits->end(); ++hitItr) {
    if ( !(13 == abs(hitItr->particleType()) ) ) continue;
    uint32_t rpcRawId = hitItr->detUnitId();
    RPCDetId rpcDet(rpcRawId);
    RPCDetIdUtil  u(rpcDet);
    
    if((station==2 && ((u.isBarrel() && u.barrelLayer()==4) || (!u.isBarrel() && u.endcapLayer()==2))) ||
       (station==1 && u.isBarrel() && u.barrelLayer()==1)){

      ///Do not use endcap roll 1 in trigger tower 7
      if(rpcDet.region()!=0 && rpcDet.station()==2 && rpcDet.roll()==1) continue;

      hitPoint = rpcGeometry->idToDet(rpcDet)->toGlobal(hitItr->entryPoint());
      glbMom = rpcGeometry->idToDet(rpcDet)->toGlobal(hitItr->momentumAtEntry());

      if (u.isBarrel() || result.rawId()==0) result = HitSpecObj(rpcRawId, hitPoint, glbMom);

      TH2D* h = u.isBarrel() ?  hDetHitDigiGrabber_DphiVsPt_Bar : hDetHitDigiGrabber_DphiVsPt_End;
      h->Fill( simu->pt(), deltaPhi(simu->phi(),hitPoint.phi())*simu->charge());
      /*
      std::cout <<"Det: "<< rpcRawId 
		<<rpcDet
		<<" glbMom: pt:" << glbMom.perp() <<" pz: "<<glbMom.z()<<" phi: "<<glbMom.phi() 
		<<" globalPosition: r:"<<hitPoint.perp()<<" z:"<<hitPoint.z()<<" phi: "<<hitPoint.phi()
		<<std::endl;
      */
    }
  }
  ///In case no hit in reference station, find SimHit closest to reference plane
  if(station==99){
    edm::ESHandle<MagneticField> magField;
    es.get<IdealMagneticFieldRecord>().get(magField);
    
    if(hitPoint.perp()<1){

      GlobalPoint hitPointTmp;
      GlobalVector glbMomTmp;
      float minDist=999;
      
      BoundCylinder *aRefCylinder = new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 530, 530, -700, 700));
      BoundDisk *aRefDisk = new  BoundDisk( GlobalPoint(0.,0.,790), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10.));
      
      for (edm::PSimHitContainer::const_iterator hitItr = simHits->begin(); hitItr != simHits->end(); ++hitItr) {
	if ( !(13 == abs(hitItr->particleType()) ) ) continue;
	uint32_t rpcRawId = hitItr->detUnitId();
	RPCDetId rpcDet(rpcRawId);
	hitPointTmp = rpcGeometry->idToDet(rpcDet)->toGlobal(hitItr->entryPoint());
	glbMomTmp = rpcGeometry->idToDet(rpcDet)->toGlobal(hitItr->momentumAtEntry());
	
	GlobalTrajectoryParameters ngtp=GlobalTrajectoryParameters (hitPointTmp,glbMomTmp, 
								    simu->charge(),
								    magField.product());
	
	HelixBarrelCylinderCrossing aBarrelCrossing(hitPointTmp,glbMomTmp,ngtp.transverseCurvature(),  
						    PropagationDirection::anyDirection,
						    *aRefCylinder);

	HelixForwardPlaneCrossing aPlaneCrossing(HelixForwardPlaneCrossing::PositionType(hitPointTmp.x(), hitPointTmp.y(),hitPointTmp.z()),
						 HelixForwardPlaneCrossing::PositionType(glbMomTmp.x(), glbMomTmp.y(), glbMomTmp.z()),
						 ngtp.transverseCurvature(),  
						 PropagationDirection::anyDirection);

	if(aBarrelCrossing.hasSolution() && fabs(aBarrelCrossing.pathLength())<minDist){
	  minDist = fabs(aBarrelCrossing.pathLength());
	  hitPoint = hitPointTmp;
	  glbMom = glbMom;
	}

	std::pair<bool,double> aCrossingRes = aPlaneCrossing.pathLength(*aRefDisk);

	if(aCrossingRes.first && 
	   fabs(aCrossingRes.second)<minDist){
	  minDist = fabs(aCrossingRes.second);
	  hitPoint = hitPointTmp;
	  glbMom = glbMom;
	}
      }
    }
    ////Propagete closest hit to reference cylinder
    float eta = simu->eta();
    GlobalVector glbMom(simu->pt()*cos(simu->phi()),
			simu->pt()*sin(simu->phi()),
			simu->pt()*sinh(eta));
    
    const FreeTrajectoryState fts = FreeTrajectoryState(hitPoint,glbMom,TrackCharge(simu->charge()),magField.product());
    
    ReferenceCountingPointer<Surface> rpc;
    if (eta < -1.04)       rpc = ReferenceCountingPointer<Surface>(new  BoundDisk( GlobalPoint(0.,0.,-790), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
    else if (eta < -0.72)  rpc = ReferenceCountingPointer<Surface>(new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 530, 530, -700, 700 ) ) );
    else if (eta < 0.72)   rpc = ReferenceCountingPointer<Surface>(new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 500, 500, -700, 700 ) ) );
    else if (eta < 1.04)   rpc = ReferenceCountingPointer<Surface>(new  BoundCylinder( GlobalPoint(0.,0.,0.), TkRotation<float>(), SimpleCylinderBounds( 530, 530, -700, 700 ) ) );
    else                      rpc = ReferenceCountingPointer<Surface>(new  BoundDisk( GlobalPoint(0.,0.,790), TkRotation<float>(), SimpleDiskBounds( 300., 710., -10., 10. ) ) );
    
    edm::ESHandle<Propagator> propagator;
    es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAny", propagator);
    TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(fts, *rpc);
    if(trackAtRPC.isValid()) result = HitSpecObj(1, trackAtRPC.globalPosition(), glbMom);
  }
  //////////////
  return result;
}

std::vector< std::pair<uint32_t,uint32_t> > DetHitDigiGrabber::digiCollector(const edm::Event &ev, 
									     const edm::EventSetup &es, 
									     bool filter) const
{
  std::vector< std::pair<uint32_t,uint32_t> > result;

  edm::Handle< edm::PSimHitContainer > simRPCHits;
  static std::string rpcHitsCollName = theConfig.getParameter<std::string>("rpcHitsCollName");
  ev.getByLabel("g4SimHits", rpcHitsCollName, simRPCHits);

  edm::Handle< edm::PSimHitContainer > simDTHits;
  static std::string dtHitsCollName = theConfig.getParameter<std::string>("dtHitsCollName");
  ev.getByLabel("g4SimHits", dtHitsCollName, simDTHits);

  edm::Handle< edm::PSimHitContainer > simCSCHits;
  static std::string cscHitsCollName = theConfig.getParameter<std::string>("cscHitsCollName");
  ev.getByLabel("g4SimHits", cscHitsCollName, simCSCHits);

  std::vector<RPCDigiSpec> rpc = rpcDetDigis(ev,es);
  for (std::vector<RPCDigiSpec>::const_iterator it = rpc.begin(); it!= rpc.end(); ++it) {
    //////////Filter noise digis
    bool hasMuonHit = false;
    for (edm::PSimHitContainer::const_iterator hitItr = simRPCHits->begin(); 
	 filter && hitItr != simRPCHits->end(); ++hitItr) {
      if (abs(hitItr->particleType())!=13) continue;
      uint32_t rpcRawId = hitItr->detUnitId();
      if(rpcRawId==it->rawId()) hasMuonHit = true;
      std::cout<<"RPC hasMuonHit: "<<hasMuonHit<<std::endl;
    }
    ////////////////////////////////
    if(hasMuonHit || !filter) result.push_back( std::make_pair( it->rawId(), it->codedDigi()) );
  }

  std::vector<CSCDigiSpec> csc = cscDetDigis(ev,es);
  for (std::vector<CSCDigiSpec>::const_iterator it = csc.begin(); it!= csc.end(); ++it) {
    //////////Filter noise digis
    bool hasMuonHit = false;
    for (edm::PSimHitContainer::const_iterator hitItr = simCSCHits->begin(); 
	 filter && hitItr != simCSCHits->end(); ++hitItr) {
      if (abs(hitItr->particleType())!=13) continue;
      uint32_t cscRawId = hitItr->detUnitId();      

      CSCDetId aId(cscRawId);
      CSCDetId aIdFromDigi(it->rawId());
      ///detID holds more info than wheel, station and sector
      ///strip everything except the three above
      CSCDetId aIdStripped(aId.endcap(),
		   aId.station(),
		   aId.ring(),
		   aId.chamber());

      if(aIdStripped.rawId()==it->rawId()) hasMuonHit = true;
    }
    ////////////////////////////////
    if(hasMuonHit || !filter) result.push_back( std::make_pair( it->rawId(), it->codedDigi()) );
  }

  std::vector<DTphDigiSpec> dt = dtPhiDetDigis(ev,es);
  for (std::vector<DTphDigiSpec>::const_iterator it = dt.begin(); it!= dt.end(); ++it) {
    //////////Filter noise digis
    bool hasMuonHit = false;
    for (edm::PSimHitContainer::const_iterator hitItr = simDTHits->begin(); 
	 filter && hitItr != simDTHits->end(); ++hitItr) {
      if (abs(hitItr->particleType())!=13) continue;
      uint32_t dtRawId = hitItr->detUnitId();


      DTChamberId aDt(it->rawId());
      DTChamberId aDt1(dtRawId);
      ///Digi detId has sector numbering shifted by 1
      DTChamberId aDtStripped(aDt.wheel(),
			      aDt.station(),
			      aDt.sector()+1);

      ///detID holds more info than wheel, station and sector
      ///strip everything except the three above
      DTChamberId aDtStripped1(aDt1.wheel(),
			      aDt1.station(),
			      aDt1.sector());

      if(aDtStripped1==aDtStripped.rawId()) hasMuonHit = true;
    }
    ////////////////////////////////
    if(hasMuonHit || !filter) result.push_back( std::make_pair( it->rawId(), it->codedDigi()) );
  }

  return result;
}

std::vector<RPCDigiSpec> DetHitDigiGrabber::rpcDetDigis(const edm::Event &ev, const edm::EventSetup &es) const 
{
  std::vector<RPCDigiSpec> result;
  edm::Handle<RPCDigiCollection> rpcDigis;
  static std::string rpcDigisCollName = theConfig.getParameter<std::string>("rpcDigisCollName"); 
  ev.getByLabel( rpcDigisCollName, rpcDigis);
  static bool warnNoColl = theConfig.getUntrackedParameter<bool>("warnNoColl" , true);
  if (!rpcDigis.isValid()) {
    if (warnNoColl) std::cout <<"** WARNING: not collection: "<< rpcDigisCollName << std::endl;
    return result;
  }

  for (RPCDigiCollection::DigiRangeIterator ic=rpcDigis->begin(); ic != rpcDigis->end(); ++ic) {
    RPCDetId rpcDetId =  (*ic).first;
    std::vector<unsigned int> strips;
    for (RPCDigiCollection::const_iterator it = (*ic).second.first; it != (*ic).second.second; ++it) strips.push_back(it->strip());
    std::sort(strips.begin(), strips.end());
    for (std::vector<unsigned int>::const_iterator it=strips.begin(); it!=strips.end(); ++it) {
      unsigned int s = *it; 
      if (result.empty() || (result.back().rawId() !=  rpcDetId.rawId()) ) result.push_back( RPCDigiSpec( rpcDetId.rawId(), s, s ) );
      else if (s - result.back().toStrip() == 1) result.back().setToStrip(s);
      else if (s - result.back().toStrip() > 1) result.push_back( RPCDigiSpec( rpcDetId.rawId(), s, s ) );
    } 
  }
  
//  for (std::vector<RPCDigiSpec>::const_iterator id=result.begin(); id != result.end(); ++id) {
//    std::cout <<" DET: "<< id->rawId() <<" from: "<< id->fromStrip()<<" to:"<< id->toStrip() << std::endl;
//  }

//  edm::ESHandle<RPCGeometry> rpcGeometry;
//  es.get<MuonGeometryRecord>().get(rpcGeometry);
//  const TrackingGeometry::DetIdContainer & detIds = rpcGeometry->detIds();
//  for (TrackingGeometry::DetIdContainer::const_iterator it = detIds.begin(); it != detIds.end(); ++it) {
//    const RPCDigiCollection::Range range = rpcDigis->get(*it);
//    if (range.first !=  range.second) {
//      RPCDetId rpcDet(*it);
//      RPCDetIdUtil  rpcDetUtil( rpcDet );
//      std::cout <<"DET: "<< rpcDetUtil << std::endl;
//      for (RPCDigiCollection::const_iterator id = range.first; id != range.second; ++id) id->print() ;
//    }
//  }
  return result;
}

std::vector<CSCDigiSpec> DetHitDigiGrabber::cscDetDigis(const edm::Event &ev, const edm::EventSetup &es) const 
{
  std::vector<CSCDigiSpec> result;

  edm::Handle<CSCCorrelatedLCTDigiCollection> lctDigiColls;
  static std::string cscDigisCollName = theConfig.getParameter<std::string>("cscCorrelatedLCTDigisCollName");
  ev.getByLabel(cscDigisCollName,lctDigiColls);
  static bool warnNoColl = theConfig.getUntrackedParameter<bool>("warnNoColl" , true);
  if (!lctDigiColls.isValid()) {
    if (warnNoColl) std::cout <<"** WARNING: not collection: "<< cscDigisCollName<< std::endl;
    return result;
  }
  typedef  CSCCorrelatedLCTDigiCollection::DigiRangeIterator DRI;
  for (DRI lctDigiColl = lctDigiColls->begin(); lctDigiColl != lctDigiColls->end(); lctDigiColl++) {
    typedef CSCCorrelatedLCTDigiCollection::const_iterator DI;
    for (DI lctDigi = (*lctDigiColl).second.first; lctDigi != (*lctDigiColl).second.second; lctDigi++) {
      if (lctDigi->isValid()) {
        result.push_back( CSCDigiSpec((*lctDigiColl).first, *lctDigi)); 
//        std::cout << CSCDigiSpec((*lctDigiColl).first, *lctDigi) << std::endl;
//        (*lctDigi).print();
      }
    }
  } 
  return result;
}

std::vector<DTphDigiSpec> DetHitDigiGrabber::dtPhiDetDigis(const edm::Event &ev, const edm::EventSetup &es) const
{
  std::vector<DTphDigiSpec> result;  
  edm::Handle<L1MuDTChambPhContainer> dtPhiDigiColls;
  static std::string dtDigisCollName = theConfig.getParameter<std::string>("dtDigiCollName");
  static bool warnNoColl = theConfig.getUntrackedParameter<bool>("warnNoColl" , true);
  ev.getByLabel(dtDigisCollName,dtPhiDigiColls);
  if (!dtPhiDigiColls.isValid()) {
    if (warnNoColl) std::cout <<"** WARNING: not collection: "<< dtDigisCollName<< std::endl;
    return result;
  }
  L1MuDTChambPhContainer::Phi_Container * phi_Container = dtPhiDigiColls->getContainer();
  typedef L1MuDTChambPhContainer::Phi_iterator IP;

  for (IP ip = phi_Container->begin(); ip < phi_Container->end(); ip++){
    const L1MuDTChambPhDigi& phDigi = (*ip);
    result.push_back( DTphDigiSpec(phDigi));
/*
    DTphDigiSpec d(phDigi); 
    if (    (d.phi() != phDigi.phi())
         || (d.phiB() != phDigi.phiB())
         || (d.code() != phDigi.code())
         || (d.bxNum() != phDigi.bxNum())
         || (d.bxCnt() != phDigi.BxCnt())
         || (d.ts2() != phDigi.Ts2Tag()) ) { 
      std::cout <<" ************* WARNING: " << std::endl;
      std::cout <<"DT_PH:  wheel: "<<phDigi.whNum()
                <<", station: "<<phDigi.stNum()
                <<", sector: "<<phDigi.scNum()
                <<", phi: "<<phDigi.phi()
                <<", phiB: "<<phDigi.phiB()
                <<", q,bx,bxCnt,ts2: "<<phDigi.code()<<" "<<phDigi.bxNum()<<" "<<phDigi.BxCnt()<<" "<<phDigi.Ts2Tag()
                <<std::endl;
      std::cout << result.back() <<std::endl;
    }
*/
  }
  return result;
}
/*
void DetHitDigiGrabber::dtEtaDetDigis(const edm::Event &ev, const edm::EventSetup &es) co
{
  edm::Handle<L1MuDTChambThContainer> dtThetaDigiColls;
  static std::string dtDigisCollName = theConfig.getParameter<std::string>("dtDigiCollName");
  static bool warnNoColl = theConfig.getUntrackedParameter<bool>("warnNoColl" , true);
  ev.getByLabel(dtDigisCollName,dtThetaDigiColls);
  if (!dtThetaDigiColls.isValid()) {
    if (warnNoColl) std::cout <<"** WARNING: not collection: "<< dtDigisCollName<< std::endl;
    return;
  }
  L1MuDTChambThContainer::The_Container * theta_Container = dtThetaDigiColls->getContainer();
  typedef L1MuDTChambThContainer::The_iterator IT;
  for (IT it = theta_Container->begin(); it < theta_Container->end(); it++){
    const L1MuDTChambThDigi& thDigi = (*it);
    std::cout <<"DT_TH:  wheel: "<<thDigi.whNum()
                 <<", station: "<<thDigi.stNum()
                 <<", sector: "<<thDigi.scNum()
                 <<std::endl;
  }
}
*/

void DetHitDigiGrabber::initHistos(TObjArray & histos)
{
  
  hDetHitDigiGrabber_DphiVsPt_Bar = new TH2D("hDetHitDigiGrabber_DphiVsPt_Bar","hDetHitDigiGrabber_DphiVsPt_Bar",100,1.,20.,150.,0.,1.5);
  histos.Add(hDetHitDigiGrabber_DphiVsPt_Bar);
  hDetHitDigiGrabber_DphiVsPt_End = new TH2D("hDetHitDigiGrabber_DphiVsPt_End","hDetHitDigiGrabber_DphiVsPt_End",100,1.,20.,150.,0.,1.5);
  histos.Add(hDetHitDigiGrabber_DphiVsPt_End);
}

