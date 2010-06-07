#ifndef L1Obj_H
#define L1Obj_H
struct L1Obj {
  float pt, eta, phi;
  int   bx, q;
  bool isValid() { return q >= 0;}
  L1Obj() : pt(0.),eta(0.),phi(0.),bx(0),q(-1) {}
};
#endif
