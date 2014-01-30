#ifndef UserCode_L1RpcTriggerAnalysis_AnaSiMuDistribution_H
#define UserCode_L1RpcTriggerAnalysis_AnaSiMuDistribution_H


class TObjArray;
class HitSpecObj;
class TrackObj;
class EventObj;
#include <vector>
#include <cstdint>

namespace edm {class ParameterSet;}

class AnaSiMuDistribution {
public:
  AnaSiMuDistribution (const edm::ParameterSet&);
  void init(TObjArray& histos);
  bool filter(const EventObj* ev, 
	      const TrackObj * simu, 
	      const HitSpecObj * hitSpec,
	      const HitSpecObj * hitSpecProp=0);
private:
  double ptMin, ptMax, etaMinRef, etaMaxRef, phiMinRef, phiMaxRef;
  bool checkMatchedDets;
  bool checkMatchedSectors;
  std::vector<uint32_t> matchedDets;
  std::vector<uint32_t> matchedSectors;
};
#endif

