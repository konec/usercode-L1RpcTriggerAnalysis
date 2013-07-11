#include "UserCode/L1RpcTriggerAnalysis/interface/Pattern.h"

#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"

/////////////////////////////////////////////////
/////////////////////////////////////////////////
uint32_t Pattern::rotateDetId(uint32_t rawId, int step){

  ///Assume 60deg steps
  while(step<0){step+=6;}

  uint32_t rawIdRotated = rawId; 

  DetId detId(rawId);
  switch (detId.subdetId()){    
  case MuonSubdetId::RPC: {
    RPCDetId rpcDet(rawId);
    ///Barrel has 30deg sectors
    if(rpcDet.region()==0) step*=2;
    RPCDetId rpcDetRotated(rpcDet.region(),
			   rpcDet.ring(),
			   rpcDet.station(), 
			   (rpcDet.sector()+step-1)%12+1,
			   rpcDet.layer(),
			   rpcDet.subsector(),
			   rpcDet.roll());
    return rpcDetRotated.rawId();      
  }
  case MuonSubdetId::DT: {
    ///Barrel has 30deg sectors
    step*=2;
    DTChamberId dtDet(rawId);
    DTChamberId dtDetRotated(dtDet.wheel(),
			     dtDet.station(),
			     (dtDet.sector()+step)%12);
    return dtDetRotated.rawId();      
    break;
  }
  case MuonSubdetId::CSC: {
    CSCDetId cscDet(rawId);
    //Most CSC chambers are 10deg wide
    int cscFactor = 6; 
    int maxChamber = 36;
    ///Some are 20deg wide
    if(cscDet.station()>1 && cscDet.ring()==1){
	cscFactor = 3;
	maxChamber = 18;
      }
    int cscStep = step*cscFactor;
    CSCDetId cscDetRotated(cscDet.endcap(),
			   cscDet.station(),
			   cscDet.ring(),
			   (cscDet.chamber()+cscStep-1)%maxChamber+1,
			   cscDet.layer());
    return cscDetRotated.rawId();      
  }
  }  
  return rawIdRotated;
}
/////////////////////////////////////////////////
/////////////////////////////////////////////////
Pattern Pattern::getRotated(int step) const{

  Pattern rotated;
  
  for (auto it = theData.cbegin(); it != theData.cend(); ++it){
    rotated.add(std::pair<uint32_t,  unsigned int >(rotateDetId(it->first,step),it->second));
  }
  return rotated;
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

Pattern Pattern::addOrCopy( std::pair<uint32_t,  unsigned int > aData){

  auto it = theData.find(aData.first);    
  if(it!= theData.cend()){
    Pattern modified =  *this;
    modified.theData[aData.first] = aData.second;
    return modified;
  }
  theData[aData.first] = aData.second;
  return Pattern();
}

void Pattern::add (std::vector<Pattern> & vpat, std::pair<uint32_t,  unsigned int > aData)
{
  //Use indexing to avoid problem with iterator invalidation afer adding new elements to vector
  uint32_t vSize = vpat.size(); 
  for(uint32_t index = 0;index<vSize;++index){
    Pattern modified =  vpat[index].addOrCopy(aData);
    if (modified && (find(vpat.begin(), vpat.end(), modified) == vpat.end()) ) vpat.push_back(modified);
  }
}


std::ostream & operator << (std::ostream &out, const Pattern &o)
{
  out <<" Pattern:  size: "<<o.size();
  for (auto it = o.theData.cbegin(); it != o.theData.cend(); ++it){
    DetId detId( it->first);
    switch (detId.subdetId()) {
    case MuonSubdetId::RPC: { out << std::endl <<RPCDetId(it->first)<<" "<<RPCDigiSpec(it->first, it->second);  break; }
    case MuonSubdetId::DT:  { out << std::endl <<DTChamberId(it->first)<<" "<<DTphDigiSpec(it->first, it->second); break; }
    case MuonSubdetId::CSC: { out << std::endl <<CSCDetId(it->first)<<" "<<CSCDigiSpec(it->first, it->second);  break; }
    };
  }
  return out;
}

