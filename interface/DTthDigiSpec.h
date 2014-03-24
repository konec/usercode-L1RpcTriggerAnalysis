
#ifndef UserCode_1RpcTriggerAnalysis_DTthDigiSpec_H
#define UserCode_1RpcTriggerAnalysis_DTthDigiSpec_H

#include "DataFormats/L1DTTrackFinder/interface/L1MuDTChambThDigi.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"

#include <iostream>

class DTthDigiSpec {
typedef unsigned int DigiType;

public:

  DTthDigiSpec(uint32_t aRawId, DigiType aCodedDigi) : theRawId(aRawId), theCodedDigi(aCodedDigi) {}

  DTthDigiSpec(const L1MuDTChambThDigi& digi) : theRawId(0), theCodedDigi(0){

    theRawId = DTChamberId(digi.whNum(), digi.stNum(), digi.scNum()).rawId();

    theCodedDigi = 0;  
    for(int i=0;i<7;++i) theCodedDigi = ( theCodedDigi  | (digi.position(i) & 0x1)<<i );
    theCodedDigi = theCodedDigi<<7;
    for(int i=0;i<7;++i) theCodedDigi = ( theCodedDigi  | (digi.quality(i) & 0x1)<<i );

}

  int position(unsigned int iPos) const {
return (theCodedDigi >> (7+iPos)) & 0x1;
}
    
  int quality(unsigned iPos) const {
return (theCodedDigi >> iPos) & 0x1;
  }

  uint32_t rawId() const { return theRawId; } 
  DigiType codedDigi()  const { return theCodedDigi; }

private:
  uint32_t theRawId;
  DigiType theCodedDigi;
  friend std::ostream & operator << (std::ostream &out, const DTthDigiSpec &o) {
    out <<"DTP: "<<o.theRawId<<"   wheel: "<<DTChamberId(o.theRawId).wheel()
	<<", station: "<<DTChamberId(o.theRawId).station()
	<<", sector: "<<DTChamberId(o.theRawId).sector()
	<<", pos(q) ";
    for(int i=0;i<7;++i) 
      out<<"(i="<<i<<") "<<o.position(i)<<"("<<o.quality(i)<<") ";
    return out;
  }
                                            
};
#endif

