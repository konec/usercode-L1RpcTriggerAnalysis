#ifndef DetEfficiencyManager_h
#define DetEfficiencyManager_h

#include <stdint.h>
#include <cmath>
#include <string>
#include <vector>
#include "UserCode/L1RpcTriggerAnalysis/interface/DetEfficiency.h"

class DetEfficiencyManager {
public: 
  DetEfficiencyManager() {}
  void addDetHit(uint32_t rawId);
  void addDetSimHit(uint32_t rawId);
  void addDetMuon(uint32_t rawId);
  const std::vector<DetEfficiency> & dets() const { return theDets; }  
  void print() const;
  unsigned int allDetHit() const;
  unsigned int allDetMuon() const;
private:
  std::vector<DetEfficiency> theDets;
  std::vector<DetEfficiency>::iterator checkDet(uint32_t rawId);
	
};

#endif
