#include "UserCode/L1RpcTriggerAnalysis/interface/Pattern.h"

#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"

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

void Pattern::add (  std::vector<Pattern> & vpat, std::pair<uint32_t,  unsigned int > aData)
{
  /*
  std::vector<Pattern> copied;
  for (std::vector<Pattern>::iterator ip = vpat.begin(); ip != vpat.end(); ++ip) {
    Pattern modified =  ip->addOrCopy(aData);
    if (modified && (find(copied.begin(), copied.end(), modified) == copied.end()) ) copied.push_back(modified);
  }
  if (copied.size() != 0) vpat.insert(vpat.end(), copied.begin(), copied.end()); 
  */

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
      case MuonSubdetId::RPC: { out << std::endl <<" RPC: "<<RPCDigiSpec(it->first, it->second);  break; }
      case MuonSubdetId::DT:  { out << std::endl <<" DT:  "<<DTphDigiSpec(it->first, it->second); break; }
      case MuonSubdetId::CSC: { out << std::endl <<" CSC: "<<CSCDigiSpec(it->first, it->second);  break; }
    };
  }
  return out;
}

