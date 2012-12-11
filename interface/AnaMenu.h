#ifndef UserCode_L1RpcTriggerAnalysis_AnaMenu_H
#define UserCode_L1RpcTriggerAnalysis_AnaMenu_H

class TObjArray;
class MuonObj;
class EventObj;
class TriggerMenuResultObj;

#include <vector>
#include <map>
#include <string>
#include "FWCore/ParameterSet/interface/ParameterSet.h"


class AnaMenu {
public:
  AnaMenu(const edm::ParameterSet& cfg) : debug(false), theConfig(cfg) {}
  void init(TObjArray& histos);
  void updateMenu(const std::vector<std::string> & menuL1, const std::vector<std::string> & menuHLT);
  bool filter( const EventObj* ev, const MuonObj* muon, 
               const TriggerMenuResultObj *bitsL1,
               const TriggerMenuResultObj *bitsHLT);

  void resume(TObjArray& histos);
  bool debug;
private:
  typedef std::map< std::string, unsigned int> AlgoMap ;
  AlgoMap theAlgosL1, theAlgosHLT;
  std::vector<std::string>  theMenuL1, theMenuHLT;
  edm::ParameterSet theConfig;

};

#endif
