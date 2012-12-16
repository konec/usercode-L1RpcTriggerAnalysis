#ifndef UserCode_L1RpcTriggerAnalysis_AnaEvent_H
#define UserCode_L1RpcTriggerAnalysis_AnaEvent_H

class EventObj;
class TObjArray;
namespace edm { class ParameterSet; }

#include <map>
#include <vector>
#include <bitset>


class AnaEvent {
public:
  AnaEvent(const edm::ParameterSet& cfg);
  void init(TObjArray& histos);
  bool filter(EventObj* ev);
  void resume(TObjArray& histos);
  bool debug;
private:
  std::vector<unsigned int> theSkipRuns;
  std::bitset<3564> theValidBX;
  std::map< std::pair<unsigned int, unsigned int>, uint64_t> theRunEvent;
  bool theUseValidBX_ONLY, theUseValidBX_NoBxMinus2_ONLY, theUseValidBX_NoBxPlus2_ONLY;
};

#endif

