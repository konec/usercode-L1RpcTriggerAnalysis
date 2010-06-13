#ifndef L1Obj_H
#define L1Obj_H
#include "TObject.h"

struct L1Obj : public TObject {

  float pt, eta, phi;
  int   bx, q;

  bool isValid() { return q >= 0;}

  L1Obj() : pt(0.),eta(0.),phi(0.),bx(0),q(-1) {}
  ClassDef(L1Obj,1)
};
#endif
