#include "UserCode/L1RpcTriggerAnalysis/interface/AnaTimingL1.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraphErrors.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include <vector>
#include <cmath>

template <class T> T sqr( T t) {return t*t;}
namespace {
  TH1D *hTimingL1_Rpc, *hTimingL1_Dt, *hTimingL1_Csc; 
  TH2D *hTimingL1_RpcVsDt, *hTimingL1_RpcVsCsc;
}

void AnaTimingL1::init(TObjArray& histos)
{
  hTimingL1_Rpc = new TH1D("hTimingL1_Rpc","hTimingL1_Rpc", 5,-2.5,2.5);   hTimingL1_Rpc->Sumw2(); histos.Add(hTimingL1_Rpc);
  hTimingL1_Dt  = new TH1D("hTimingL1_Dt","hTimingL1_Dt",   5,-2.5,2.5);   hTimingL1_Dt->Sumw2();  histos.Add(hTimingL1_Dt);
  hTimingL1_Csc = new TH1D("hTimingL1_Csc","hTimingL1_Csc", 5,-2.5,2.5);   hTimingL1_Csc->Sumw2(); histos.Add(hTimingL1_Csc);
  hTimingL1_RpcVsDt  = new TH2D("hTimingL1_RpcVsDt", "hTimingL1_RpcVsDt", 5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsDt);
  hTimingL1_RpcVsCsc = new TH2D("hTimingL1_RpcVsCsc","hTimingL1_RpcVsCsc",5,-2.5,2.5, 5,-2.5,2.5); histos.Add(hTimingL1_RpcVsCsc);
}

void  AnaTimingL1::resume(TObjArray& histos)
{
}

void  AnaTimingL1::run(const EventObj* ev, const MuonObj* muon, const L1ObjColl *l1RpcColl, const L1ObjColl *l1OtherColl)
{
  double ptMu  = muon->pt();
  if (ptMu < 10.) return;
  if (!muon->isGlobal()) return;

  std::vector<L1Obj> l1Rpcs = l1RpcColl->getL1ObjsMatched();
  std::vector<L1Obj> l1Oths = l1OtherColl->getL1ObjsMatched();  

  int bxRpc = 999;
  int bxDt = 999;
  int bxCsc = 999;
  bool rpc, dt,csc; 
  rpc=dt=csc = false; 

  for (unsigned int i=0; i< l1Rpcs.size(); i++) {
    if (1+l1Rpcs[i].bx < bxRpc) { 
      rpc=true; 
      bxRpc = l1Rpcs[i].bx+1;
    }
  }

  for (unsigned int i=0; i< l1Oths.size(); i++) {
    if ( std::fabs(l1Oths[i].eta) < 1.05) {
      if (l1Oths[i].bx < bxDt) { bxDt = l1Oths[i].bx; dt = true; }
    } else {
      if (l1Oths[i].bx < bxCsc) { bxCsc = l1Oths[i].bx; csc = true; }
    }
  }

  if (dt && csc) std::cout << "PROBLEM, BOTH CSC AND DT!" <<std::endl;
  if (rpc) hTimingL1_Rpc->Fill(bxRpc);
  if (dt)  hTimingL1_Dt->Fill(bxDt);
  if (csc) hTimingL1_Csc->Fill(bxCsc);
  if ( dt && rpc)  hTimingL1_RpcVsDt->Fill(bxRpc, bxDt);
  if (csc && rpc) hTimingL1_RpcVsCsc->Fill(bxRpc, bxCsc);
  
}
