#ifndef UserCode_L1RpcTriggerAnalysis_AnaDet_H
#define UserCode_L1RpcTriggerAnalysis_AnaDet_H

#include "UserCode/L1RpcTriggerAnalysis/interface/DetEfficiencyManager.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/LayerCoincManager.h"

class TGraph;
class TObjArray;
class TH1D;
class MuonObj;
class L1ObjColl;
class L1Obj;
class DetCluDigiObj;

#include <vector>
#include <map>
#include <string>
#include <bitset>

class AnaDet {
public:
  AnaDet() : debug(false) {}
  void init(TObjArray& histos);
  void run( const MuonObj* muon, 
            const std::vector<DetCluDigiObj> & detsHitsCompatibleWithMuon,
            const std::vector<uint32_t> & detsCrossedByMuon,
            const std::vector<uint32_t> & detsCrossedByMuonDeepInside);
            
  TGraph* resume();
  bool debug;

private:
  DetEfficiencyManager theDetEfficiencyManager;
  LayerCoincManager    theLayerCoincManager;
};

#endif
