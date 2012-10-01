#ifndef UserCode_L1RpcTriggerAnalysis_AnaClu_H
#define UserCode_L1RpcTriggerAnalysis_AnaClu_H

#include "UserCode/L1RpcTriggerAnalysis/interface/LayerCoincManager.h"

class TGraph;
class TObjArray;
class TH1D;
class MuonObj;
class L1ObjColl;
class L1Obj;
#include <vector>
#include <map>
#include <string>
#include <bitset>

class AnaClu {
public:
  AnaClu() : debug(false) {}
  void init(TObjArray& histos);
  void run( const MuonObj* muon, 
            const L1ObjColl *l1RpcColl,
            const std::vector<uint32_t> & detsHitsCompatibleWithMuon,
            const std::vector<uint32_t> & nDigisCompDets);
  bool debug;

private:
  LayerCoincManager    theLayerCoincManager;
};

#endif
