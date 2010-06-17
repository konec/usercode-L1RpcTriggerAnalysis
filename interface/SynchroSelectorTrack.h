#ifndef SynchroSelectorTrack_H
#define SynchroSelectorTrack_H

#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroSelector.h"
class TObjArray;
class TH1F;


class SynchroSelectorTrack : public SynchroSelector {
public:
   SynchroSelectorTrack(const edm::ParameterSet&,  TObjArray&);
   virtual ~SynchroSelectorTrack(){}
   virtual bool takeIt(const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es);
   bool checkL1RpcMatching( const TrajectoryStateOnSurface & tsos,  const edm::Event&ev, const edm::EventSetup& es);
private:
  float mindetaForStudy, mindphiForStudy;
  TH1F * hDxy, *hDeltaPhi, *hDeltaEta, *hNum;
};
#endif 
