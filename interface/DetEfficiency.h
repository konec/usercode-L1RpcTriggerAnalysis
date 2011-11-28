#ifndef DetEfficiency_h
#define DetEfficiency_h

#include <stdint.h>
#include <cmath>
#include <string>

class DetEfficiency {
public: 
  
  DetEfficiency(uint32_t rawId) : theRawId(rawId), theHits(0), theMuons(0), theSimHits(0) {}
  double detId() const { return theRawId; }
  bool isDummy() const;
  void addHit() {theHits++; }
  void addMuon() {theMuons++; }
  void addSimHit() { theSimHits++; }
  uint32_t numberOfSimHits() const { return theSimHits; }
  uint32_t numberOfHits() const { return theHits; }
  uint32_t numberOfMuons() const { return theMuons; }
  double efficiency() const; 
  double efficiencyError() const;
  std::string print() const;
private:
  uint32_t theRawId; 
  uint32_t theHits, theMuons, theSimHits;
};
#endif
