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
#include "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/BestMuonFinder.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetHitCompatibleCollector.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsGrabber.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/FilterMenu.h"



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
  TrackObj* track;
  TriggerMenuResultObj *bitsL1;
  TriggerMenuResultObj *bitsHLT;
  
  std::vector<SynchroCountsObj> counts;
  std::vector<uint32_t> detsCrossedByMuon, detsCrossedByMuonDeepInside, detsSIMU;
  std::vector<DetCluDigiObj> detsHitsCompatibleWithMuon;
  L1ObjColl * l1RpcColl;
  L1ObjColl * l1OtherColl;
  L1ObjColl * l1RpcCollEmu;
  L1ObjColl * l1GmtColl;

  unsigned int theCounter;
								    
  TObjArray      theHelper;
  BestMuonFinder theBestMuonFinder;
  
  DetHitCompatibleCollector theDetHitCollector;
  SynchroCountsGrabber theSynchroGrabber;
  FilterMenu theMenuInspector;
};
#endif
