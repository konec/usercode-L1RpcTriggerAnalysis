#ifndef L1Obj_H
#define L1Obj_H
#include "TObject.h"
#include <iostream>

struct L1Obj : public TObject {

  enum TYPE { NONE, RPCb, RPCf, DT, CSC, GMT, RPCb_emu, RPCf_emu, GMT_emu };

  float pt, eta, phi;
  int   bx, q;
  TYPE  type;

  bool isValid() { return q >= 0;}

  void print() const {
    std::cout<<"L1Obj: ";
    switch (type) {
      case RPCb:     { std::cout <<"RPCb    "; break; }
      case RPCf:     { std::cout <<"RPCf    "; break; }
      case DT:       { std::cout <<"DT      "; break; }
      case CSC:      { std::cout <<"CSC     "; break; }
      case GMT:      { std::cout <<"GMT     "; break; }
      case RPCb_emu: { std::cout <<"RPCb_emu"; break; }
      case RPCf_emu: { std::cout <<"RPCf_emu"; break; }
      case GMT_emu:  { std::cout <<"GMT_emu "; break; }
      case NONE   :  { std::cout <<"NONE    "; break; }
      default: std::cout <<"Unknown";
    };
    std::cout <<" pt: "<<pt<<", eta: "<<eta<<", phi: "<<phi<<", q: "<<q<<", bx: "<<bx;
  }

  L1Obj() : pt(0.),eta(0.),phi(0.),bx(0),q(-1), type(NONE) {}
  ClassDef(L1Obj,1)
};
#endif
