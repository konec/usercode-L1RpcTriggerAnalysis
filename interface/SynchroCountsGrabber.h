#ifndef UserCode_L1RpcTriggerAnalysis_SynchroCountsGrabber_H
#define UserCode_L1RpcTriggerAnalysis_SynchroCountsGrabber_H

#include "DQM/RPCMonitorClient/interface/RPCLinkSynchroStat.h"
#include "CondFormats/RPCObjects/interface/LinkBoardElectronicIndex.h"
#include "DataFormats/RPCDigi/interface/RPCRawSynchro.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "CondFormats/DataRecord/interface/RPCEMapRcd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroSelectorMuon.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/RPCDigi/interface/RPCRawSynchro.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"





namespace edm { class Event; class EventSetup; class Run;}
namespace reco { class Muon; }

class TObjArray;

class SynchroCountsGrabber {
public:
   SynchroCountsGrabber(const edm::ParameterSet& cfg);
   ~SynchroCountsGrabber();
   RPCRawSynchro::ProdItem counts(const edm::Event &ev, const edm::EventSetup &es);
   RPCRawSynchro::ProdItem counts(const edm::Event &ev, const edm::EventSetup &es, float eta, float phi);
   void setMuon(const reco::Muon *aMuon) { theMuon = aMuon; }
   void initHistos(TObjArray & histos) {theSelector.initHistos(histos); }
private:
  edm::ESWatcher<RPCEMapRcd> theMapWatcher;
  const RPCReadOutMapping * theCabling;
  const reco::Muon * theMuon;
  SynchroSelectorMuon theSelector;
  bool deltaR_MuonToDetUnit_cutoff, checkInside;
  bool theNoSynchroWarning;

//  edm::EDGetTokenT<RPCRawSynchro::ProdItem> rpcRawSynchroProdItemTag_;
};
#endif
