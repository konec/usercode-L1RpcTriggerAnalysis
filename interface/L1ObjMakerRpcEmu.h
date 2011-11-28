#ifndef L1ObjMakerRpcEmu_H
#define L1ObjMakerRpcEmu_H

#include <vector>
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "FWCore/Utilities/interface/InputTag.h"

namespace edm {class Event; }

class L1ObjMakerRpcEmu {
public:
  L1ObjMakerRpcEmu(const edm::InputTag &rpcTriggerDigis, const edm::Event &ev) : theL1RpcDigis(rpcTriggerDigis), theEv(ev) {} 
  std::vector<L1Obj> operator()();
private:
  edm::InputTag theL1RpcDigis;
  const edm::Event & theEv;
};
#endif

