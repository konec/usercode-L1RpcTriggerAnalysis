#ifndef SynchroSelectorMuon_H
#define SynchroSelectorMuon_H

#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroSelector.h"

class TObjArray;
class TH1F;

class SynchroSelectorMuon : public SynchroSelector {
public:
   SynchroSelectorMuon(const edm::ParameterSet&, TObjArray& );
   virtual ~SynchroSelectorMuon(){}
   virtual bool takeIt(const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es);
private:
   TH1F * hDxy ;
};
#endif 
