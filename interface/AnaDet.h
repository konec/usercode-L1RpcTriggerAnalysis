#ifndef UserCode_L1RpcTriggerAnalysis_AnaDet_H
#define UserCode_L1RpcTriggerAnalysis_AnaDet_H

#include "UserCode/L1RpcTriggerAnalysis/interface/DetEfficiencyManager.h"

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

class AnaDet {
public:
  AnaDet(TObjArray& histos);
  void run( const MuonObj* muon, 
            const std::vector<uint32_t> & detsHitsCompatibleWithMuon,
            const std::vector<uint32_t> & detsCrossedByMuon,
            const std::vector<uint32_t> & detsCrossedByMuonDeepInside);
            
  TGraph* resume();
  bool debug;

private:
  DetEfficiencyManager theDetEfficiencyManager;
  std::vector< std::bitset<6> > theLayers;
};

#endif
