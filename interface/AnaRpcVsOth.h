#ifndef UserCode_L1RpcTriggerAnalysis_AnaRpcVsOth_H
#define UserCode_L1RpcTriggerAnalysis_AnaRpcVsOth_H
class TObjArray;
class TH1D;
class TH2D;
class TrackObj;
class L1ObjColl;
class L1Obj;
#include <vector>
#include <map>
#include <string>
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class AnaRpcVsOth {
public: 
  AnaRpcVsOth(const edm::ParameterSet & cfg) : theConfig(cfg) {}
  void init(TObjArray& histos); 
  void run(const TrackObj* muon, const L1ObjColl *l1Coll);
private:
  double maxPt(const std::vector<L1Obj> & l1Objs) const; 

  std::map< std::string, TH1D* > h1;
  std::map< std::string, TH2D* > h2;

  const static int nBinsEff= 6;
  const static double binsEff[];
  
  edm::ParameterSet theConfig;
};

#endif
