#ifndef UserCode_L1RpcTriggerAnalysis_DetHitDigiGrabber_H
#define UserCode_L1RpcTriggerAnalysis_DetHitDigiGrabber_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"

namespace edm { class Event; class EventSetup; }
class TObjArray;
class TrackObj;



class DetHitDigiGrabber {
public:
  DetHitDigiGrabber(const edm::ParameterSet& cfg);
  ~DetHitDigiGrabber();

  //
  // get reference hit point (MS2out) or (RE2)
  //
  HitSpecObj rpcDetHits(const edm::Event &ev, const edm::EventSetup &es, const TrackObj * simu, int station = 2) const;
  std::vector< std::pair<uint32_t, uint32_t> > digiCollector(const edm::Event &ev, const edm::EventSetup &es) const;

  //std::vector<RpcDigiSpec> 
  std::vector<RPCDigiSpec> rpcDetDigis(const edm::Event &ev, const edm::EventSetup &es) const;
  std::vector<CSCDigiSpec> cscDetDigis(const edm::Event &ev, const edm::EventSetup &es) const ; 
  std::vector<DTphDigiSpec>  dtPhiDetDigis(const edm::Event &ev, const edm::EventSetup &es)const; 
//  void  dtEtaDetDigis(const edm::Event &ev, const edm::EventSetup &es)const; 

  void initHistos(TObjArray & histos);
private:
  edm::ParameterSet theConfig;
 
}; 
#endif 

