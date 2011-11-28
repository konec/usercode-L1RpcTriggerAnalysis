#ifndef L1RpcTriggerAnalysis_RPCDetIdUtil_H
#define L1RpcTriggerAnalysis_RPCDetIdUtil_H

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include <iostream>

class RPCDetIdUtil{
public:
  RPCDetIdUtil( const RPCDetId & rpcDet) : theRpcDet(rpcDet) { }

  bool isBarrel() const { return (theRpcDet.region()==0); }

  unsigned int layer() const {
    return isBarrel() ? 
        ( theRpcDet.station() <=2  ? 2*( theRpcDet.station()-1)+ theRpcDet.layer() : theRpcDet.station()+2 )
      : theRpcDet.station();
  }

  int part() { 
   return isBarrel() ? theRpcDet.ring() :
          (theRpcDet.region()==1 ? theRpcDet.ring()+1 : -theRpcDet.ring()-1);
  }

  RPCDetId rpdDetIt() { return theRpcDet; }

  void print() { 
    std::cout <<"DetId: "<<theRpcDet.rawId()
              <<" region: "<<theRpcDet.region()
              <<" layer:  "<<layer()
              <<" ring/wheel: "<<theRpcDet.ring() << std::endl; }
     
private:
  RPCDetId theRpcDet;
};
#endif
