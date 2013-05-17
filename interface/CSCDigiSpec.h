#ifndef UserCode_1RpcTriggerAnalysis_CSCDigiSpec_H
#define UserCode_1RpcTriggerAnalysis_CSCDigiSpec_H

#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigi.h"
#include <ostream>


class CSCDigiSpec {
typedef unsigned int DigiType;

public:

  CSCDigiSpec(uint32_t aRawId, DigiType aCodedDigi) 
    : theRawId(aRawId), theCodedDigi(aCodedDigi) {}

  CSCDigiSpec(uint32_t aRawId, const CSCCorrelatedLCTDigi & digi)
    : theRawId(aRawId), theCodedDigi(0)
  {
    setQuality(digi.getQuality());
    setStrip(digi.getStrip()); 
    setPattern(digi.getPattern()); 
    setKeywire(digi.getKeyWG()); 
  }

  CSCDigiSpec(uint32_t aRawId,  
              unsigned int aStrip, unsigned int aPattern,
              unsigned int aQuality, unsigned int aKeyWG)
    : theRawId(aRawId), theCodedDigi(0) 
  { setQuality(aQuality); setStrip(aStrip); setPattern(aPattern); setKeywire(aKeyWG); }

  void setQuality( unsigned int aQuality) {
    theCodedDigi = ( (theCodedDigi&(0xFFFFF0)) | (aQuality&0xF) );
  }
  void setPattern( unsigned int aPattern) {
    theCodedDigi = ( (theCodedDigi&(0xFFFF0F)) | ((aPattern&0xF)<<4) ); 
  }
  void setStrip( unsigned int aStrip) {
    theCodedDigi = ( (theCodedDigi&(0xFF00FF)) | ((aStrip&0xFF)<<8) );
  }

  void setKeywire(  unsigned int aKeyWG) {
     theCodedDigi = ( (theCodedDigi&(0xFFFFFF)) | ((aKeyWG&0xFF)<<16) ); 
  }

  unsigned int quality() const { return (theCodedDigi&0xF); }
  unsigned int pattern() const { return ((theCodedDigi>>4)&0xF); }
  unsigned int strip() const {   return ((theCodedDigi>>8)&0xFF);} 
  unsigned int keywire() const { return ((theCodedDigi>>16)&0xFF);}
     
  uint32_t rawId() const { return theRawId; }
  DigiType codedDigi() const { return theCodedDigi; } 
private:

  uint32_t theRawId;
  DigiType theCodedDigi;

  friend std::ostream & operator << (std::ostream &out, const CSCDigiSpec &o) {
    out <<"CSC: "<< o.theRawId <<" Quality = "<< o.quality()
                               <<" Key Wire = "<< o.keywire()
                               <<" Strip = "<< o.strip()
                               <<" Pattern = "<<o.pattern();
    return out;
  }
};
#endif

