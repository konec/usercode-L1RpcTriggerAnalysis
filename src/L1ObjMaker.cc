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
  if (t1 != L1Obj::NONE) get(result,t1); 
  if (t2 != L1Obj::NONE) get(result,t2); 
  if (t3 != L1Obj::NONE) get(result,t3); 
  if (t4 != L1Obj::NONE) get(result,t4); 
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
  for( RRItr = gmt_records.begin() ; RRItr != gmt_records.end() ; RRItr++ ) {
    vector<L1MuRegionalCand> cands;
    vector<L1MuGMTExtendedCand> gmts;
    switch (type) {
      case L1Obj::RPCB: { cands = RRItr->getBrlRPCCands(); break; }  
      case L1Obj::RPCF: { cands = RRItr->getFwdRPCCands(); break; }
      case L1Obj::DT:   { cands = RRItr->getDTBXCands(); break; }
      case L1Obj::CSC:  { cands = RRItr->getCSCCands(); break; }
      case L1Obj::GMT:  { gmts  = RRItr->getGMTCands(); break; }
      default: break;
    };

    typedef vector<L1MuRegionalCand>::const_iterator ITC;
    for(ITC it = cands.begin() ; it != cands.end() ; ++it ) {
      if (it->empty()) continue;
      objs.push_back( makeL1Obj( *it, type) );
    }

    typedef vector<L1MuGMTExtendedCand>::const_iterator ITG;
    for(ITG it = gmts.begin() ; it != gmts.end() ; ++it ) {
      if (it->empty()) continue;
      objs.push_back( makeL1Obj( *it, type) );
    }

  }
}
