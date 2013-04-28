#ifndef L1RpcTriggerAnalysis_BestMuonFinder_H
#define L1RpcTriggerAnalysis_BestMuonFinder_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Provenance/interface/RunID.h"
#include "DataFormats/Provenance/interface/EventID.h"

namespace edm { class Event; class EventSetup; }
namespace reco { class Muon; }

class TH1D;
class TH2D;
class TObjArray;

class BestMuonFinder {

public:
  BestMuonFinder( const edm::ParameterSet& cfg);
  const reco::Muon* result( const edm::Event &ev, const edm::EventSetup &es) { run(ev,es); return theMuon; }
  bool isUnique( const edm::Event &ev, const edm::EventSetup &es) { run(ev,es); return theUnique;}
  unsigned int numberOfAllMuons( const edm::Event &ev, const edm::EventSetup &es) { run(ev,es); return theAllMuons; }
  void initHistos( TObjArray & histos);

  unsigned int numberOfValidMuonRPCHits() const { return theRPCHits; }
  unsigned int numberOfValidMuonDTHits()  const { return theDTHits; }
  unsigned int numberOfValidMuonCSCHits() const { return theCSCHits; }
  unsigned int numberOfValidTrackerHits() const { return theTrackerHits; } 

private:
  bool run(const edm::Event &ev, const edm::EventSetup &es);

private:
  edm::EventNumber_t lastEvent;
  edm::RunNumber_t   lastRun;
  edm::ParameterSet  theConfig;

  bool theUnique;
  unsigned int theAllMuons;
  const reco::Muon* theMuon;

  unsigned int theTrackerHits, theRPCHits, theDTHits, theCSCHits;

  TH1D *hMuChi2Tk, *hMuChi2Gl, *hMuNHitsTk ;
  TH2D *hMuPtVsEta, *hMuHitsRPCvsCSC, *hMuHitsRPCvsDT; 
  TH1D *hMuonPt_BMF, *hMuonEta_BMF, *hMuonPhi_BMF;


};
#endif
