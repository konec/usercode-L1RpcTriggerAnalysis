#ifndef DetCluDigiObjVect_H
#define DetCluDigiObjVect_H

#include "TObject.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObj.h"

#include <vector>

struct DetCluDigiObjVect : public TObject {
  std::vector< DetCluDigiObj > data;
ClassDef(DetCluDigiObjVect,1)
};

#endif
