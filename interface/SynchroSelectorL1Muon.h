#ifndef SynchroSelectorL1Muon_H
#define SynchroSelectorL1Muon_H

#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroSelector.h"
#include <vector>
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"

class TObjArray;
class TH1F;


class SynchroSelectorL1Muon : public SynchroSelector {
public:
   SynchroSelectorL1Muon(const edm::ParameterSet&,  TObjArray&);
   virtual ~SynchroSelectorL1Muon(){}
   virtual void update(const edm::Event&ev, const edm::EventSetup& es);
   virtual bool takeIt(const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es);
private:
  std::vector<L1MuRegionalCand> theL1Muons;  
  TH1F *hDeltaEta, *hDeltaPhi;
};
#endif

