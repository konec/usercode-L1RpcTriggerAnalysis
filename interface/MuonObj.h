#ifndef MuonObj_H
#define MuonObj_H
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"

class MuonObj : public TrackObj {
public:
  MuonObj():TrackObj(),theMuonBits(0) {}
  MuonObj(float pt, float eta, float phi) : TrackObj(pt,eta,phi), theMuonBits(0) {}
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
private:  
  unsigned int theMuonBits; 
public:
ClassDef(MuonObj,1)
};
#endif
