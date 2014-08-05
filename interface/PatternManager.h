#ifndef UserCode_L1RpcTriggerAnalysis_PatternManager_H
#define UserCode_L1RpcTriggerAnalysis_PatternManager_H

class TrackObj;
class HitSpecObj;
class EventObj;

#include "UserCode/L1RpcTriggerAnalysis/interface/MtfCoordinateConverter.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/GoldenPattern.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <vector>
#include <utility>
#include <map>

#include "TRandom3.h"

class RPCDigiSpec;
class GoldenPattern;
#include "xercesc/dom/DOM.hpp"

class PatternManager {

public:
  typedef std::vector< std::pair<uint32_t, uint32_t> > VDigiSpec;
  PatternManager(const edm::ParameterSet &cfg);

  ~PatternManager();
public:
  void run(const EventObj* ev,  const edm::EventSetup& es,
	   const TrackObj * simu, const HitSpecObj * hitSpec,  
	   const VDigiSpec & higSpec);
  
  L1Obj check(const EventObj* ev, const edm::EventSetup& es,
	      const TrackObj * simu, 
	      const HitSpecObj * hitSpec, 
	      const VDigiSpec & higSpec,
	      int iGranularity);
  
  void endJob();
  void beginJob();

private:

  void dumpPatterns(std::ostream &out);
void writeXML(std::string fname);
void dumpPatternsXML(xercesc::DOMDocument* theDoc, 
			    xercesc::DOMElement* theTopElement);

  edm::ParameterSet theConfig;
  edm::ESHandle<RPCGeometry> rpcGeometry;

  MtfCoordinateConverter *myPhiConverter;

  unsigned int theEvForPatCounter,  theEvUsePatCounter;
  bool phiMapDone;
  TRandom3 aRandom;

  std::map< GoldenPattern::Key, int> aCounterMap; 
  std::map< GoldenPattern::Key, GoldenPattern> theGPs; 
  std::multimap<GoldenPattern::Key, GoldenPattern::Key> theGPsPhiMap;

};
#endif
