#ifndef L1Obj_H
#define L1Obj_H
#include "TObject.h"
#include <iostream>

struct L1Obj : public TObject {

  enum TYPE { NONE, RPCb, RPCf, DT, CSC, GMT, RPCb_emu, RPCf_emu, GMT_emu };

  float pt, eta, phi;
  int   bx, q;
  TYPE  type;

  L1Obj() : pt(0.),eta(0.),phi(0.),bx(0),q(-1), type(NONE) {}
  bool isValid() { return q >= 0;}

  friend ostream & operator<< (ostream &out, const L1Obj &o);

  ClassDef(L1Obj,1)
};
#endif
