#ifndef UserCode_L1RpcTriggerAnalysis_AnaMenu_H
#define UserCode_L1RpcTriggerAnalysis_AnaMenu_H

class TObjArray;
class MuonObj;
class EventObj;

#include <vector>
#include <map>
#include <string>

class AnaMenu {
public:
  AnaMenu() : debug(false) {}
  void init(TObjArray& histos);
  bool filter( const EventObj* ev, const MuonObj* muon, 
            const  std::vector<std::string> & namesL1,
            const std::vector<unsigned int> & algosL1,
            const std::vector<std::string> &  namesHLT,
            const std::vector<unsigned int> & algosHLT);

  void resume(TObjArray& histos);
  bool debug;
private:
   typedef std::map< std::string, unsigned int> AlgoMap ;
   AlgoMap theAlgosL1, theAlgosHLT;
};

#endif
