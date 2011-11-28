#include "UserCode/L1RpcTriggerAnalysis/interface/DetEfficiencyManager.h"
#include <algorithm>
#include <iostream>

struct LessDE_detId { 
bool operator()(const DetEfficiency& d1, const DetEfficiency& d2) { return d1.detId() < d2.detId(); }
};
struct LessDE_efficiency { 
bool operator()(const DetEfficiency& d1, const DetEfficiency& d2) { return d1.efficiency() < d2.efficiency(); }
};

void DetEfficiencyManager::addDetSimHit(uint32_t rawId)
{
  checkDet(rawId)->addSimHit();
}

void DetEfficiencyManager::addDetHit(uint32_t rawId)
{
  checkDet(rawId)->addHit();
}

void DetEfficiencyManager::addDetMuon(uint32_t rawId)
{
  checkDet(rawId)->addMuon();
}

unsigned int DetEfficiencyManager::allDetHit() const
{
  unsigned int result = 0;

  for (std::vector<DetEfficiency>::const_iterator it = theDets.begin(); it < theDets.end(); ++it) {
    if (!it->isDummy()) result += it->numberOfHits();
  }
  return result;
}

unsigned int DetEfficiencyManager::allDetMuon() const
{
  unsigned int result = 0;

  for (std::vector<DetEfficiency>::const_iterator it = theDets.begin(); it < theDets.end(); ++it) {
    if (!it->isDummy()) result += it->numberOfMuons();
  }
  return result;
}

std::vector<DetEfficiency>::iterator DetEfficiencyManager::checkDet(uint32_t rawId)
{
  DetEfficiency candid(rawId);
  std::vector<DetEfficiency>::iterator lb = 
      std::lower_bound(theDets.begin(), theDets.end(), candid, LessDE_detId()); 
  if (lb != theDets.end() && (candid.detId() == lb->detId()) ) return lb;
  else return theDets.insert(lb, candid); 
}

void DetEfficiencyManager::print() const {
  std::vector<DetEfficiency> sorted  = theDets;
  std::sort(sorted.begin(), sorted.end(), LessDE_efficiency());
  uint32_t nHits = 0;
  uint32_t nDets = 0;
  for (std::vector<DetEfficiency>::const_iterator it = sorted.begin(); it < sorted.end(); ++it) {
    if (!it->isDummy()) {
	    nHits += it->numberOfHits();
	    nDets += it->numberOfMuons();
    }
    std::cout << it->print() << std::endl; 
  }
  std::cout <<" TOTAL EFFICIENCY FROM ROLLS:  "<<nHits<<"/"<<nDets<<"  = " << nHits/double(nDets) << std::endl;
}
