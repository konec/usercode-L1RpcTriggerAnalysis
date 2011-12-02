#ifndef UserCode_L1RpcTriggerAnalysis_AnaEmu_H
#define UserCode_L1RpcTriggerAnalysis_AnaEmu_H

class TObjArray;
class TH1D;
class MuonObj;
class L1ObjColl;
class L1Obj;
class EventObj;
#include <vector>
#include <map>
#include <string>

class AnaEmu {
public:
  AnaEmu(TObjArray& histos);
  void run( const EventObj* event, const MuonObj* muon,
            const L1ObjColl *l1RpcCollEmu,
            const L1ObjColl *l1RpcColl);
  bool debug;

private:
};

#endif
