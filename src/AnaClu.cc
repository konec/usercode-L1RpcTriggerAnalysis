#include "UserCode/L1RpcTriggerAnalysis/interface/AnaClu.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TAxis.h"
#include "TGraphErrors.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include <sstream>
#include <algorithm>

namespace { 
  TH1D *hClu_SizeDigi, *hClu_SizeRHit;
  TH2D *hClu_DigiRHit;
} 


void AnaClu::run( 
            const EventObj* event,
            const MuonObj* muon,
            const L1ObjColl *l1RpcColl,
            const std::vector<uint32_t> & detsHitsCompatibleWithMuon,
            const std::vector<uint32_t> & nDigisCompDets,
            const std::vector<uint32_t> & clSizeCompDets)
{
  if (cluRunMap.find(event->run) == cluRunMap.end()) cluRunMap[event->run] = std::make_pair(0,0);
  if (   detsHitsCompatibleWithMuon.size() != nDigisCompDets.size()
      || detsHitsCompatibleWithMuon.size() != clSizeCompDets.size() ) {
   std::cout <<" PROBLME, ROZMIARY" << std::endl;
   return;
  } 

  
  for (unsigned int i=0; i< nDigisCompDets.size(); ++i) {
    hClu_SizeRHit->Fill(clSizeCompDets[i]);
    hClu_SizeDigi->Fill(nDigisCompDets[i]);
    hClu_DigiRHit->Fill(clSizeCompDets[i],nDigisCompDets[i]);
    cluRunMap[event->run].first++;
    cluRunMap[event->run].second += nDigisCompDets[i];
  }
}

TGraph* AnaClu::resume()
{
  TGraphErrors * hGraphRun = new TGraphErrors();
   unsigned int nPoints = 0;
  for( CluRunMap::const_iterator im = cluRunMap.begin(); im != cluRunMap.end(); ++im) if (im->second.first != 0) ++nPoints;
  hGraphRun->Set(nPoints);

  unsigned int iPoint=0;
  for( CluRunMap::const_iterator im = cluRunMap.begin(); im != cluRunMap.end(); ++im) {
    if (im->second.first==0 ) continue;
    double clu = static_cast<double>(im->second.second) / im->second.first;
    double cluErr = sqrt( static_cast<double>(im->second.second)) / im->second.first;
    std::cout <<" RUN: "<<im->first <<" Cluster: "<< clu <<std::endl;
    hGraphRun->SetPoint(iPoint, im->first, clu);
    hGraphRun->SetPointError(iPoint, 0., cluErr);
    iPoint++;
  }
  return hGraphRun;
}


void AnaClu::init(TObjArray& histos)
{
 hClu_SizeDigi = new TH1D( "hClu_SizeDigi", "hClu_SizeDigi", 97, -0.5, 96.5);  histos.Add( hClu_SizeDigi);
 hClu_SizeRHit = new TH1D( "hClu_SizeRHit", "hClu_SizeRHit", 97, -0.5, 96.5);  histos.Add( hClu_SizeRHit);
 hClu_DigiRHit = new TH2D( "hClu_DigiRHit","hClu_DigiRHit", 97, -0.5, 96.5, 97, -0.5, 96.5);  histos.Add(hClu_DigiRHit);
}
