#ifndef OMTFAnalyzer_H
#define OMTFAnalyzer_H

#include <vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"
#include "DataFormats/L1TMuon/interface/L1TRegionalMuonCandidate.h"
#include "DataFormats/L1TMuon/interface/L1TRegionalMuonCandidateFwd.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaEff.h"

#include "TObjArray.h"

namespace edm { class Event; class EventSetup; }

class OMTFAnalyzer : public edm::EDAnalyzer {
public:
  OMTFAnalyzer(const edm::ParameterSet & cfg);
  virtual ~OMTFAnalyzer(){}
  virtual void beginJob();
  virtual void beginRun(const edm::Run&,  const edm::EventSetup& es){};
  virtual void analyze(const edm::Event&, const edm::EventSetup& es);
  virtual void endJob();

private:

 template <class T> L1Obj makeL1Obj( T& t, L1Obj::TYPE type) {
    L1Obj obj;
    obj.bx = t.bx();
    obj.q  = t.quality();
    obj.pt = t.ptValue();
    obj.eta = t.etaValue();
    obj.phi = t.phiValue();
    //obj.charge = t.charge();
    obj.type = type;
    return obj;
  }

  bool getGMTReadout(const edm::Event &iEvent,
		     std::vector<L1Obj> &result,  L1Obj::TYPE type);

  bool getOMTFCandidates(const edm::Event &iEvent,
			 std::vector<L1Obj> &result);

  TObjArray theHistos;
  edm::ParameterSet theConfig;
  edm::InputTag trigOMTFCandSrc, trigGMTCandSrc, g4SimTrackSrc;

  edm::EDGetTokenT<l1t::L1TRegionalMuonCandidateCollection > inputOMTFToken;
  edm::EDGetTokenT<L1MuGMTReadoutCollection> inputGMTToken;

  AnaEff              *theAnaEff;

  unsigned int multiplyEvents;

}; 

#endif
