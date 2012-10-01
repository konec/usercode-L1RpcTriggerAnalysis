#include "UserCode/L1RpcTriggerAnalysis/interface/ConverterRPCRawSynchroSynchroCountsObj.h"

std::vector<SynchroCountsObj> ConverterRPCRawSynchroSynchroCountsObj::toSynchroObj(const RPCRawSynchro::ProdItem & raw) {
  std::vector<SynchroCountsObj> result;
  for (RPCRawSynchro::ProdItem::const_iterator it = raw.begin(); it != raw.end(); ++it) {
    SynchroCountsObj data;
     data.dccId = it->first.dccId;
     data.dccInputChannelNum = it->first.dccInputChannelNum;
     data.tbLinkInputNum = it->first.tbLinkInputNum;
     data.lbNumInLink = it->first.lbNumInLink;
     data.bx = it->second ;
    result.push_back(data);
  }
  return result;
}

RPCRawSynchro::ProdItem ConverterRPCRawSynchroSynchroCountsObj::toRawSynchro( const std::vector<SynchroCountsObj> & counts) {
  RPCRawSynchro::ProdItem result;
  for (std::vector<SynchroCountsObj>::const_iterator it = counts.begin(); it != counts.end(); ++it) {
    LinkBoardElectronicIndex lbIndex = {it->dccId, it->dccInputChannelNum,it->tbLinkInputNum, it->lbNumInLink};
    result.push_back( std::make_pair( lbIndex, it->bx) );
  }
  return result;
}


