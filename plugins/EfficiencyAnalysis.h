#ifndef EfficiencyAnalysis_H
#define EfficiencyAnalysis_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

namespace edm { class Event; class EventSetup; }

class TH1D;
class TH2D;

class EfficiencyAnalysis : public edm::EDAnalyzer {
public:
  EfficiencyAnalysis(const edm::ParameterSet & cfg) : theConfig(cfg) {}
  virtual ~EfficiencyAnalysis(){}
  virtual void analyze(const edm::Event&, const edm::EventSetup&) {}
  virtual void beginJob();
  virtual void endJob(){}
private:


  TH1D* getDeltaPtFit(TH2D *hDeltaPtVsEta);

  edm::ParameterSet theConfig;

}; 

#endif
