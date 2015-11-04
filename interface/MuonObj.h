#ifndef MuonObj_H
#define MuonObj_H
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include <ostream>

class MuonObj : public TrackObj {
public:
    MuonObj(float pt=0., float eta=0., float phi=0., float charge=0.) 
      : TrackObj(pt,eta,phi,charge), 
        nRPCHits(0), nDTHits(0), nCSCHits(0), nTrackerHits(0), nMatchedStations(0),
        isUnique(true), nAllMuons(0), theMuonBits(0) {}
  virtual ~MuonObj(){}
  void setBits(bool isGlobal, bool isTracker, bool isOuter, bool isCalo, bool isMatched) {
     if (isGlobal)  theMuonBits = 1 << 4;
     if (isTracker) theMuonBits |= 1<<3;
     if (isOuter)   theMuonBits |= 1<<2;
     if (isCalo)    theMuonBits |= 1<<1;
     if (isMatched) theMuonBits |= 1; 
  }
  bool isGlobal()  const { return  (theMuonBits>>4)&1 ;}  
  bool isTracker() const { return  (theMuonBits>>3)&1 ;}  
  bool isOuter()   const { return  (theMuonBits>>2)&1 ;}  
  bool isCalo()    const { return  (theMuonBits>>1)&1 ;}  
  bool isMatched() const { return   theMuonBits&1 ;}  
public:
  unsigned int nRPCHits, nDTHits, nCSCHits, nTrackerHits, nMatchedStations;
  bool         isUnique;
  unsigned int nAllMuons;
private:  
  unsigned int theMuonBits; 
public:
  //ClassDef(MuonObj,2)
};


std::ostream & operator<< (std::ostream &out, const MuonObj &o);

#endif
