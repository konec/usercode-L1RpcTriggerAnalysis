#ifndef L1Obj_H
#define L1Obj_H
#include "TObject.h"
#include <iostream>

struct L1Obj : public TObject {

  enum TYPE { NONE, RPCb, RPCf, DT, CSC, GMT, RPCb_emu, RPCf_emu, GMT_emu, OTF };

  float pt, eta, phi;
  int   bx, q, charge;
  TYPE  type;

 L1Obj() : pt(-1.),eta(99.),phi(99.),bx(0),q(-1), charge(99), type(NONE) {}
  bool isValid() const { return q >= 0;}

  friend ostream & operator<< (ostream &out, const L1Obj &o);

  ClassDef(L1Obj,1)
};
#endif
