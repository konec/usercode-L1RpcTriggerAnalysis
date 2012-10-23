#ifndef UserCode_L1RpcTriggerAnalysis_AnaClu_H
#define UserCode_L1RpcTriggerAnalysis_AnaClu_H

class TGraph;
class TObjArray;
class TH1D;
class MuonObj;
class EventObj;
class L1ObjColl;
class L1Obj;
class DetCluDigiObj;
#include <vector>
#include <map>
#include <string>
#include <bitset>

class AnaClu {
public:
  AnaClu() : debug(false) {}
  void init(TObjArray& histos);
  void run( const EventObj* ev, const MuonObj* muon, 
            const L1ObjColl *l1RpcColl,
            const std::vector<DetCluDigiObj> & detsHitsCompatibleWithMuon);
  TGraph* resume();
  bool debug;
private:
   typedef std::map< unsigned int, std::pair<unsigned int, unsigned int> > CluRunMap;
   CluRunMap cluRunMap;
};

#endif
