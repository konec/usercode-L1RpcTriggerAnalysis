#include "UserCode/L1RpcTriggerAnalysis/interface/AnaDet.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TAxis.h"
#include "TGraphErrors.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObj.h"
#include <sstream>
#include <bitset>
#include <algorithm>

namespace { 
  TH1D *hDet_EfficRoll, *hDet_EfficRollWeighted, *hDet_EfficPull; 
  TH1D *hDet_CoincDet, *hDet_CoincHit, *hDet_Muon; 
  TH1D *hDet_PartHit[6], *hDet_PartDet[6];
} 


void AnaDet::run( 
            const MuonObj* muon,
            const std::vector<DetCluDigiObj> & detsHitsCompatibleWithMuon,
            const std::vector<uint32_t> & detsCrossedByMuon,
            const std::vector<uint32_t> & detsCrossedByMuonDeepInside)
{
  typedef std::vector<uint32_t>::const_iterator IDET;
  typedef std::vector<DetCluDigiObj>::const_iterator IDCD;
  if( muon->pt() < 10.) return;
//  if (fabs(muon->eta()) > 1.14 && fabs(muon->eta()) < 1.24) debug = true;

  DetEfficiencyManager aManDIS;
  for( IDCD it= detsHitsCompatibleWithMuon.begin(); it < detsHitsCompatibleWithMuon.end(); it++) aManDIS.addDetHit( it->det );
  for( IDET it= detsCrossedByMuonDeepInside.begin(); it < detsCrossedByMuonDeepInside.end(); it++) aManDIS.addDetMuon( *it );
  const std::vector<DetEfficiency> & detEfficDIS = aManDIS.dets();

  //
  // fill "master" eff. manager and DIS efficiencies
  //
  for (std::vector<DetEfficiency>::const_iterator it = detEfficDIS.begin(); it != detEfficDIS.end(); ++it) {
    if (it->isDummy()) continue;
    if (it->numberOfMuons() > 0) {

       theDetEfficiencyManager.addDetMuon(it->detId());
       if (it->numberOfHits() > 0) theDetEfficiencyManager.addDetHit(it->detId());

       RPCDetIdUtil det(it->detId());
       if (it->numberOfHits())  hDet_PartHit[det.layer()-1]->Fill(det.part());
       hDet_PartDet[det.layer()-1]->Fill(det.part());
    }
  }

  //
  // studies with "not-deep" efficiencies
  //
  DetEfficiencyManager aMan;
  if (debug) std::cout <<"detsCrossedByMuon: "<<detsCrossedByMuon.size()<<" detsHitsCompatibleWithMuon: "<<detsHitsCompatibleWithMuon.size()<<std::endl;
  for( IDCD it= detsHitsCompatibleWithMuon.begin(); it < detsHitsCompatibleWithMuon.end(); it++) aMan.addDetHit( it->det );
  for( IDET it= detsCrossedByMuon.begin(); it < detsCrossedByMuon.end(); it++) aMan.addDetMuon( *it );
  const std::vector<DetEfficiency> & detEffic= aMan.dets();
  //
  // coincidenciy efficiencies 
  //
  std::bitset<6> hits, hitsL, hitsH;
  std::bitset<6> dets, detsL, detsH;
  double fetaL = fabs(muon->eta())-0.01;
  double fetaH = fabs(muon->eta())+0.07;
  for (std::vector<DetEfficiency>::const_iterator it = detEffic.begin(); it != detEffic.end(); ++it) {
    if (debug) { std::cout <<it->print() << std::endl; }
    if (it->isDummy()) continue;
    RPCDetIdUtil det(it->detId());

    unsigned int dl = det.layer(muon->eta());
    if (it->numberOfHits() > 0   && dl > 0) hits.set(dl-1);
    if (it->numberOfMuons() > 0  && dl > 0) dets.set(dl-1);

    unsigned int dlL = det.layer(fetaL);
    if (it->numberOfHits() > 0   && dlL > 0) hitsL.set(dlL-1);
    if (it->numberOfMuons() > 0  && dlL > 0) detsL.set(dlL-1);

    unsigned int dlH = det.layer(fetaH);
    if (it->numberOfHits() > 0   && dlH > 0) hitsH.set(dlH-1);
    if (it->numberOfMuons() > 0  && dlH > 0) detsH.set(dlH-1);
  }
  if (debug) std::cout <<" Dets crossed by muons: "<<dets.to_string() << std::endl;
  if (debug) std::cout <<" Hits letf    by muons: "<<hits.to_string() << std::endl;

  bool matched_hits =    theLayerCoincManager.matched( fetaL, hitsL) 
                      || theLayerCoincManager.matched( muon->eta(), hits ) 
                      || theLayerCoincManager.matched( fetaH, hitsH);
  bool matched_dets =    theLayerCoincManager.matched( fetaL, detsL) 
                      || theLayerCoincManager.matched( muon->eta(), dets ) 
                      || theLayerCoincManager.matched( fetaH, detsH);

  hDet_Muon->Fill(muon->eta());
  if (matched_hits) hDet_CoincHit->Fill(muon->eta());
  if (matched_dets) hDet_CoincDet->Fill(muon->eta());
  if (debug) std::cout<<"Coinc dets: "<<matched_dets<<"Coinc hits: "<<matched_hits<<std::endl;
    
}

TGraph* AnaDet::resume()
{
  TGraphErrors * hDet_GraphEffic = new TGraphErrors();
  const std::vector<DetEfficiency> &  detEffic = theDetEfficiencyManager.dets();
  unsigned int iPointforEfficiency = 0;
  unsigned int nPointforEfficiency = 0;
  for (std::vector<DetEfficiency>::const_iterator it = detEffic.begin(); it != detEffic.end(); ++it) if (!it->isDummy()) nPointforEfficiency++; 
  hDet_GraphEffic->Set(nPointforEfficiency);
  for (std::vector<DetEfficiency>::const_iterator it = detEffic.begin(); it != detEffic.end(); ++it) {
    if (it->isDummy()) continue;
    hDet_EfficRoll->Fill(it->efficiency());
    hDet_EfficRollWeighted->Fill(it->efficiency(),it->numberOfMuons());
    if (it->efficiencyError() > 0.) hDet_EfficPull->Fill( (it->efficiency()-0.95)/it->efficiencyError());
    hDet_GraphEffic->SetPoint(iPointforEfficiency, it->detId(), it->efficiency());
    hDet_GraphEffic->SetPointError(iPointforEfficiency, 0., it->efficiencyError());
    ++iPointforEfficiency;
  }
  return hDet_GraphEffic;
}

void AnaDet::init(TObjArray& histos)
{
  hDet_EfficRoll = new TH1D("hDet_EfficRoll","hDet_EfficRoll",220,-0.05,1.05); histos.Add(hDet_EfficRoll);
  hDet_EfficRollWeighted = new TH1D("hDet_EfficRollWeighted","hDet_EfficRollWeighted",220,-0.05,1.05); histos.Add(hDet_EfficRollWeighted);
  hDet_EfficPull = new TH1D("hDet_EfficPull","hDet_EfficPull",100,-5.,5.); histos.Add(hDet_EfficPull);

  hDet_Muon= new TH1D( "hDet_Muon", "hDet_Muon", 64, -1.6, 1.6);  histos.Add( hDet_Muon);
  hDet_CoincHit = new TH1D("hDet_CoincHit","hDet_CoincHit",64, -1.6, 1.6);  histos.Add(hDet_CoincHit);
  hDet_CoincDet = new TH1D("hDet_CoincDet","hDet_CoincDet",64, -1.6, 1.6);  histos.Add(hDet_CoincDet);

  for (unsigned int i=1; i<=6; ++i) {
    std::stringstream name;
    name.str(""); name<<"hDet_PartHit"<<i; hDet_PartHit[i-1] = new TH1D( name.str().c_str(), name.str().c_str(), 9, -4.5, 4.5);  histos.Add(hDet_PartHit[i-1]);
    name.str(""); name<<"hDet_PartDet"<<i; hDet_PartDet[i-1] = new TH1D( name.str().c_str(), name.str().c_str(), 9, -4.5, 4.5);  histos.Add(hDet_PartDet[i-1]);
    std::stringstream ax;
    for (int j=0; j<9; ++j) { 
      ax.str(""); if(j<2) ax<<"ring: "<<j-3; else if (j<7) ax<<"wheel: "<<j-4; else ax<<"ring: "<<j-5;  
      hDet_PartHit[i-1]->GetXaxis()->SetBinLabel(j+1, ax.str().c_str());
      hDet_PartDet[i-1]->GetXaxis()->SetBinLabel(j+1, ax.str().c_str());
    }   
  }
}
