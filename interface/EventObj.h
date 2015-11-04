#ifndef EventObj_H
#define EventObj_H
#include "TObject.h"
#include <iostream>



class EventObj : public TObject {
 public:
  unsigned int bx,id,lumi,run;
  ULong64_t time, orbit;
  //ClassDef(EventObj,1)

};

std::ostream & operator<< (std::ostream &out, const EventObj&o);

#endif
