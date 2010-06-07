#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjMaker.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"

#include <iostream>
#include <fstream>

using namespace std;

vector<L1Obj> L1ObjMaker::operator()( TYPE t1, TYPE t2, TYPE t3, TYPE t4)
{
  vector<L1Obj> result;
  if (t1 != NONE) get(result,t1); 
  if (t2 != NONE) get(result,t2); 
  if (t3 != NONE) get(result,t3); 
  if (t4 != NONE) get(result,t4); 
  return result;
}

void L1ObjMaker::get(vector<L1Obj> & objs, L1ObjMaker::TYPE type)
{
  edm::Handle<L1MuGMTReadoutCollection> pCollection;
  theEv.getByLabel(theReadout,pCollection);

  L1MuGMTReadoutCollection const* gmtrc = pCollection.product();
  if (!gmtrc) return;

  vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  vector<L1MuGMTReadoutRecord>::const_iterator RRItr;
  typedef vector<L1MuRegionalCand>::const_iterator ITC;
  for( RRItr = gmt_records.begin() ; RRItr != gmt_records.end() ; RRItr++ ) {
    vector<L1MuRegionalCand> cands;
    switch (type) {
      case RPCB: { cands = RRItr->getBrlRPCCands(); break; }  
      case RPCF: { cands = RRItr->getFwdRPCCands(); break; }
      case DT:   { cands = RRItr->getDTBXCands(); break; }
      case CSC:  { cands = RRItr->getCSCCands(); break; }
      default: break;
    };
    for(ITC it = cands.begin() ; it != cands.end() ; ++it ) {
      if (it->empty()) continue;
      L1Obj obj;
      obj.bx = it->bx();
      obj.q  = it->quality();
      obj.pt = it->ptValue();
      obj.eta = it->etaValue(); 
      obj.phi = it->phiValue(); 
      objs.push_back(obj);
    }
  }
}
