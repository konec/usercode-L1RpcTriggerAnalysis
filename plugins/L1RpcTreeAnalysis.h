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
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaTimingL1.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaMenu.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaEvent.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaHitSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/PatternManager.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaSiMuDistribution.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaOtfEff.h"

#include "TObjArray.h"


namespace edm { class Event; class EventSetup; }

class L1RpcTreeAnalysis : public edm::EDAnalyzer {
public:
  L1RpcTreeAnalysis(const edm::ParameterSet & cfg);
  virtual ~L1RpcTreeAnalysis(){}
  virtual void beginJob();
  virtual void beginRun(const edm::Run&,  const edm::EventSetup& es);
  virtual void analyze(const edm::Event&, const edm::EventSetup& es);
  virtual void endJob();

private:
  TObjArray theHistos;
  edm::ParameterSet theConfig;

  AnaMuonDistribution *theAnaMuonDistribution;
  AnaRpcVsOth         *theAnaRpcVsOth;
  AnaRpcMisc          *theAnaRpcMisc;
  AnaEff              *theAnaEff;
  AnaDet              *theAnaDet;
  AnaEmu              *theAnaEmu;
  AnaSynch            *theAnaSynch;
  AnaClu              *theAnaClu;
  AnaTimingL1         *theAnaTimingL1;
  AnaMenu             *theAnaMenu;
  AnaEvent            *theAnaEvent;

  AnaDigiSpec         *theAnaDigiSpec;
  AnaHitSpec          *theAnaHitSpec;

  PatternManager      *thePatternProducer;
  PatternManager      *thePatternProvider;

  AnaSiMuDistribution *theAnaSiMuDistribution;
  AnaOtfEff           *theAnaOtfEff;
}; 

#endif
