#ifndef EfficiencyTree_H
#define EfficiencyTree_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <vector>
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"


namespace edm { class Event; class EventSetup; }
class TTree; 
class TFile;

class EfficiencyTree : public edm::EDAnalyzer {
public:
  EfficiencyTree(const edm::ParameterSet& cfg);
  virtual void beginJob();
  virtual ~EfficiencyTree(){} 
  virtual void analyze(const edm::Event &ev, const edm::EventSetup &es); 
  virtual void endJob();
private:
  edm::ParameterSet theConfig;

  // std::vector<float> 
  // std::vector<float> 
  L1Obj l1Rpc;
  L1Obj l1Other;
  EventObj event;
  TrackObj track;
  TrackObj muon;

  std::vector<bool> hitBarrel, hitEndcap;
                    
  TFile *theFile;
  TTree *theTree;
}; 
#endif
