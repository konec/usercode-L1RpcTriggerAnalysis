#include "UserCode/L1RpcTriggerAnalysis/interface/AnaEmu.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TAxis.h"
#include "TGraphErrors.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include <sstream>

namespace {
  TH1D  *hEmu_Muon, *hEmu_Wide, *hEmu_L1Rpc;
}

AnaEmu::AnaEmu(TObjArray& histos)
  : debug(false)
{
 hEmu_Muon= new TH1D( "hEmu_Muon", "hEmu_Muon", 64, -1.6, 1.6);  histos.Add( hEmu_Muon);
 hEmu_Wide= new TH1D( "hEmu_Wide", "hEmu_Wide", 64, -1.6, 1.6);  histos.Add( hEmu_Wide);
 hEmu_L1Rpc= new TH1D( "hEmu_L1Rpc", "hEmu_L1Rpc", 64, -1.6, 1.6);  histos.Add( hEmu_L1Rpc);
}

void AnaEmu::run( const EventObj* event, const MuonObj* muon,
            const L1ObjColl *l1RpcCollEmu,
            const L1ObjColl *l1RpcColl)
{
  if( muon->pt() < 10.) return;
  hEmu_Muon->Fill(muon->eta());
  std::vector<L1Obj> l1Rpcs = l1RpcColl->getL1ObjsSelected();
  //std::vector<L1Obj> l1RpcsEmu = l1RpcCollEmu->getL1ObjsMatched();
  std::vector<L1Obj> l1RpcsEmu = l1RpcCollEmu->getL1ObjsMatched();
/*
  if (l1RpcsEmu.size()!=0 && l1Rpcs.size()==0 && muon->eta()>-0.25 && muon->eta() < -0.2) {
    std::cout<<"--------------"<<std::endl;
    debug = true;
    std::cout <<"MUON: pt="<<muon->pt()<<" eta="<<muon->eta()<<std::endl;
    std::cout <<" HAS EMU, NO TRIGGER matched" << std::endl;
  }
*/
  if (l1Rpcs.size() > 0) hEmu_L1Rpc->Fill(muon->eta());
  if (l1RpcsEmu.size() > 0) hEmu_Wide->Fill(muon->eta());
  if (l1Rpcs.size() > 0 && l1RpcsEmu.size()==0) {
     std::cout <<"has no Emu but Rpc, run="<<event->run<<" lumi="<<event->lumi<<std::endl;
//     debug = true;
  }
  if (debug) std::cout <<"MUON: pt="<<muon->pt()<<" eta="<<muon->eta()<<std::endl;
  if (debug) {std::cout <<" L1 EMU: "<<std::endl; l1RpcCollEmu->print();}
  if (debug) {std::cout <<" L1 RPC: "<<std::endl; l1RpcColl->print();}
}


