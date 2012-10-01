#ifndef UserCode_L1RpcTriggerAnalysis_AnaSynch_H
#define UserCode_L1RpcTriggerAnalysis_AnaSynch_H

class TObjArray;
class TH1D;
class MuonObj;
class EventObj;

#include <vector>
#include <map>
#include <string>
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CondFormats/RPCObjects/interface/LinkBoardElectronicIndex.h"
#include "DataFormats/RPCDigi/interface/RPCRawSynchro.h"
#include "DQM/RPCMonitorClient/interface/RPCLinkSynchroStat.h"

namespace edm { class EventSetup; class Run;}



class AnaSynch {
public:
  AnaSynch();
  void init(TObjArray& histos);
  void run( const EventObj* event, const MuonObj* muon, const  RPCRawSynchro::ProdItem & synchro);
  void beginRun(const edm::Run& ru, const edm::EventSetup& es);
  void endJob();
private:
  RPCLinkSynchroStat theSynchroStat;
};

#endif

