#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
ClassImp(EventObj)
ostream & operator<< (ostream &out, const EventObj &o) {
  out <<"run: "<< o.run <<" event: "<<o.id<<" lumi: "<<o.lumi; 
  return out;
}

