#ifndef UserCode_1RpcTriggerAnalysis_RPCDigiSpec_H
#define UserCode_1RpcTriggerAnalysis_RPCDigiSpec_H

#include <ostream>

class RPCDigiSpec {
typedef unsigned int DigiType;
public:

  RPCDigiSpec(uint32_t aRawId, unsigned int aFromStrip, unsigned int aToStrip) 
    : theRawId(aRawId), theCodedDigi( ((aFromStrip&0xFF) << 8) | (aToStrip&0xFF) )
  { }

  RPCDigiSpec(uint32_t aRawId, DigiType  aCodedDigi)
    : theRawId(aRawId), theCodedDigi(aCodedDigi) 
  { } 

  unsigned int fromStrip() const { return ((theCodedDigi >>8) & 0xFF); } 
  unsigned int toStrip() const   { return (theCodedDigi&0xFF); }
  unsigned int halfStrip() const { return fromStrip()+toStrip(); }

  void setToStrip( unsigned int aToStrip) {
    theCodedDigi = ( (theCodedDigi&(0xFF<<8)) | (aToStrip&0xFF) );  
  }
  void setFromStrip( unsigned int aFromStrip) {
     theCodedDigi = ((theCodedDigi&0xFF) | (((aFromStrip&0xFF) << 8)) );
  }

  uint32_t rawId() const { return theRawId; }
  DigiType codedDigi() const { return theCodedDigi; }
  
private:
  uint32_t theRawId;
  DigiType theCodedDigi;

  friend std::ostream & operator << (std::ostream &out, const RPCDigiSpec &o) {
    out <<"RPC: "<<o.rawId()<< " halfStrip: "<<o.halfStrip()<<" ("<<o.fromStrip()<<":"<<o.toStrip()<<")"; 
    return out;
  }
};
#endif
