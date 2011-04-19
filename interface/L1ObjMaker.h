#ifndef L1ObjMaker_H
#define L1ObjMaker_H

#include <vector>
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "FWCore/Utilities/interface/InputTag.h"

namespace edm {class Event; }

class L1ObjMaker {
public:
  L1ObjMaker(const edm::InputTag &readout, const edm::InputTag &readoutEmu, const edm::Event &ev) : theReadout(readout), theReadoutEmu(readoutEmu), theEv(ev) {}
  enum TYPE { NONE, RPCB, RPCF, DT, CSC };
  std::vector<L1Obj> operator()(TYPE t1, TYPE t2=NONE, TYPE t3=NONE, TYPE t4=NONE); 
private:
  void get(std::vector<L1Obj> & objs, TYPE t);
private:
  edm::InputTag theReadout;
  edm::InputTag theReadoutEmu;
  const edm::Event & theEv;
};
#endif
