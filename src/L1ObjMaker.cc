#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjMaker.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"
#include "L1Trigger/RPCTrigger/interface/RPCConst.h"



#include "FWCore/Utilities/interface/InputTag.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"

#include <iostream>
#include <fstream>

using namespace std;

L1ObjMaker::L1ObjMaker(const  edm::ParameterSet & cfg) 
  :  theConfig(cfg), 
     lastEvent(0),lastRun(0)
{}

void L1ObjMaker::run(const edm::Event &ev)
{
  if (lastEvent==ev.id().event() && lastRun==ev.run()) return;
  lastEvent = ev.id().event() ;
  lastRun = ev.run();
  theL1Objs.clear();

  if (theConfig.exists("l1RpcSource"))     getGMTReadout( ev, theL1Objs, theConfig.getParameter<edm::InputTag>("l1RpcSource"),    L1Obj::RPCb); 
  if (theConfig.exists("l1RpcSource"))     getGMTReadout( ev, theL1Objs, theConfig.getParameter<edm::InputTag>("l1RpcSource"),    L1Obj::RPCf); 
  if (theConfig.exists("l1CscSource"))     getGMTReadout( ev, theL1Objs, theConfig.getParameter<edm::InputTag>("l1CscSource"),    L1Obj::CSC); 
  if (theConfig.exists("l1DtSource") )     getGMTReadout( ev, theL1Objs, theConfig.getParameter<edm::InputTag>("l1DtSource"),     L1Obj::DT); 
  if (theConfig.exists("l1GmtSource"))     getGMTReadout( ev, theL1Objs, theConfig.getParameter<edm::InputTag>("l1GmtSource"),    L1Obj::GMT); 
  if (theConfig.exists("l1RpcEmuSource"))  getRpcRegional(ev, theL1Objs, theConfig.getParameter<edm::InputTag>("l1RpcEmuSource")); 
  if (theConfig.exists("l1GmtEmuSource"))  getGMTReadout( ev, theL1Objs, theConfig.getParameter<edm::InputTag>("l1GmtEmuSource"), L1Obj::GMT_emu); 

  
}

bool L1ObjMaker::getGMTReadout(const edm::Event &ev, vector<L1Obj> &result, const edm::InputTag &readout, L1Obj::TYPE type)
{
  edm::Handle<L1MuGMTReadoutCollection> pCollection;
  ev.getByLabel(readout,pCollection);
  if (!pCollection.isValid()) return false;

  L1MuGMTReadoutCollection const* gmtrc = pCollection.product();
  if (!gmtrc) return false;

  vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  vector<L1MuGMTReadoutRecord>::const_iterator RRItr;
  for( RRItr = gmt_records.begin() ; RRItr != gmt_records.end() ; RRItr++ ) {
    vector<L1MuRegionalCand>    cands;
    vector<L1MuGMTExtendedCand> gmts;
    switch (type) {
      case L1Obj::RPCb: { cands = RRItr->getBrlRPCCands(); break; }  
      case L1Obj::RPCf: { cands = RRItr->getFwdRPCCands(); break; }
      case L1Obj::DT:   { cands = RRItr->getDTBXCands(); break; }
      case L1Obj::CSC:  { cands = RRItr->getCSCCands(); break; }
      case L1Obj::GMT:      { gmts  = RRItr->getGMTCands(); break; }
      case L1Obj::GMT_emu:  { gmts  = RRItr->getGMTCands(); break; }
      default: break;
    };

    typedef vector<L1MuRegionalCand>::const_iterator ITC;
    for(ITC it = cands.begin() ; it != cands.end() ; ++it ) {
      if (it->empty()) continue;
      result.push_back( makeL1Obj( *it, type) );
    }

    typedef vector<L1MuGMTExtendedCand>::const_iterator ITG;
    for(ITG it = gmts.begin() ; it != gmts.end() ; ++it ) {
      if (it->empty()) continue;
      result.push_back( makeL1Obj( *it, type) );
    }

  }
  return true;
}

bool L1ObjMaker:: getRpcRegional(const edm::Event &ev, std::vector<L1Obj> & result, const edm::InputTag &l1RpcDigis)
{
  typedef std::vector<L1MuRegionalCand> RegCand;
  RegCand allRpcCand;

  edm::Handle<std::vector<L1MuRegionalCand> > candB;
  ev.getByLabel(l1RpcDigis.label(), "RPCb", candB);
  edm::Handle<std::vector<L1MuRegionalCand> > candF;
  ev.getByLabel(l1RpcDigis.label(), "RPCf", candF);

  if (!candB.isValid() && !candF.isValid() ) return false;

  allRpcCand.insert(allRpcCand.end(), candB->begin(), candB->end());
  allRpcCand.insert(allRpcCand.end(), candF->begin(), candF->end());

  for (unsigned int icand = 0; icand < allRpcCand.size(); ++icand) {
    const L1MuRegionalCand & it = allRpcCand[icand];
    if (it.empty()) continue;
    L1Obj obj;
    int tower = it.eta_packed();
    if (tower > 16) tower = - ( (~tower & 63) + 1);
    obj.eta = RPCConst::etaFromTowerNum(tower);
    obj.phi = 2*M_PI*(it.phi_packed())/144;
    obj.pt  = RPCConst::ptFromIpt(it.pt_packed());
    obj.charge = it.chargeValue();
    obj.q   = it.quality();
    obj.bx  = it.bx();
    obj.type = (icand < candB->size()) ? L1Obj::RPCb_emu : L1Obj::RPCf_emu;
    result.push_back(obj);
  }
  return true;
}


