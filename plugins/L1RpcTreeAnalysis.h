#ifndef L1RpcTreeAnalysis_H
#define L1RpcTreeAnalysis_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/AnaEff.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaDet.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaEmu.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaRpcMisc.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaRpcVsOth.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaMuonDistribution.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaSynch.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaClu.h"

#include "TObjArray.h"


namespace edm { class Event; class EventSetup; }

class L1RpcTreeAnalysis : public edm::EDAnalyzer {
public:
  L1RpcTreeAnalysis(const edm::ParameterSet & cfg) : theConfig(cfg) {}
  virtual ~L1RpcTreeAnalysis(){}
  virtual void beginJob();
  virtual void beginRun(const edm::Run&,  const edm::EventSetup& es);
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();

private:
  edm::ParameterSet theConfig;
  TObjArray theHistos;

  AnaMuonDistribution theAnaMuonDistribution;
  AnaRpcVsOth         theAnaRpcVsOth;
  AnaRpcMisc          theAnaRpcMisc;
  AnaEff              theAnaEff;
  AnaDet              theAnaDet;
  AnaEmu              theAnaEmu;
  AnaSynch            theAnaSynch;
  AnaClu              theAnaClu;


}; 

#endif
