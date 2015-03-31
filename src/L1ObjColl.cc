#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include <iostream>

ClassImp(L1Obj)
ClassImp(L1ObjColl)


L1ObjColl L1ObjColl::selectByType( TYPE t) const
{
  L1ObjColl result;
  for (unsigned int i=0; i<theL1Obj.size(); i++) if (theL1Obj[i].type == t) result.push_back( theL1Obj[i], theL1Matching[i], theDeltaR[i]); 
  return result;
}

L1ObjColl L1ObjColl::selectByPt( double ptMin, double ptMax) const
{
  L1ObjColl result;
  const double epsilon = 1.e-9;
  for (unsigned int i=0; i<theL1Obj.size(); i++) if ( theL1Obj[i].pt > (ptMin-epsilon) && theL1Obj[i].pt < (ptMax+epsilon) ) result.push_back( theL1Obj[i], theL1Matching[i], theDeltaR[i]);
  return result;
}
L1ObjColl L1ObjColl::selectByPtMin( double ptMin) const
{
  L1ObjColl result;
  const double epsilon = 1.e-9;
  for (unsigned int i=0; i<theL1Obj.size(); i++) if ( theL1Obj[i].pt > (ptMin-epsilon) ) result.push_back( theL1Obj[i], theL1Matching[i], theDeltaR[i]);
  return result;
}

L1ObjColl L1ObjColl::selectByEta( double etaMin, double etaMax) const
{
  L1ObjColl result;
  for (unsigned int i=0; i<theL1Obj.size(); i++) if ( etaMin < theL1Obj[i].eta && theL1Obj[i].eta <= etaMax)  result.push_back( theL1Obj[i], theL1Matching[i], theDeltaR[i]);
  return result;
}

L1ObjColl L1ObjColl::selectByBx( int bxMin, int bxMax) const
{
  L1ObjColl result;
  for (unsigned int i=0; i<theL1Obj.size(); i++) if ( bxMin <=  theL1Obj[i].bx &&  theL1Obj[i].bx <= bxMax) result.push_back( theL1Obj[i], theL1Matching[i], theDeltaR[i]);
  return result;
}

L1ObjColl L1ObjColl::selectByQuality( int qMin, int qMax) const
{
  L1ObjColl result;
  for (unsigned int i=0; i<theL1Obj.size(); i++) if ( qMin <=  theL1Obj[i].q &&  theL1Obj[i].q <= qMax) result.push_back( theL1Obj[i], theL1Matching[i], theDeltaR[i]);
  return result;
}

L1ObjColl L1ObjColl::selectByMatched() const
{
  L1ObjColl result;
  for (unsigned int i=0; i<theL1Obj.size(); i++) if (theL1Matching[i]) result.push_back( theL1Obj[i], theL1Matching[i], theDeltaR[i]);
  return result;
}

L1ObjColl L1ObjColl::selectByDeltaR( double deltaRMax) const
{
  L1ObjColl result;
  for (unsigned int i=0; i<theL1Obj.size(); i++) if ( theDeltaR[i] <= deltaRMax) result.push_back( theL1Obj[i], theL1Matching[i], theDeltaR[i]);
  return result;
}

L1ObjColl L1ObjColl::operator+(const L1ObjColl &o) const
{
  L1ObjColl result = *this;
  for (unsigned int i=0; i<o.theL1Obj.size(); i++) result.push_back( o.theL1Obj[i], o.theL1Matching[i], o.theDeltaR[i]);
  return result;
}

void L1ObjColl::push_back(const L1Obj & obj, bool match, double deltaR)
{
  theL1Obj.push_back(obj);
  theL1Matching.push_back(match);
  theDeltaR.push_back(deltaR);
}





std::vector<L1Obj> L1ObjColl::typeSelector(const  std::vector<L1Obj> & col,  
 TYPE t1, TYPE t2, TYPE t3, TYPE t4)
{
  std::vector<L1Obj> result;
  for (std::vector<L1Obj>::const_iterator it= col.begin(); it != col.end(); ++it) {
    if ( it->type == t1 || it->type == t2 || it->type == t3 ||it->type == t4 ) result.push_back(*it); 
  }
  return result; 
}
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


std::ostream & operator<< (std::ostream &out, const L1ObjColl &col)
 {
  for (unsigned int i=0; i< col.theL1Obj.size(); ++i) {
    out <<"("<<i<<")"<<col.theL1Obj[i];
    out <<" matched: "<< col.theL1Matching[i]<<" deltaR: "<< col.theDeltaR[i];
    if (i !=  col.theL1Obj.size()-1) out <<std::endl;
  }
   return out;
 }

std::ostream & operator<< (std::ostream &out, const L1Obj &o)
{
  out<<"L1Obj: ";
  switch (o.type) {
    case L1Obj::RPCb:     { out <<"RPCb    "; break; }
    case L1Obj::RPCf:     { out <<"RPCf    "; break; }
    case L1Obj::DT:       { out <<"DT      "; break; }
    case L1Obj::CSC:      { out <<"CSC     "; break; }
    case L1Obj::GMT:      { out <<"GMT     "; break; }
    case L1Obj::RPCb_emu: { out <<"RPCb_emu"; break; }
    case L1Obj::RPCf_emu: { out <<"RPCf_emu"; break; }
    case L1Obj::GMT_emu:  { out <<"GMT_emu "; break; }
    case L1Obj::NONE   :  { out <<"NONE    "; break; }
    default: out <<"Unknown";
  };
  out <<" pt: "<<o.pt<<", eta: "<<o.eta<<", phi: "<<o.phi<<", q: "<<o.q<<", bx: "<<o.bx;
  return out;
}
