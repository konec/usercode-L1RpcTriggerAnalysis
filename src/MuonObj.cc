#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"

//ClassImp(MuonObj)

std::ostream & operator<< (std::ostream &out, const MuonObj &o)
{
  //out<<(TrackObj)o
  out<<" MuonObj: ";
  out <<" stat: "<<o.nMatchedStations;
  out <<" hits: "<<o.nTrackerHits<<"_"<<o.nDTHits<<"_"<<o.nCSCHits<<"_"<<o.nRPCHits;
  if (o.isTracker()) out << "_TRK";
  if (o.isOuter())   out << "_OUT";
  if (o.isGlobal())  out << "_GLB";
  out <<" #AllMuons: "<<o.nAllMuons;
  if (o.isUnique) out<<"_UNIQUE";
  return out;
}


