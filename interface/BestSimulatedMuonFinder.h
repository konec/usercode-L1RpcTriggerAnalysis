#ifndef L1RpcTriggerAnalysis_BestSimulatedMuonFinder_H
#define L1RpcTriggerAnalysis_BestSimulatedMuonFinder_H

namespace edm { class Event; class EventSetup; }
class SimTrack; 

class BestSimulatedMuonFinder {

public:
  BestSimulatedMuonFinder( ){};
  const SimTrack *result(const edm::Event &ev, const edm::EventSetup &es);
private:
};
  
  
#endif
