#ifndef EfficiencyTree_H
#define EfficiencyTree_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"

#include <vector>
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "TObjArray.h"


namespace edm { class Event; class EventSetup; }
namespace reco { class Muon; }
class TTree; 
class TFile;
class TH1F;
class TH2F;
class RPCDetId; 

class EfficiencyTree : public edm::EDAnalyzer {
public:
  EfficiencyTree(const edm::ParameterSet& cfg);
  virtual void beginJob();
  virtual ~EfficiencyTree(){} 
  virtual void analyze(const edm::Event &ev, const edm::EventSetup &es); 
  virtual void endJob();

private:

  class BarrelAndLayer {
  public:
    BarrelAndLayer(const RPCDetId & rpcDet);
    bool isBarrel() const { return theBarrel; }
    unsigned int layer() const { return theLayer; }
  private:
    bool theBarrel; unsigned int theLayer;
  };

  TrajectoryStateOnSurface trackAtSurface(const reco::Muon*, const GlobalPoint&, const edm::Event&, const edm::EventSetup&) const;
  TrajectoryStateOnSurface trackAtSurface(const reco::Muon*, const RPCDetId&, const edm::Event&, const edm::EventSetup&) const;

  edm::ParameterSet theConfig;
                    
  TFile *theFile;
  TTree *theTree;
  
  EventObj* event;
  MuonObj* muon;
  TrackObj* track;
  std::vector<bool> hitBarrel, hitEndcap;
  std::vector<unsigned int> detBarrel, detEndcap;
  std::vector<bool> hitBarrelHP, hitEndcapHP;
  std::vector<unsigned int> detBarrelHP, detEndcapHP;
  std::vector<unsigned int> validPRCHitsBarrel, validPRCEndcap;
  std::vector<unsigned int> validDTHitsBarrel, validDTEndcap;
  std::vector<unsigned int> validCSCHitsBarrel, validCSCEndcap;
  L1ObjColl * l1RpcColl; 
  L1ObjColl * l1OtherColl; 

  TObjArray histos;
//  TH1F *hPullX_Sta, *hPullY_Sta, *hPullX_Tk, *hPullY_Tk;
  TH1F *hPullX_B[6], *hPullX_E[3], *hDistX_B[6], *hDistX_E[3] ,*hPullY; 
  TH1F *hDeltaR_Mu, *hDeltaR_Tk;
  TH1F *hPropToDetDeltaR;
  TH1F *hMinDeltaRTrajMu;
//  TH2F *hTmp;

}; 
#endif
