#ifndef L1Obj_H
#define L1Obj_H
#include "TObject.h"
#include <iostream>

struct L1Obj : public TObject {

  enum TYPE { NONE, RPCB, RPCF, DT, CSC, GMT };

  float pt, eta, phi;
  int   bx, q;
  TYPE  type;

  bool isValid() { return q >= 0;}

  void print() const {
    std::cout<<"L1Obj: ";
    switch (type) {
      case RPCB: { std::cout <<"RPCB"; break; }
      case RPCF: { std::cout <<"RPCF"; break; }
      case DT:   { std::cout <<"DT  "; break; }
      case CSC:  { std::cout <<"CSC "; break; }
      case GMT:  { std::cout <<"GMT "; break; }
      default: std::cout <<"NONE";
    };
    std::cout <<" pt: "<<pt<<", eta: "<<eta<<", phi: "<<phi<<", q: "<<q<<", bx: "<<bx;
  }

  L1Obj() : pt(0.),eta(0.),phi(0.),bx(0),q(-1), type(NONE) {}
  ClassDef(L1Obj,1)
};
#endif
