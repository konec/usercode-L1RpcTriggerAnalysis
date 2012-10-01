#include "UserCode/L1RpcTriggerAnalysis/interface/AnaClu.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TAxis.h"
#include "TGraphErrors.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include <sstream>
#include <bitset>
#include <algorithm>

namespace { 
  TH1D *hClu_;
} 


void AnaClu::run( 
            const MuonObj* muon,
            const L1ObjColl *l1RpcColl,
            const std::vector<uint32_t> & detsHitsCompatibleWithMuon,
            const std::vector<uint32_t> & nDigisCompDets)
{
  if (detsHitsCompatibleWithMuon.size() != nDigisCompDets.size() ) {
   std::cout <<" PROBLME, ROZMIARY" << std::endl;
   return;
  } 
//    for (unsigned int i=0; i< nDigisCompDets.size(); ++i)
//      std::cout <<"("<<detsHitsCompatibleWithMuon[i]<<","<<nDigisCompDets[i]<<")";
//    std::cout << std::endl;
}


void AnaClu::init(TObjArray& histos)
{
}
