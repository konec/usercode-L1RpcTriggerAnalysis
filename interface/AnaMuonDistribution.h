#ifndef UserCode_L1RpcTriggerAnalysis_AnaMuonDistribution_H
#define UserCode_L1RpcTriggerAnalysis_AnaMuonDistribution_H


class TObjArray;
class MuonObj;

class AnaMuonDistribution {
public:
  AnaMuonDistribution(TObjArray& histos);
  void run(const MuonObj* muon);
};
#endif
