#ifndef ConverterRPCRawSynchroSynchroCountsObj_H
#define ConverterRPCRawSynchroSynchroCountsObj_H

#include "CondFormats/RPCObjects/interface/LinkBoardElectronicIndex.h"
#include "DataFormats/RPCDigi/interface/RPCRawSynchro.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObj.h"

namespace ConverterRPCRawSynchroSynchroCountsObj {

  std::vector<SynchroCountsObj> toSynchroObj(const RPCRawSynchro::ProdItem & raw);
  RPCRawSynchro::ProdItem toRawSynchro( const std::vector<SynchroCountsObj> & counts);

}

#endif

