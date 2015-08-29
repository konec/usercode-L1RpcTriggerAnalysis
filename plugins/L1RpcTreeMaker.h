#ifndef L1RpcTreeMaker_H
#define L1RpcTreeMaker_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObjVect.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObjVect.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/BestMuonFinder.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetHitCompatibleCollector.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsGrabber.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/FilterMenu.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjMaker.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetHitDigiGrabber.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h"


#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/RPCDigi/interface/RPCRawSynchro.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"




#include <vector>
#include "TObjArray.h"


namespace edm { class Event; class EventSetup; }
namespace reco { class Muon; }
class TTree;
class TFile;
class TH1F;
class TH2F;
class RPCDetId;
class TriggerMenuResultObj;

class L1RpcTreeMaker : public edm::EDAnalyzer {
public:
  L1RpcTreeMaker(const edm::ParameterSet& cfg);
  virtual ~L1RpcTreeMaker();
  virtual void beginJob();
  virtual void beginRun(const edm::Run &ru, const edm::EventSetup &es);
  virtual void analyze(const edm::Event &ev, const edm::EventSetup &es);
  virtual void endJob();

private:
  edm::ParameterSet theConfig;
  TFile *theFile;
  TTree *theTree;

  EventObj* event;
  MuonObj* muon;
  TrackObj* simu;
  TriggerMenuResultObj *bitsL1;
  TriggerMenuResultObj *bitsHLT;
  
//  std::vector<SynchroCountsObj> counts;
  std::vector<uint32_t> detsCrossedByMuon, detsCrossedByMuonDeepInside, detsSIMU;
//  std::vector<DetCluDigiObj> detsHitsCompatibleWithMuon;
  DetCluDigiObjVect * detsHitsCompatibleWithMuon;
  SynchroCountsObjVect * synchroCounts;
  L1ObjColl * l1ObjColl;
  HitSpecObj *hitSpec, *hitSpecSt1;
  std::vector< std::pair<uint32_t, uint32_t> > digSpec;

  unsigned int theCounter;
								    
  TObjArray      theHelper;
  BestMuonFinder theBestMuonFinder;
  
  DetHitCompatibleCollector theDetHitCollector;
  SynchroCountsGrabber theSynchroGrabber;
  L1ObjMaker theL1ObjMaker;
  FilterMenu theMenuInspector;
  DetHitDigiGrabber theDetHitDigiGrabber;
private:
   edm::EDGetTokenT<reco::MuonCollection> theBestMuon_Tag;
   edm::EDGetTokenT<RPCRawSynchro::ProdItem> theSynchroCounts_Tag;

   edm::EDGetTokenT<edm::TriggerResults> theHLTTriggerResult_Tag;
   edm::EDGetTokenT<L1GtTriggerMenuLite> theL1TriggerMenuLite_Tag;
   edm::EDGetTokenT<L1GlobalTriggerReadoutRecord> theL1TriggerResult_Tag; 
   edm::EDGetTokenT<L1MuGMTReadoutCollection>     theL1GMTEmulReadout_Tag; 
   edm::EDGetTokenT<L1MuGMTReadoutCollection>     theL1GMTReadout_Tag; 
   edm::EDGetTokenT<std::vector<L1MuRegionalCand> >     theL1RpcbEmu_Tag;
   edm::EDGetTokenT<std::vector<L1MuRegionalCand> >     theL1RpcfEmu_Tag;
};
#endif
