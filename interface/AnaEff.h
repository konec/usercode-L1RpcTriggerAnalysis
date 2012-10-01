#ifndef UserCode_L1RpcTriggerAnalysis_AnaEff_H
#define UserCode_L1RpcTriggerAnalysis_AnaEff_H
class TObjArray;
class TH1D;
class MuonObj;
class L1ObjColl;
class L1Obj;
#include <vector>
#include <map>
#include <string>




class AnaEff {
public: 
  AnaEff() : debug(false) {}
  void init(TObjArray& histos); 
  void run(const MuonObj* muon, const L1ObjColl *l1RpcColl, const L1ObjColl *l1OtherColl);

  bool debug;

private:
  double maxPt(const std::vector<L1Obj> & l1Objs) const; 
  TH1D* hEfficMuPt_D, *hEfficRpcNoCut_N, *hEfficRpcPtCut_N;
  std::map< std::string, TH1D* > hm;

  const static unsigned int nPtCuts= 6;
  const static double ptCuts[];

};

#endif
