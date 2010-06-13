#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"

ClassImp(L1Obj)
ClassImp(L1ObjColl)

std::vector<L1Obj> L1ObjColl::getL1ObjsMatched(double ptMin) const 
{
  std::vector<L1Obj> result;
  unsigned int nObj = theL1Obj.size();
  unsigned int nCom = theL1Matching.size();

  for (unsigned int i=0; i<nObj && i<nCom; ++i) 
      if (theL1Matching[i] && theL1Obj[i].pt >= ptMin) result.push_back(theL1Obj[i]);

  return result;
}
