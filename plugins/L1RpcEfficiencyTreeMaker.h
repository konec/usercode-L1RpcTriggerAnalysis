#ifndef L1RpcEfficiencyTreeMaker_H
#define L1RpcEfficiencyTreeMaker_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/BestMuonFinder.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetHitCompatibleCollector.h"


#include <vector>
#include "TObjArray.h"


namespace edm { class Event; class EventSetup; }
namespace reco { class Muon; }
class TTree;
class TFile;
class TH1F;
class TH2F;
class RPCDetId;

class L1RpcEfficiencyTreeMaker : public edm::EDAnalyzer {
public:
  L1RpcEfficiencyTreeMaker(const edm::ParameterSet& cfg);
  virtual ~L1RpcEfficiencyTreeMaker();
  virtual void beginJob();
  virtual void analyze(const edm::Event &ev, const edm::EventSetup &es);
  virtual void endJob();

private:
  edm::ParameterSet theConfig;
  TFile *theFile;
  TTree *theTree;

  EventObj* event;
  MuonObj* muon;
  TrackObj* track;
  std::vector<uint32_t> detsCrossedByMuon, detsCrossedByMuonDeepInside, detsHitsCompatibleWithMuon, detsSIMU;
  L1ObjColl * l1RpcColl;
  L1ObjColl * l1OtherColl;
  L1ObjColl * l1RpcCollEmu;

  unsigned int theCounter;
								    
  TObjArray      theHelper;
  BestMuonFinder theBestMuonFinder;
  DetHitCompatibleCollector theDetHitCollector;
};
#endif
