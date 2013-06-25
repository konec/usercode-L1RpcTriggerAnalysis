#include "UserCode/L1RpcTriggerAnalysis/interface/Pattern.h"

#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"

bool Pattern::operator==(const Pattern& o) const
{
  unsigned int thissize = theData.size(); 
 if (thissize != o.size()) return false;

  for (unsigned int idx=0; idx<thissize; idx++) {
     if (theData[idx].first != o.theData[idx].first) return false;
     if (theData[idx].second != o.theData[idx].second) return false; 
  } 
 
  return true;
}

Pattern Pattern::addOrCopy( std::pair<uint32_t,  unsigned int > aData)
{

  for (unsigned int idx=0; idx < theData.size(); ++idx) {
    if (theData[idx].first == aData.first) {
      Pattern modified =  *this;
      modified.theData[idx].second = aData.second;
      return modified;
//      if (*this == modified) return Pattern(); // nothing to do 
//      else  return modified;                   // really duplicate
    }
  }
  theData.push_back(aData);
  return Pattern();
}

void Pattern::add (  std::vector<Pattern> & vpat, std::pair<uint32_t,  unsigned int > aData)
{
  std::vector<Pattern> copied;
  for (std::vector<Pattern>::iterator ip = vpat.begin(); ip != vpat.end(); ++ip) {
    Pattern modified =  ip->addOrCopy(aData);
    if (modified && (find(copied.begin(), copied.end(), modified) == copied.end()) ) copied.push_back(modified);
  }
  if (copied.size() != 0) vpat.insert(vpat.end(), copied.begin(), copied.end()); 
}



std::ostream & operator << (std::ostream &out, const Pattern &o)
{
  out <<" Pattern:  size: "<<o.size();
  for (Pattern::DataType::const_iterator it=o.theData.begin(); it!= o.theData.end(); it++) {
    DetId detId( it->first);
    switch (detId.subdetId()) {
      case MuonSubdetId::RPC: { out << std::endl <<" RPC: "<<RPCDigiSpec(it->first, it->second);  break; }
      case MuonSubdetId::DT:  { out << std::endl <<" DT:  "<<DTphDigiSpec(it->first, it->second); break; }
      case MuonSubdetId::CSC: { out << std::endl <<" CSC: "<<CSCDigiSpec(it->first, it->second);  break; }
    };
  }
  return out;
}

