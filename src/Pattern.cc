#include "UserCode/L1RpcTriggerAnalysis/interface/Pattern.h"

#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"


bool Pattern::add ( std::pair<uint32_t,  unsigned int > aData)
{
  for (DataType::const_iterator it=theData.begin(); it!= theData.end(); it++) {
    if (it->first == aData.first) {
      return false;
    }
  }
  theData.push_back(aData);
  return true;
}

bool Pattern::add (  std::vector<Pattern> & vpat, std::pair<uint32_t,  unsigned int > aData)
{
  bool allOK = true;
  std::vector<Pattern> copied;
  for (std::vector<Pattern>::iterator ip = vpat.begin(); ip != vpat.end(); ++ip) {
    if (! ip->add(aData) ) {
      allOK = false;
      Pattern modified = *ip; 
      for (auto ic =modified.theData.begin(); ic!=modified.theData.end(); ic++) {
        if (ic->first==aData.first) {ic->second = aData.second; break; }
      } 
      copied.push_back(modified);
    }
  }
  if (copied.size() != 0) vpat.insert(vpat.end(), copied.begin(), copied.end()); 
  return allOK; 
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

