#ifndef EfficiencyTree_H
#define EfficiencyTree_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <vector>
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "TObjArray.h"



namespace edm { class Event; class EventSetup; }
class TTree; 
class TFile;
class TH1F;

class EfficiencyTree : public edm::EDAnalyzer {
public:
  EfficiencyTree(const edm::ParameterSet& cfg);
  virtual void beginJob();
  virtual ~EfficiencyTree(){} 
  virtual void analyze(const edm::Event &ev, const edm::EventSetup &es); 
  virtual void endJob();
private:
  edm::ParameterSet theConfig;
                    
  TFile *theFile;
  TTree *theTree;
  
  EventObj* event;
  MuonObj* muon;
  TrackObj* track;
  std::vector<bool> hitBarrel, hitEndcap;
  std::vector<unsigned int> detBarrel, detEndcap;
  std::vector<unsigned int> validPRCHitsBarrel, validPRCEndcap;
  std::vector<unsigned int> validDTHitsBarrel, validDTEndcap;
  std::vector<unsigned int> validCSCHitsBarrel, validCSCEndcap;
  L1ObjColl * l1RpcColl; 
  L1ObjColl * l1OtherColl; 

  TObjArray histos;
  TH1F *hPullX_Sta, *hPullY_Sta, *hPullX_Tk, *hPullY_Tk;
  TH1F *hDeltaR_Sta, *hDeltaR_TkMu, *hDeltaR_TkTk;
  TH1F *hPropToDetDeltaR;

}; 
#endif
