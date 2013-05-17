#ifndef TrackObj_H
#define TrackObj_H
#include "TObject.h"
#include <ostream>

class TrackObj : public TObject {
public:
  TrackObj(float pt=0., float eta=0., float phi=0., int charge=0) : thePt(pt), theEta(eta), thePhi(phi), theCharge(charge) {}
    void setKine(float pt, float eta, float phi, int charge) { thePt=pt; theEta=eta; thePhi=phi; theCharge=charge;}
  virtual ~TrackObj(){}
  float pt() const { return thePt;}
  float eta() const { return theEta;}
  float phi() const { return thePhi;}
  int charge() const { return theCharge;}
private:
   float thePt, theEta, thePhi;
   int theCharge;
   friend ostream & operator<< (ostream &out, const TrackObj &o);
public:
ClassDef(TrackObj,1)
};
#endif
