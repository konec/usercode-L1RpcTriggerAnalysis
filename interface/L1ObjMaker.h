#ifndef L1ObjMaker_H
#define L1ObjMaker_H

#include <vector>
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "FWCore/Utilities/interface/InputTag.h"

namespace edm {class Event; }

class L1ObjMaker {
public:
  L1ObjMaker(const edm::InputTag &readout, const edm::Event &ev) : theReadout(readout), theEv(ev) {} 
  typedef L1Obj::TYPE TYPE;
  std::vector<L1Obj> operator()(TYPE t1, TYPE t2=L1Obj::NONE, TYPE t3=L1Obj::NONE, TYPE t4=L1Obj::NONE); 
private:
  void get(std::vector<L1Obj> & objs, TYPE t);
private:
  template <class T> L1Obj makeL1Obj( T& t, L1Obj::TYPE type) {
    L1Obj obj;
    obj.bx = t.bx();
    obj.q  = t.quality();
    obj.pt = t.ptValue();
    obj.eta = t.etaValue();
    obj.phi = t.phiValue();
    obj.type = type;
    return obj;
  }
  edm::InputTag theReadout;
  const edm::Event & theEv;
};
#endif
