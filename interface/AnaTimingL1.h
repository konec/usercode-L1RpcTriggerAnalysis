#ifndef UserCode_L1RpcTriggerAnalysis_AnaTimingL1_H
#define UserCode_L1RpcTriggerAnalysis_AnaTimingL1_H

class TObjArray;
class MuonObj;
class L1ObjColl;
class L1Obj;
class EventObj;

#include "FWCore/ParameterSet/interface/ParameterSet.h"

class AnaTimingL1 {

public:
  AnaTimingL1(const edm::ParameterSet& cfg) : debug(false), theConfig(cfg) {}
  void init(TObjArray& histos);
  void run(const EventObj* ev, const MuonObj* muon, const L1ObjColl *l1Coll);
  void resume(TObjArray& histos);

  bool debug;

private:
  edm::ParameterSet theConfig;

};
#endif

