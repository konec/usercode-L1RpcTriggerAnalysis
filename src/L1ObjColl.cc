#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include <iostream>

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

std::vector<L1Obj> L1ObjColl::getL1ObjsSelected(
    bool requireMatched, 
    bool requireNonMatched, 
    double ptMin, double ptMax,
    int bxMin, int bxMax,
    double etaMin, double etaMax, 
    double phiMin, double phiMax,
    int qMin, int qMax) const 
{
  std::vector<L1Obj> result;
  unsigned int nMS = theL1Matching.size();
  for (unsigned int i=0; i<theL1Obj.size(); ++i) {
	  bool isMatched = ( (i<nMS) && theL1Matching[i] );
	  if (requireMatched && !isMatched ) continue;
	  if (requireNonMatched && isMatched) continue;
	  if (theL1Obj[i].pt < ptMin) continue;
	  if (theL1Obj[i].pt > ptMax) continue;
	  if (theL1Obj[i].bx < bxMin) continue;
	  if (theL1Obj[i].bx > bxMax) continue;
	  if (theL1Obj[i].eta < etaMin) continue;
	  if (theL1Obj[i].eta > etaMax) continue;
	  if (theL1Obj[i].phi < phiMin) continue;
	  if (theL1Obj[i].phi > phiMax) continue;
	  if (theL1Obj[i].q < qMin) continue;
	  if (theL1Obj[i].q > qMax) continue;
	  result.push_back(theL1Obj[i]);
  }
  return result;
}

void L1ObjColl::print() const 
{
  for (unsigned int i=0; i<theL1Obj.size(); ++i) {
    std::cout <<"("<<i<<")";theL1Obj[i].print();
    if (theL1Matching.size()>i) std::cout<<" matched: "<<theL1Matching[i];
    std::cout <<std::endl; 
  }
}
