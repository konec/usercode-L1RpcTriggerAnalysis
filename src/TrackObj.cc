#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"

//ClassImp(TrackObj)

TrackObj::TrackObj(float pt, float eta, float phi, int charge) : thePt(pt), theEta(eta), thePhi(phi), theCharge(charge) {}

void TrackObj::setKine(float pt, float eta, float phi, int charge) { thePt=pt; theEta=eta; thePhi=phi; theCharge=charge;}

std::ostream & operator<< (std::ostream &out, const TrackObj &o)
{
  out<<"TrackObj: ";
  out <<" pt: "<<o.thePt<<", eta: "<<o.theEta<<", phi: "<<o.thePhi<<", charge: "<<o.theCharge;
  return out;
}

