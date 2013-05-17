#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

void RPCDetIdUtil::print(const edm::EventSetup &es)
{
  edm::ESHandle<RPCGeometry> rpcGeometry;
  es.get<MuonGeometryRecord>().get(rpcGeometry);

  GlobalPoint position = rpcGeometry->idToDet(theRpcDet)->position();
  
  std::cout << (*this) <<" R= "<<position.perp()
                       <<" Z= "<<position.z()
                       <<" eta="<<position.eta()
                       <<" consecutiveLayer: "<<layer(position.eta())
                       << std::endl;
}

std::ostream & operator<< (std::ostream &out, const RPCDetIdUtil &o) {
  out <<"DetId: "<<o.theRpcDet.rawId()
              <<" region: "<<o.theRpcDet.region()
              <<" layer:  "<<o.layer()
              <<" ring/wheel: "<<o.theRpcDet.ring();
  return out;
}

