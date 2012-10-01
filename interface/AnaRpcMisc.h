#ifndef UserCode_L1RpcTriggerAnalysis_AnaRpcMisc_H
#define UserCode_L1RpcTriggerAnalysis_AnaRpcMisc_H
class TObjArray;
class TH1D;
class MuonObj;
class L1ObjColl;
class L1Obj;
class TH2D;
class EventObj;
class TGraph;
#include <vector>
#include <map>

class AnaRpcMisc {
public: 
  AnaRpcMisc() : debug(false) {}
  void init(TObjArray& histos); 
  void run(const EventObj* ev, const MuonObj* muon, const L1ObjColl *l1RpcColl, const L1ObjColl *l1OtherColl);
  TGraph* resume();

  bool debug;
private:
  double maxPt(const std::vector<L1Obj> & l1Objs) const;
  TH2D *hRpcMisc_UE, *hRpcMisc_OE;
  TH1D *hRpcMisc_EffRun;
  TH1D *hRpcMisc_Time, *hRpcMisc_TimeAll, *hRpcMisc_TimeDen;
  typedef std::map< unsigned int, std::pair<unsigned int, unsigned int> > EffRunMap;
  EffRunMap effRunMap;
};

#endif
