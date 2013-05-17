#ifndef L1RpcTriggerAnalysis_RPCDetIdUtil_H
#define L1RpcTriggerAnalysis_RPCDetIdUtil_H

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include <iostream>
#include <ostream>
#include <cmath>
namespace edm { class EventSetup; }

class RPCDetIdUtil{
public:
  RPCDetIdUtil( const RPCDetId & rpcDet) : theRpcDet(rpcDet) { }
  RPCDetIdUtil( uint32_t rawId) : theRpcDet( RPCDetId(rawId) ) { }

  bool isBarrel() const { return (theRpcDet.region()==0); }

  unsigned int barrelLayer() const {
    return  theRpcDet.station() <=2  ? 
                2*( theRpcDet.station()-1)+ theRpcDet.layer() 
              : theRpcDet.station()+2;
  }
  unsigned int endcapLayer() const {
    return  theRpcDet.station();
  }
  unsigned int layer() const {
    return isBarrel() ? barrelLayer() : endcapLayer(); 
  }

  //WARNING - use eta of DetUnit position, not muon direction 
  unsigned int layer(float eta) const {
    if (fabs(eta) < 0.83) { 
      return barrelLayer();
    } else if ( fabs(eta) < 0.93) { 
      if ( isBarrel() && (barrelLayer() < 6)) return barrelLayer();
    } else if ( fabs(eta) < 1.04) {
      if ( isBarrel() && (barrelLayer()==1)) return 1;
      if ( isBarrel() && (barrelLayer()==2)) return 2;
      if ( isBarrel() && (barrelLayer()==3)) return 3;
      if ( isBarrel() && (barrelLayer()==4)) return 4;
//      if ( isBarrel() && (barrelLayer()==5)) return 5;
      if (!isBarrel() && endcapLayer()==1 ) return 5;
    } else if ( fabs(eta)<1.14) {
      if ( isBarrel() && (barrelLayer() < 3)) return barrelLayer();
      if (!isBarrel() && endcapLayer()==1) return 3;
      if (!isBarrel() && endcapLayer()==2) return 4;
    } else if (  fabs(eta)<1.24) {
      if ( isBarrel() && (barrelLayer() == 1)) return 1;
      if (!isBarrel()) return endcapLayer();
    } else return theRpcDet.station();
    return 0;
  }

  int part() { 
   return isBarrel() ? theRpcDet.ring() :
          (theRpcDet.region()==1 ? theRpcDet.ring()+1 : -theRpcDet.ring()-1);
  }

  RPCDetId rpdDetIt() { return theRpcDet; }

  void print(const edm::EventSetup& es);
     
private:
  RPCDetId theRpcDet;
  friend std::ostream & operator<< (std::ostream &out, const RPCDetIdUtil &o);
};
#endif
