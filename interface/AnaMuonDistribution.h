#ifndef UserCode_L1RpcTriggerAnalysis_AnaMuonDistribution_H
#define UserCode_L1RpcTriggerAnalysis_AnaMuonDistribution_H


class TObjArray;
class MuonObj;
namespace edm {class ParameterSet;}

class AnaMuonDistribution {
public:
  AnaMuonDistribution (const edm::ParameterSet&);
  void init(TObjArray& histos);
  bool filter(const MuonObj* muon);
private:
  double ptMin, etaMax;
  unsigned int  minNumberOfMatchedStations, minNumberRpcHits, minNumberDtCscHits;
  bool requireUnique;
};
#endif
