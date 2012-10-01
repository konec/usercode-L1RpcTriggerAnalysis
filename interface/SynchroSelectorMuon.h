#ifndef SynchroSelectorMuon_H
#define SynchroSelectorMuon_H

#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroSelector.h"

class TObjArray;
class TH1F;
class TrajectoryStateOnSurface;

class SynchroSelectorMuon : public SynchroSelector {
public:
   SynchroSelectorMuon(const edm::ParameterSet&, TObjArray& );
   SynchroSelectorMuon(const edm::ParameterSet&);
   void initHistos(TObjArray& histos);
   virtual ~SynchroSelectorMuon(){}
   virtual bool takeIt(const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es);
   bool checkTraj( TrajectoryStateOnSurface & aTSOS, const RPCDetId & det, 
                   const edm::Event&ev, const edm::EventSetup& es);
private:
   TH1F * hDxy ;
};
#endif 
