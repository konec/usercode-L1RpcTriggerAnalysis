#ifndef DQM_RPCMonitorClient_LinkSynchroAnalysis_H
#define DQM_RPCMonitorClient_LinkSynchroAnalysis_H

#include "DQM/RPCMonitorClient/interface/RPCMonitorLinkSynchro.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "CondFormats/DataRecord/interface/RPCEMapRcd.h"
#include "TObjArray.h"
#include "SynchroSelector.h"

namespace edm { class Event; class EventSetup; }
class RPCReadOutMapping;


class LinkSynchroAnalysis : public RPCMonitorLinkSynchro {
public:
  LinkSynchroAnalysis(const edm::ParameterSet& cfg);
  virtual ~LinkSynchroAnalysis();
  virtual  const RPCRawSynchro::ProdItem& select(const RPCRawSynchro::ProdItem&, const edm::Event&, const edm::EventSetup&);
  virtual void beginJob();
  virtual void endJob();
private:
  edm::ParameterSet theAnaConfig;
  edm::ESWatcher<RPCEMapRcd> theMapWatcher;
  RPCReadOutMapping * theCabling;
  TObjArray theHistos; 
  unsigned int theEventCounter;
  unsigned int theRPCCounter;
  std::vector<SynchroSelector> theSynchroFilters;
};
#endif

