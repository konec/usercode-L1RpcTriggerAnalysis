#ifndef DetCluDigiObj_H
#define DetCluDigiObj_H

#include "TObject.h"

struct DetCluDigiObj : public TObject {
  UInt_t det;
  UInt_t clusterSize, nDigis;
ClassDef(DetCluDigiObj,1)
};
#endif

