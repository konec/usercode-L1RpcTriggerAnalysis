#ifndef UserCode_L1RpcTriggerAnalysis_PatternManager_H
#define UserCode_L1RpcTriggerAnalysis_PatternManager_H

class TrackObj;
class HitSpecObj;
class EventObj;

#include "FWCore/Framework/interface/EventSetup.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/GoldenPattern.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <vector>
#include <utility>
#include <map>


class PatternManager {

public:
  typedef std::vector< std::pair<uint32_t, uint32_t> > VDigiSpec;
  PatternManager(const edm::ParameterSet &cfg);

  ~PatternManager();
public:
  void run(const EventObj* ev,  const edm::EventSetup& es,
	   const TrackObj * simu, const HitSpecObj * hitSpec,  
	   const VDigiSpec & higSpec);

  void makePhiMap(const edm::EventSetup& es);

  L1Obj check(const EventObj* ev, const TrackObj * simu, const HitSpecObj * hitSpec,  const VDigiSpec & higSpec);
  L1Obj checkNew(const EventObj* ev, const TrackObj * simu, 
		 const HitSpecObj * hitSpec,  const HitSpecObj * hitSpecSt1,  
		 const VDigiSpec & higSpec);
  void endJob();
  void beginJob();

private:
  edm::ParameterSet theConfig;

 unsigned int theEvForPatCounter,  theEvUsePatCounter;
 bool phiMapDone;

  std::map< GoldenPattern::Key, GoldenPattern> theGPs; 
  std::multimap<GoldenPattern::Key, GoldenPattern::Key> theGPsPhiMap;

  std::map<int,int> stripToPhiBarrel, stripToPhiEndcap;


};
#endif
