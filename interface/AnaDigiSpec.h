#ifndef UserCode_L1RpcTriggerAnalysis_AnaDigiSpec_H
#define UserCode_L1RpcTriggerAnalysis_AnaDigiSpec_H

class TrackObj; 
class HitSpecObj;
class TObjArray;
class EventObj;
class TH2D;

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <vector>
#include <utility>
#include <map>




class  AnaDigiSpec {
public:
  typedef std::vector< std::pair<uint32_t, uint32_t> > VDigiSpec;
  AnaDigiSpec(const edm::ParameterSet & cfg) : theConfig(cfg) {}
  void init(TObjArray& histos);
  void run(const EventObj* ev, const TrackObj * simu, const HitSpecObj * hitSpec,  const VDigiSpec & higSpec); 
  void resume(TObjArray& histos);
private:
  edm::ParameterSet theConfig;
  typedef std::map< uint32_t, TH2D* > HMap;
  HMap  theCscPosB, theCscBendB, theDtPosB, theDtBendB, theRpcBPosB, theRpcEPosB; 
  HMap  theCscPosE, theCscBendE, theDtPosE, theDtBendE, theRpcBPosE, theRpcEPosE; 
private:
  typedef std::vector<TH2D*> VHisto;
  VHisto topN(const HMap & hmap, unsigned int n) const;
};
#endif

