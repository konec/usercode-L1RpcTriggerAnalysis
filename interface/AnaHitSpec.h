#ifndef UserCode_L1RpcTriggerAnalysis_AnaHitSpec_H
#define UserCode_L1RpcTriggerAnalysis_AnaHitSpec_H

class TrackObj;
class HitSpecObj;
class TH2D;
class TObjArray;
class EventObj;

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <map>


class AnaHitSpec {
public:
  AnaHitSpec(const edm::ParameterSet & cfg) : theConfig(cfg) {}
   void init(TObjArray& histos);
   void run(const EventObj* ev, const TrackObj * simu, const HitSpecObj * hitSpec);
   void resume(TObjArray& histos);
private:
  edm::ParameterSet theConfig;
  typedef std::map< uint32_t, TH2D* > HMap;
  HMap  theH;
};
#endif
