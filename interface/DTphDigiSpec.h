#ifndef UserCode_1RpcTriggerAnalysis_DTphDigiSpec_H
#define UserCode_1RpcTriggerAnalysis_DTphDigiSpec_H

#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambPhDigi.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"

class DTphDigiSpec {
typedef unsigned int DigiType;

public:

  DTphDigiSpec(uint32_t aRawId, DigiType aCodedDigi) : theRawId(aRawId), theCodedDigi(aCodedDigi) {}

  DTphDigiSpec(const L1MuDTChambPhDigi& digi) : theRawId(0), theCodedDigi(0)
 {
    theRawId = DTChamberId( digi.whNum(), digi.stNum(), digi.scNum()).rawId();

    bool mphi = (digi.phi() <0); 
    unsigned int absphi = abs( digi.phi());
    bool mphiB = (digi.phiB() <0);
    unsigned int absphiB = abs( digi.phiB());
    unsigned int quality = digi.code();
    unsigned int bx = 7+digi.bxNum();
    bool bxCnt  = (digi.BxCnt()!=0);
    bool ts2    = (digi.Ts2Tag() != 0);

    theCodedDigi = ( 0 | (mphi & 0x1) );
    theCodedDigi = ( (theCodedDigi<<11) | (absphi  & 0x7FF) );
    theCodedDigi = ( (theCodedDigi<<1)  |  (mphiB  & 0x1) ); 
    theCodedDigi = ( (theCodedDigi<<10) | (absphiB & 0x3FF) );
    theCodedDigi = ( (theCodedDigi<<3)  | (quality & 0x7) );
    theCodedDigi = ( (theCodedDigi<<4)  | (bx      & 0xF) );
    theCodedDigi = ( (theCodedDigi<<1)  | (bxCnt   & 0x1) );
    theCodedDigi = ( (theCodedDigi<<1)  | (ts2     & 0x1) );
  }

  int phi() const {
    int aphi = (theCodedDigi >> 20) & 0x7FF;
    bool minus = (theCodedDigi >> 31) & 0x1;
    return (minus) ? -aphi : aphi;
  }

  int phiB() const {
    int aphiB = (theCodedDigi >> 9) & 0x3FF;
    bool minus = (theCodedDigi >> 19) & 0x1;
    return (minus) ? -aphiB : aphiB;
  }

  int code()  const { return  (theCodedDigi>>6)&0x7; }
  int bxNum() const { return  ((theCodedDigi>>2)&0xF) -7; } 
  bool bxCnt() const { return (theCodedDigi>>1)&0x1; }
  bool ts2() const { return   theCodedDigi&0x1; }

  uint32_t rawId() const { return theRawId; } 
  DigiType codedDigi()  const { return theCodedDigi; }

private:
  uint32_t theRawId;
  DigiType theCodedDigi;
  friend std::ostream & operator << (std::ostream &out, const DTphDigiSpec &o) {
    out <<"DTP: "<<o.theRawId<<"   wheel: "<<DTChamberId(o.theRawId).wheel()
                             <<", station: "<<DTChamberId(o.theRawId).station()
                             <<", sector: "<<DTChamberId(o.theRawId).sector()
                             <<", phi: "<<o.phi()
                             <<", phiB: "<< o.phiB()
                 <<", q,bx,bxCnt,ts2: "<<o.code()<<" "<<o.bxNum()<<" "<<o.bxCnt()<<" "<<o.ts2();
    return out;
  }
                                            
};
#endif

