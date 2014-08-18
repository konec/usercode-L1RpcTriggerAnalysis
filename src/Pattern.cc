#include <algorithm>
#include <fstream>
#include <sstream>

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

// Xerces include files
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOM.hpp"
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include "xercesc/sax/HandlerBase.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XercesDefs.hpp"
XERCES_CPP_NAMESPACE_USE

#include "DQMServices/ClientConfig/interface/ParserFunctions.h"
/////////////////////////

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
     (aId.region()==0 && aId.station()==4)
     ) return false;
  }
    break;
  case MuonSubdetId::DT: {
    DTphDigiSpec digi(rawId, aData.second);
    ///Select TD digis with hits in inner and outer layers 
    if (digi.bxNum() != 0 || digi.bxCnt() != 0 || digi.ts2() != 0 ||  digi.code()<4) return false;	
    break;
  }
  case MuonSubdetId::CSC: {
    CSCDetId csc(rawId);
    //if(csc.station()==1 && csc.ring()==4) return false; //Skip ME1/a due to use of ganged strips, causing problems in phi calculation
    ///////////////////
    break;
  }
  }

  int aLayer = MtfCoordinateConverter::getLayerNumber(aData.first);
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
void Pattern::dumpToXML(xercesc::DOMDocument* theDoc, 
			xercesc::DOMElement* theTopElement,			
			MtfCoordinateConverter *myPhiConverter, 
			int nPhi){

  std::ostringstream stringStr;
  myPhiConverter->setReferencePhi(0.0);

  xercesc::DOMElement *aLayer, *aHit; 
  for (auto aEntry : detsHit){
    aLayer = theDoc->createElement(qtxml::_toDOMS("Layer"));
    stringStr.str("");
    stringStr<<aEntry.first;
    aLayer->setAttribute(qtxml::_toDOMS("iLayer"), qtxml::_toDOMS(stringStr.str()));
    int iHit = 0;
    for (auto it = theData.cbegin(); it != theData.cend(); ++it){
      unsigned int iLayer = myPhiConverter->getLayerNumber(it->second.first);
      if(iLayer == aEntry.first){
	aHit = theDoc->createElement(qtxml::_toDOMS("Hit"));
	stringStr.str("");
	stringStr<<iHit;
	aHit->setAttribute(qtxml::_toDOMS("iHit"), qtxml::_toDOMS(stringStr.str()));
	stringStr.str("");
	stringStr<<myPhiConverter->convert(it->second,nPhi);
	aHit->setAttribute(qtxml::_toDOMS("iPhi"), qtxml::_toDOMS(stringStr.str()));
	aLayer->appendChild(aHit);
	++iHit;
      }
    }
  theTopElement->appendChild(aLayerr);
  }
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
