#ifndef UserCode_L1RpcTriggerAnalysis_AnaRpcVsOth_H
#define UserCode_L1RpcTriggerAnalysis_AnaRpcVsOth_H
class TObjArray;
class TH1D;
class TH2D;
class MuonObj;
class L1ObjColl;
class L1Obj;
#include <vector>
#include <map>
#include <string>

class AnaRpcVsOth {
public: 
  AnaRpcVsOth(TObjArray& histos); 
  void run(const MuonObj* muon, const L1ObjColl *l1RpcColl, const L1ObjColl *l1OtherColl);
private:
  double maxPt(const std::vector<L1Obj> & l1Objs) const; 

  std::map< std::string, TH1D* > h1;
  std::map< std::string, TH2D* > h2;

  const static int nBinsEff= 6;
  const static double binsEff[];
  
};

#endif
