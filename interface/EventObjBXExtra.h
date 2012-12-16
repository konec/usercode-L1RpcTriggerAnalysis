#ifndef EventObjBxExtra_H
#define EventObjBxExtra_H

#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"

struct EventObjBXExtra : public EventObj {
  EventObjBXExtra(const EventObj& ev) : EventObj(ev), hasValidBX(false), hasValidBX_Minus2(false), hasValidBX_Plus2(false) {}
  bool hasValidBX, hasValidBX_Minus2, hasValidBX_Plus2;
};
#endif
