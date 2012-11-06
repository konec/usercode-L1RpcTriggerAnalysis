#ifndef UserCode_L1RpcTriggerAnalysis_AnaTimingL1_H
#define UserCode_L1RpcTriggerAnalysis_AnaTimingL1_H

class TObjArray;
class MuonObj;
class L1ObjColl;
class L1Obj;
class EventObj;

#include <vector>
#include <map>


class AnaTimingL1 {
public:
  AnaTimingL1() : debug(false) {}
  void init(TObjArray& histos);
  void run(const EventObj* ev, const MuonObj* muon, const L1ObjColl *l1Coll);
  void resume(TObjArray& histos);
  bool debug;

private:
//  double maxPt(const std::vector<L1Obj> & l1Objs) const;
//  typedef std::map< unsigned int, std::pair<unsigned int, unsigned int> > EffRunMap;
//  EffRunMap effRunMap;

};
#endif

