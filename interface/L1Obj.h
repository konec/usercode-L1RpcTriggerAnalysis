#ifndef L1Obj_H
#define L1Obj_H
#include "TObject.h"
#include <iostream>

struct L1Obj : public TObject {

  float pt, eta, phi;
  int   bx, q;

  bool isValid() { return q >= 0;}

  void print() const {std::cout<<"L1Obj pt: "<<pt<<", eta: "<<eta<<", phi: "<<phi<<", q: "<<q<<", bx: "<<bx;}

  L1Obj() : pt(0.),eta(0.),phi(0.),bx(0),q(-1) {}
  ClassDef(L1Obj,1)
};
#endif
