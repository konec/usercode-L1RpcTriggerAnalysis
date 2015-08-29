#ifndef  UserCode_L1RpcTriggerAnalysis_DetHitCompatibleCollector_H 
#define  UserCode_L1RpcTriggerAnalysis_DetHitCompatibleCollector_H 

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObj.h"


namespace edm { class Event; class EventSetup; }
namespace reco { class Muon; }
class TObjArray;
class TH1F;
class TH2F;

class DetHitCompatibleCollector {
public:
  DetHitCompatibleCollector(const edm::ParameterSet& cfg);
  ~ DetHitCompatibleCollector();

//  typedef std::pair< uint32_t, std::pair<unsigned int, unsigned int> > DetCluDigi;
  std::vector<DetCluDigiObj> compatibleHits( const reco::Muon* muon, const edm::Event &ev, const edm::EventSetup &es);
  std::vector<uint32_t>   compatibleDets( const reco::Muon* muon, const edm::Event &ev, const edm::EventSetup &es, bool deepInside);
  std::vector<uint32_t>   compatibleSIMU( const reco::Muon* muon, const edm::Event &ev, const edm::EventSetup &es);
//  std::vector<uint32_t>   nDigisCompDets( const std::vector<uint32_t> & detIds, const edm::Event &ev, const edm::EventSetup &es);
//  std::vector<uint32_t>   clSizeCompDets( const std::vector<uint32_t> & detIds, const edm::Event &ev, const edm::EventSetup &es);
  void initHistos( TObjArray & histos);
  
private:
  TH1F *hPullX_B[6], *hPullX_E[4], *hDistX_B[6], *hDistX_E[4] , *hPullX;
  TH1F *hPullY_B[6], *hPullY_E[4], *hDistY_B[6], *hDistY_E[4] , *hPullY;
  TH2F *hPlaceComp, *hPlaceDiff;
  TH1F *hPropToDetDeltaR;

  bool theNoDigiWarning;
};
#endif
