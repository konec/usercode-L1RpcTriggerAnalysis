#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
ClassImp(TrackObj)

std::ostream & operator<< (std::ostream &out, const TrackObj &o)
{
  out<<"TrackObj: ";
  out <<" pt: "<<o.thePt<<", eta: "<<o.theEta<<", phi: "<<o.thePhi<<", charge: "<<o.theCharge;
  return out;
}

