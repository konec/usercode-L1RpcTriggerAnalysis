#ifndef TrackObj_H
#define TrackObj_H
#include "TObject.h"

class TrackObj : public TObject {
public:
  TrackObj(float pt=0., float eta=0., float phi=0.) : thePt(pt), theEta(eta), thePhi(phi) {}
  void setKine(float pt, float eta, float phi) { thePt=pt; theEta=eta; thePhi=phi;}
  virtual ~TrackObj(){}
  float pt() const { return thePt;}
  float eta() const { return theEta;}
  float phi() const { return thePhi;}
private:
  float thePt, theEta, thePhi;
public:
ClassDef(TrackObj,1)
};
#endif
