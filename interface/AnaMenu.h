#ifndef UserCode_L1RpcTriggerAnalysis_AnaMenu_H
#define UserCode_L1RpcTriggerAnalysis_AnaMenu_H

class TObjArray;
class MuonObj;
class EventObj;
class TriggerMenuResultObj;

#include <vector>
#include <map>
#include <string>

class AnaMenu {
public:
  AnaMenu() : debug(false) {}
  void init(TObjArray& histos);
  bool filter( const EventObj* ev, const MuonObj* muon, 
               const TriggerMenuResultObj *bitsL1,
               const TriggerMenuResultObj *bitsHLT);

  void resume(TObjArray& histos);
  bool debug;
private:
   typedef std::map< std::string, unsigned int> AlgoMap ;
   AlgoMap theAlgosL1, theAlgosHLT;
   std::vector<std::string>  namesL1, namesHLT;
};

#endif
