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
bool Pattern::add(MtfCoordinateConverter *myPhiConverter,  int nPhi,
		  std::pair<uint32_t,  unsigned int > aData) {

  int myPOS = -999, myBEN = -999;
  ///Clean up digis. Remove unconnected detectors
  uint32_t rawId = aData.first;   
  DetId detId(rawId);
  if (detId.det() != DetId::Muon) 
    std::cout << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
  RPCDetId aId(rawId);    
    if(aId.region()<0 
       //|| (aId.region()==0 && aId.ring()<2) ||
       //(aId.region()==0 && aId.station()==4) ||
       //(aId.region()==1 && aId.station()==2 && aId.roll()==1) || 
       || (aId.region()==1 && aId.station()==4)
       ) return false;
    myPOS = myPhiConverter->convert(aData,nPhi);
    myBEN = -9999;
    break;
  }
  case MuonSubdetId::DT: {
    DTChamberId dt(rawId);
    DTphDigiSpec digi(rawId, aData.second);
    ///Select DT digis with hits in inner and outer layers 
    if (digi.bxNum() != 0 || digi.bxCnt() != 0 || digi.ts2() != 0 ||  digi.code()<4) return false;	
    myPOS = myPhiConverter->convert(aData,nPhi);
    myBEN = digi.phiB();
    break;
  }
  case MuonSubdetId::CSC: {
    CSCDetId csc(rawId);
    CSCDigiSpec digi(rawId, aData.second);
    //if(csc.station()==1 && csc.ring()==1) return false; //Skip ME1/A due to use of ganged strips, causing problems in phi calculation
    ///////////////////
    myPOS = myPhiConverter->convert(aData,nPhi);
    myBEN = digi.pattern();
    break;
  }
  }

  int aLayer = MtfCoordinateConverter::getLayerNumber(aData.first);
  std::pair<uint32_t,  std::pair<uint32_t, unsigned int> > aDataWithLayer(aLayer,aData);

  std::pair<uint32_t, int> aDataWithLayerNew(aLayer,myPOS);
  theDataNew.insert(aDataWithLayerNew);
  if(myBEN>-9999){
    std::pair<uint32_t, int> aDataWithLayerNew(aLayer,myBEN);
    theDataNew.insert(aDataWithLayerNew);
  }

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
  out<<"\nUnique layers: ";
  for (auto aEntry : o.detsHit) out<<aEntry.first<<" ";  
  out<<std::endl;

  return out;
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
int Pattern::deviationSum(MtfCoordinateConverter *myPhiConverter, int nPhi) const{

  int devSum = 0;
  for (auto it = theData.cbegin(); it != theData.cend(); ++it)
    devSum+=abs(myPhiConverter->convert(it->second,nPhi));

  return devSum;
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
void Pattern::print(MtfCoordinateConverter *myPhiConverter, int nPhi){

  int devSum = 0;
  std::cout <<" Pattern:  size: "<<size();
  for (auto it = theData.cbegin(); it != theData.cend(); ++it){
    DetId detId(it->second.first);
    switch (detId.subdetId()) {
    case MuonSubdetId::RPC: { std::cout << std::endl <<RPCDetId(it->second.first)<<" "<<RPCDigiSpec(it->second.first, it->second.second);  break; }
    case MuonSubdetId::DT:  { std::cout << std::endl <<DTChamberId(it->second.first)<<" "<<DTphDigiSpec(it->second.first, it->second.second); break; }
    case MuonSubdetId::CSC: { std::cout << std::endl <<CSCDetId(it->second.first)<<" "<<CSCDigiSpec(it->second.first, it->second.second);  break; }
    };
    std::cout<<" layer: "<<myPhiConverter->getLayerNumber(it->second.first)
	     <<" pos rel: "<<myPhiConverter->convert(it->second,nPhi);
    devSum+=abs(myPhiConverter->convert(it->second,nPhi));
  }
  std::cout<<" sum dev: "<<devSum;
  std::cout<<"\nUnique layers: ";
  for (auto aEntry : detsHit) std::cout<<aEntry.first<<" ";  
  std::cout<<std::endl;

}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
