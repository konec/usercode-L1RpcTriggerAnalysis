#include <algorithm>

#include "UserCode/L1RpcTriggerAnalysis/interface/Pattern.h"

#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MtfCoordinateConverter.h"


/////////////////////////////////////////////////
/////////////////////////////////////////////////
bool Pattern::add(std::pair<uint32_t,  unsigned int > aData) {

  ///Clean up digis. Remove unconnected detectors
  uint32_t rawId = aData.first;   
  DetId detId(rawId);
  if (detId.det() != DetId::Muon) 
    std::cout << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
  RPCDetId aId(rawId);    
    if(aId.region()<0 || 
       (aId.region()==0 && aId.ring()<2) ||
       (aId.region()==0 && aId.station()==4) ||
       (aId.region()==1 && aId.station()==2 && aId.roll()==1) || 
       (aId.region()==1 && aId.ring()<3)) return false;           
  }
    break;
  case MuonSubdetId::DT: {
    DTChamberId dt(rawId);
    DTphDigiSpec digi(rawId, aData.second);
    ///Select TD digis with hits in inner and outer layers 
    if (digi.bxNum() != 0 || digi.bxCnt() != 0 || digi.ts2() != 0 ||  digi.code()<4) return false;	
    //if(dt.wheel()<2) return false;       
    break;
  }
  case MuonSubdetId::CSC: {
    CSCDetId csc(rawId);
    break;
  }
    ///////////////////
  }

  int aLayer = MtfCoordinateConverter::getLayerNumber(aData.first)+100*detId.subdetId();
  std::pair<uint32_t,  std::pair<uint32_t, unsigned int> > aDataWithLayer(aLayer,aData);

  theData.insert(aDataWithLayer);
  int aCounts = theData.count(aLayer);
  if(aCounts>1) multipleHits = true;
  return (aCounts==1);
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
void Pattern::makeHitDetsList(){

  unique_copy(begin(theData),
              end(theData),
              back_inserter(detsHit),
              [](const DataType::value_type &entry1,
                 const DataType::value_type &entry2) {
		return (entry1.first == entry2.first);
	      } );
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
bool Pattern::operator==(const Pattern& o) const{
  unsigned int thissize = theData.size();
  if (thissize != o.size()) return false;

  for (auto it = theData.cbegin(); it != theData.cend(); ++it){
    auto itComp = o.theData.find(it->first);    
    if(itComp==o.theData.cend()) return false;
    if(itComp->second!=it->second) return false;    
  }

  return true;
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
std::ostream & operator << (std::ostream &out, const Pattern &o)
{
  out <<" Pattern:  size: "<<o.size();
  for (auto it = o.theData.cbegin(); it != o.theData.cend(); ++it){
    DetId detId(it->second.first);
    switch (detId.subdetId()) {
    case MuonSubdetId::RPC: { out << std::endl <<RPCDetId(it->second.first)<<" "<<RPCDigiSpec(it->second.first, it->second.second);  break; }
    case MuonSubdetId::DT:  { out << std::endl <<DTChamberId(it->second.first)<<" "<<DTphDigiSpec(it->second.first, it->second.second); break; }
    case MuonSubdetId::CSC: { out << std::endl <<CSCDetId(it->second.first)<<" "<<CSCDigiSpec(it->second.first, it->second.second);  break; }
    };
  }
  out<<"Unique layers: ";
  for (auto aEntry : o.detsHit) out<<aEntry.first<<" ";  
  out<<std::endl;

  return out;
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
