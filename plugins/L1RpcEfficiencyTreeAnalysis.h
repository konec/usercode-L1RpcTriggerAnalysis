#ifndef L1RpcEfficiencyTreeAnalysis_H
#define L1RpcEfficiencyTreeAnalysis_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

namespace edm { class Event; class EventSetup; }

class L1RpcEfficiencyTreeAnalysis : public edm::EDAnalyzer {
public:
  L1RpcEfficiencyTreeAnalysis(const edm::ParameterSet & cfg) : theConfig(cfg) {}
  virtual ~L1RpcEfficiencyTreeAnalysis(){}
  virtual void analyze(const edm::Event&, const edm::EventSetup&) {}
  virtual void beginJob();
  virtual void endJob(){}

private:
  edm::ParameterSet theConfig;

}; 

#endif
