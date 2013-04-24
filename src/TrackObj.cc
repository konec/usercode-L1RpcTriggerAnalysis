#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
ClassImp(TrackObj)

ostream & operator<< (ostream &out, const TrackObj &o)
{
  out<<"TrackObj: ";
  out <<" pt: "<<o.thePt<<", eta: "<<o.theEta<<", phi: "<<o.thePhi;
  return out;
}

