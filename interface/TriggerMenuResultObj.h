#ifndef TriggerMenuResultObj_H
#define TriggerMenuResultObj_H

#include "TObject.h"
#include <string>
#include <vector>

struct TriggerMenuResultObj : public TObject {
  std::vector<std::string> names; 
  std::vector<unsigned int> firedAlgos; 
  ClassDef(TriggerMenuResultObj,1)
};
#endif

