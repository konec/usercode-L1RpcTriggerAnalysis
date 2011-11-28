#include "UserCode/L1RpcTriggerAnalysis/interface/AnaDet.h"
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
  TH1D *hDet_EfficRoll, *hDet_EfficRollWeighted, *hDet_EfficPull; 
  TH1D *hDet_CoincDet, *hDet_CoincHit, *hDet_Muon; 
  TH1D *hDet_PartHit[6], *hDet_PartDet[6];
} 


void AnaDet::run( 
            const MuonObj* muon,
            const std::vector<uint32_t> & detsHitsCompatibleWithMuon,
            const std::vector<uint32_t> & detsCrossedByMuon,
            const std::vector<uint32_t> & detsCrossedByMuonDeepInside)
{
  typedef std::vector<uint32_t>::const_iterator IDET;
  if( muon->pt() < 10.) return;

  DetEfficiencyManager aManDIS;
  for( IDET it= detsHitsCompatibleWithMuon.begin(); it < detsHitsCompatibleWithMuon.end(); it++) aManDIS.addDetHit( *it );
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
  for( IDET it= detsHitsCompatibleWithMuon.begin(); it < detsHitsCompatibleWithMuon.end(); it++) aMan.addDetHit( *it );
  for( IDET it= detsCrossedByMuon.begin(); it < detsCrossedByMuon.end(); it++) aMan.addDetMuon( *it );
  const std::vector<DetEfficiency> & detEffic= aMan.dets();
  //
  // coincidenciy efficiencies 
  //
  std::bitset<6> hits;
  std::bitset<6> dets;
  for (std::vector<DetEfficiency>::const_iterator it = detEffic.begin(); it != detEffic.end(); ++it) {
    if (debug) { std::cout <<it->print() << std::endl; }
    if (it->isDummy()) continue;
    RPCDetIdUtil det(it->detId());
    if (it->numberOfHits() > 0) hits.set(det.layer()-1);
    if (it->numberOfMuons() > 0) dets.set(det.layer()-1);
  }
  if (debug) std::cout <<" Dets crossed by muons: "<<dets.to_string() << std::endl;
  if (debug) std::cout <<" Hits letf    by muons: "<<hits.to_string() << std::endl;

  if  (fabs(muon->eta()) < 0.8) {
    hDet_Muon->Fill(muon->eta());
    bool matched_hits = false;
    bool matched_dets = false;
    typedef std::vector< std::bitset<6> >::const_iterator IT;
    for (IT it= theLayers.begin(); it != theLayers.end(); ++it) if (it->to_ulong() == hits.to_ulong()) matched_hits = true;
    for (IT it= theLayers.begin(); it != theLayers.end(); ++it) if (it->to_ulong() == dets.to_ulong()) matched_dets = true;
    if (matched_hits) hDet_CoincHit->Fill(muon->eta());
    if (matched_dets) hDet_CoincDet->Fill(muon->eta());
    if (debug) std::cout<<"Coinc dets: "<<matched_dets<<"Coinc hits: "<<matched_hits<<std::endl;
/*
    int nhits = 0; for(unsigned int i=0; i<6; i++) if (hits[i]) nhits++; 
    int ndets= 0; for(unsigned int i=0; i<6; i++) if (dets[i]) ndets++; 
    if (nhits>=3 && 
       (    ( (hits[0]||hits[1]) + (hits[2]||hits[3]) == 2 ) 
         || ( (hits[0]||hits[1]) + (hits[2]||hits[3]) + hits[4] + hits[5] >=3 ) ) ) hDet_CoincHit->Fill(muon->eta());
    if (ndets>=3 && 
       (    ( (dets[0]||dets[1]) + (dets[2]||dets[3]) == 2 ) 
         || ( (dets[0]||dets[1]) + (dets[2]||dets[3]) + dets[4] + dets[5] >=3 ) ) ) hDet_CoincDet->Fill(muon->eta());
    if (!hits[0] && !hits[1] && hits[2] && hits[3] && !hits[4] &&hits[5]) debug=true;
*/
    
  } else if (fabs(muon->eta()) > 1.25 && fabs(muon->eta()) < 1.6) {
    hDet_Muon->Fill(muon->eta());
//    int nhits = 0; for(unsigned int i=0; i<3; i++) if (hits[i]) nhits++; 
//    int ndets= 0; for(unsigned int i=0; i<3; i++) if (dets[i]) ndets++; 
    if (hits.count() >= 3) hDet_CoincHit->Fill(muon->eta()); 
    if (dets.count() >= 3) hDet_CoincDet->Fill(muon->eta()); 
  } 

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

AnaDet::AnaDet(TObjArray& histos)
  : debug(false)
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
/*


  for (unsigned int i=1; i<=6; ++i) {
    std::stringstream name;
    name.str(""); name<<"hDet_Det_N"<< i; hEfficDetB_N[i-1]= new TH1D( name.str().c_str(), name.str().c_str(), 64, -1.6, 1.6);  histos.Add( hEfficDetB_N[i-1]);
    name.str(""); name<<"hDet_HitB_N"<< i; hEfficHitB_N[i-1]= new TH1D( name.str().c_str(), name.str().c_str(), 64, -1.6, 1.6);  histos.Add( hEfficHitB_N[i-1]);
    }
  }

  TH1D * hHitsB = new TH1D("hHitsB","Layers with RPC hits matching propagated muon - Barrel;No of layers;Muons",7,-0.5,6.5);  histos.Add(hHitsB);
  TH1D * hHitsE = new TH1D("hHitsE","Layers with RPC hits matching propagated muon - Endcap;No of layers;Muons",4,-0.5,3.5);  histos.Add(hHitsE);
  TH1D * hDet_sB_100 = new TH1D("hDet_sB_100","Layers with RPCs crossed by propagated muon - Barrel;No of layers;Muons",7,-0.5,6.5);  histos.Add(hDet_sB_100);
  TH1D * hDet_sE_100 = new TH1D("hDet_sE_100","Layers with RPCs crossed by propagated muon - Endcap;No of layers;Muons",4,-0.5,3.5);  histos.Add(hDet_sE_100);

  // efficiency for hits
  // barrel |eta|<0,8, endcap 1.25<|eta|<1.55 
  TH1D * hEfficGeom_M = new TH1D("hEfficGeom_M","Propaged muons matching RPC Geom;Glb.muon #eta;Muons /bin", 64, -1.6, 1.6);  histos.Add(hEfficGeom_M);
  TH1D * hEfficGeom_H = new TH1D("hEfficGeom_H","Propaged muons matching RPC Geom;Glb.muon #eta;Muons / bin", 64, -1.6, 1.6);  histos.Add(hEfficGeom_H);
  TH1D * hEfficGeom_D = new TH1D("hEfficGeom_D","RPC triggers;Glb.muon #eta;Muons / bin",64, -1.6, 1.6);  histos.Add(hEfficGeom_D);
  TH1D * hEfficGeom_T = new TH1D("hEfficGeom_T","Propaged muons crossing RPCs;Glb.muon #eta;Muons / bin",64, -1.6, 1.6);  histos.Add(hEfficGeom_T);

  TH1D * hEfficGeom_H_3z6 = new TH1D("hEfficGeom_H_3z6","Propaged muons matching RPC Geom;Glb.muon #eta;Muons / bin", 64, -1.6, 1.6);  histos.Add(hEfficGeom_H_3z6);
  TH1D * hEfficGeom_D_3z6 = new TH1D("hEfficGeom_D_3z6","RPC triggers;Glb.muon #eta;Muons / bin",64, -1.6, 1.6);  histos.Add(hEfficGeom_D_3z6);
  TH1D * hEfficGeom_T_3z6 = new TH1D("hEfficGeom_T_3z6","Propaged muons crossing RPCs;Glb.muon #eta;Muons / bin",64, -1.6, 1.6);  histos.Add(hEfficGeom_T_3z6);

  //without affected stations

  TH1D * hEfficGeom_M_good = new TH1D("hEfficGeom_M_good","Propaged muons matching RPC Geom;Glb.muon #eta;Muons /bin", 64, -1.6, 1.6);  histos.Add(hEfficGeom_M_good);
  TH1D * hEfficGeom_H_good = new TH1D("hEfficGeom_H_good","Propaged muons matching RPC Geom;Glb.muon #eta;Muons / bin", 64, -1.6, 1.6);  histos.Add(hEfficGeom_H_good);
  TH1D * hEfficGeom_D_good = new TH1D("hEfficGeom_D_good","RPC triggers;Glb.muon #eta;Muons / bin",64, -1.6, 1.6);  histos.Add(hEfficGeom_D_good);
  TH1D * hEfficGeom_T_good = new TH1D("hEfficGeom_T_good","Propaged muons crossing RPCs;Glb.muon #eta;Muons / bin",64, -1.6, 1.6);  histos.Add(hEfficGeom_T_good);

  TH1D * hEfficGeom_H_3z6_good = new TH1D("hEfficGeom_H_3z6_good","Propaged muons matching RPC Geom;Glb.muon #eta;Muons / bin", 64, -1.6, 1.6);  histos.Add(hEfficGeom_H_3z6_good);
  TH1D * hEfficGeom_D_3z6_good = new TH1D("hEfficGeom_D_3z6_good","RPC triggers;Glb.muon #eta;Muons / bin",64, -1.6, 1.6);  histos.Add(hEfficGeom_D_3z6_good);
  TH1D * hEfficGeom_T_3z6_good = new TH1D("hEfficGeom_T_3z6_good","Propaged muons crossing RPCs;Glb.muon #eta;Muons / bin",64, -1.6, 1.6);  histos.Add(hEfficGeom_T_3z6_good);


 // efficiency for hits
  // full region
  TH1D * hEfficGeomTot_M = new TH1D("hEfficGeomTot_M","Propaged muons matching RPC Geom;Glb.muon #eta;Muons /bin", 128, -1.6, 1.6);  histos.Add(hEfficGeomTot_M);
  TH1D * hEfficGeomTot_H = new TH1D("hEfficGeomTot_H","Propaged muons matching RPC Geom;Glb.muon #eta;Muons / bin", 128, -1.6, 1.6);  histos.Add(hEfficGeomTot_H);
  TH1D * hEfficGeomTot_D = new TH1D("hEfficGeomTot_D","RPC triggers;Glb.muon #eta;Muons / bin",128, -1.6, 1.6);  histos.Add(hEfficGeomTot_D);
  TH1D * hEfficGeomTot_T = new TH1D("hEfficGeomTot_T","Propaged muons crossing RPCs;Glb.muon #eta;Muons / bin",128, -1.6, 1.6);  histos.Add(hEfficGeomTot_T);


  // in order to get numbewr of crossed layers
  TH1D * hEfficChambBar_N = new TH1D("hEfficChambBar_N","Propaged muons matching RPC hits - Barrel;Layer;Muons",6,0.5,6.5); histos.Add(hEfficChambBar_N);
  TH1D * hEfficChambBar_D = new TH1D("hEfficChambBar_D","Propaged muons crossing RPCs - Barrel;Layer;Muons",6,0.5,6.5); histos.Add(hEfficChambBar_D);
  TH1D * hEfficChambEnd_N = new TH1D("hEfficChambEnd_N","Propaged muons matching RPC hits - Endcap;Layer;Muons",3,0.5,3.5); histos.Add(hEfficChambEnd_N);
  TH1D * hEfficChambEnd_D = new TH1D("hEfficChambEnd_D","Propaged muons crossing RPCs - Endcap;Layer;Muons",3,0.5,3.5); histos.Add(hEfficChambEnd_D);

  // efficiency for trigger wrt Mu 
  TH1D * hEfficMu_N =  new TH1D("hEfficMu_N","hEfficMu_N",64, -1.6, 1.6);  histos.Add(hEfficMu_N);
  TH1D * hEfficMu_D =  new TH1D("hEfficMu_D","hEfficMu_D",64, -1.6, 1.6);  histos.Add(hEfficMu_D);
*/

  //
  // initialise layers combinations in the barrel (from LUT).
  //
    theLayers.push_back(std::bitset<6>(std::string("011110")));
    theLayers.push_back(std::bitset<6>(std::string("101110")));
    theLayers.push_back(std::bitset<6>(std::string("110110")));
    theLayers.push_back(std::bitset<6>(std::string("111010")));
    theLayers.push_back(std::bitset<6>(std::string("111100")));
    theLayers.push_back(std::bitset<6>(std::string("001111")));
    theLayers.push_back(std::bitset<6>(std::string("010111")));
    theLayers.push_back(std::bitset<6>(std::string("100111")));
    theLayers.push_back(std::bitset<6>(std::string("011011")));
    theLayers.push_back(std::bitset<6>(std::string("101011")));
    theLayers.push_back(std::bitset<6>(std::string("110011")));
    theLayers.push_back(std::bitset<6>(std::string("011101")));
    theLayers.push_back(std::bitset<6>(std::string("101101")));
    theLayers.push_back(std::bitset<6>(std::string("110101")));
    theLayers.push_back(std::bitset<6>(std::string("111001")));
    theLayers.push_back(std::bitset<6>(std::string("011111")));
    theLayers.push_back(std::bitset<6>(std::string("101111")));
    theLayers.push_back(std::bitset<6>(std::string("110111")));
    theLayers.push_back(std::bitset<6>(std::string("111011")));
    theLayers.push_back(std::bitset<6>(std::string("111101")));
    theLayers.push_back(std::bitset<6>(std::string("111110")));
    theLayers.push_back(std::bitset<6>(std::string("111111")));
    theLayers.push_back(std::bitset<6>(std::string("111000")));
    theLayers.push_back(std::bitset<6>(std::string("110100")));
    theLayers.push_back(std::bitset<6>(std::string("110010")));
    theLayers.push_back(std::bitset<6>(std::string("110001")));
    theLayers.push_back(std::bitset<6>(std::string("100101")));
    theLayers.push_back(std::bitset<6>(std::string("101001")));
    theLayers.push_back(std::bitset<6>(std::string("101010")));
    theLayers.push_back(std::bitset<6>(std::string("101100")));
    theLayers.push_back(std::bitset<6>(std::string("010101")));
    theLayers.push_back(std::bitset<6>(std::string("011001")));
    theLayers.push_back(std::bitset<6>(std::string("011010")));
    theLayers.push_back(std::bitset<6>(std::string("011100")));
    theLayers.push_back(std::bitset<6>(std::string("000111")));
    theLayers.push_back(std::bitset<6>(std::string("001011")));
    theLayers.push_back(std::bitset<6>(std::string("001101")));
    theLayers.push_back(std::bitset<6>(std::string("001110")));

}
