#ifndef EventData_H
#define EventData_H
#include "TObject.h"
#include <iostream>

#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"

///Small class to create a TTree
///holding trigger response.

struct EventData: public TObject{

  EventData(){};
  ~EventData(){}

  void clear();
  
  ///Header
  float weight;
  ///Generated kinematics
  float pt;
  float eta;
  float phi;
  float phiHit;
  float etaHit;
  int charge;

  std::vector<L1Obj> l1ObjectsOtf;
  std::vector<L1Obj> l1ObjectsGmt;
  std::vector<L1Obj> l1ObjectsRpc;
  std::vector<L1Obj> l1ObjectsOther;

  ClassDef(EventData,1)
};
///////////////////////////////////////
#endif
