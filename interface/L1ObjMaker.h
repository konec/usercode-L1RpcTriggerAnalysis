#ifndef L1ObjMaker_H
#define L1ObjMaker_H

#include <vector>
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Provenance/interface/RunID.h"
#include "DataFormats/Provenance/interface/EventID.h"


namespace edm {class Event; }

class L1ObjMaker {
public:
  L1ObjMaker(const  edm::ParameterSet & cfg); 
  const std::vector<L1Obj> & operator()(const edm::Event &ev) { run(ev); return theL1Objs; }
  
private:
  void run(const edm::Event &ev);
  bool getGMTReadout(const edm::Event &ev,  std::vector<L1Obj> & result, const edm::InputTag &readout, L1Obj::TYPE t);
  bool getRpcRegional(const edm::Event &ev,  std::vector<L1Obj> & result, const edm::InputTag &l1RpcDigis);

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
private:
  edm::ParameterSet   theConfig;
  std::vector<L1Obj>  theL1Objs;
  edm::EventNumber_t lastEvent;
  edm::RunNumber_t   lastRun;

};
#endif
