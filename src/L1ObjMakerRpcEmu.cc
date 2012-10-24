#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjMakerRpcEmu.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"

#include "L1Trigger/RPCTrigger/interface/RPCConst.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"

#include <iostream>
#include <fstream>

using namespace std;

vector<L1Obj> L1ObjMakerRpcEmu::operator()()
{
  typedef std::vector<L1MuRegionalCand> RegCand;
  RegCand allRpcCand;
  vector<L1Obj> result;

  edm::Handle<std::vector<L1MuRegionalCand> > candB;
  theEv.getByLabel(theL1RpcDigis.label(), "RPCb", candB);
  edm::Handle<std::vector<L1MuRegionalCand> > candF;
  theEv.getByLabel(theL1RpcDigis.label(), "RPCf", candF);

  if (!candB.isValid() && !candF.isValid() ) return result;

  allRpcCand.insert(allRpcCand.end(), candB->begin(), candB->end());
  allRpcCand.insert(allRpcCand.end(), candF->begin(), candF->end());

  for (RegCand::const_iterator it=allRpcCand.begin(); it != allRpcCand.end(); it++) {
    if (it->empty()) continue;
    L1Obj obj;
    int tower = it->eta_packed();
    if (tower > 16) tower = - ( (~tower & 63) + 1);
    obj.eta = RPCConst::etaFromTowerNum(tower);
    obj.phi = 2*M_PI*(it->phi_packed())/144;
    obj.pt  = RPCConst::ptFromIpt(it->pt_packed());
    obj.q   = it->quality();
    obj.bx  = it->bx();
    result.push_back(obj);
  }

  return result;
}

