#include "UserCode/L1RpcTriggerAnalysis/interface/DetHitDigiGrabber.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"

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

  for (edm::PSimHitContainer::const_iterator hitItr = simHits->begin(); hitItr != simHits->end(); ++hitItr) {
    if ( !(13 == abs(hitItr->particleType()) ) ) continue;
    uint32_t rpcRawId = hitItr->detUnitId();
    RPCDetId rpcDet(rpcRawId);
    RPCDetIdUtil  u(rpcDet);
    if((station==2 && ((u.isBarrel() && u.barrelLayer()==4) || (!u.isBarrel() && u.endcapLayer()==2))) ||
       (station==1 && u.isBarrel() && u.barrelLayer()==1)){

      ///Do not use endcap roll 3 in trigger tower 7
      if(!u.isBarrel() && u.endcapLayer()==2 && rpcDet.roll()==3) continue;

      GlobalPoint hitPoint = rpcGeometry->idToDet(rpcDet)->toGlobal(hitItr->entryPoint());
      GlobalVector glbMom = rpcGeometry->idToDet(rpcDet)->toGlobal(hitItr->momentumAtEntry());

      if (u.isBarrel() || result.rawId()==0) result = HitSpecObj(rpcRawId, hitPoint, glbMom);

      TH2D* h = u.isBarrel() ?  hDetHitDigiGrabber_DphiVsPt_Bar : hDetHitDigiGrabber_DphiVsPt_End;
      h->Fill( simu->pt(), deltaPhi(simu->phi(),hitPoint.phi())*simu->charge());
/*
      std::cout <<"Det: "<< rpcRawId 
              <<" glbMom: pt:" << glbMom.perp() <<" pz: "<<glbMom.z()<<" phi: "<<glbMom.phi() 
              <<" globalPosition: r:"<<hitPoint.perp()<<" z:"<<hitPoint.z()<<" phi: "<<hitPoint.phi()
              <<std::endl;
*/
    }
  }
  return result;
}

std::vector< std::pair<uint32_t,uint32_t> > DetHitDigiGrabber::digiCollector(const edm::Event &ev, const edm::EventSetup &es) const
{
  std::vector< std::pair<uint32_t,uint32_t> > result;

  std::vector<RPCDigiSpec> rpc = rpcDetDigis(ev,es);
  for (std::vector<RPCDigiSpec>::const_iterator it = rpc.begin(); it!= rpc.end(); ++it) {
    result.push_back( std::make_pair( it->rawId(), it->codedDigi()) );
  }

  std::vector<CSCDigiSpec> csc = cscDetDigis(ev,es);
  for (std::vector<CSCDigiSpec>::const_iterator it = csc.begin(); it!= csc.end(); ++it) {
    result.push_back( std::make_pair( it->rawId(), it->codedDigi()) );
  }

  std::vector<DTphDigiSpec> dt = dtPhiDetDigis(ev,es);
  for (std::vector<DTphDigiSpec>::const_iterator it = dt.begin(); it!= dt.end(); ++it) {
    result.push_back( std::make_pair( it->rawId(), it->codedDigi()) );
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
  const L1MuDTChambPhContainer::Phi_Container * phi_Container = dtPhiDigiColls->getContainer();
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

