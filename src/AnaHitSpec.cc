#include "UserCode/L1RpcTriggerAnalysis/interface/AnaHitSpec.h"

#include <cmath>
#include <sstream>


#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"

#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TGraphErrors.h"

#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"

namespace {
  TH2D *hHitSpec_PtVsPhi_BarPlus, *hHitSpec_PtVsPhi_BarMinus;
  TH2D *hHitSpec_PtVsPhi_EndPlus, *hHitSpec_PtVsPhi_EndMinus;
  TH2D *hHitSpec_PtVsPhiGen_Plus, *hHitSpec_PtVsPhiGen_Minus;
  TH2D *hHitSpec_PtVsPhiGenAtPhi0_Plus, *hHitSpec_PtVsPhiGenAtPhi0_Minus;
}
void AnaHitSpec::init(TObjArray& histos)
{
  hHitSpec_PtVsPhi_BarPlus = new TH2D("hHitSpec_PtVsPhi_BarPlus","hHitSpec_PtVsPhi_BarPlus", 32, 0., 1.6, L1PtScale::nPtBins, L1PtScale::ptBins);
  histos.Add(hHitSpec_PtVsPhi_BarPlus);
  hHitSpec_PtVsPhi_BarMinus = new TH2D("hHitSpec_PtVsPhi_BarMinus","hHitSpec_PtVsPhi_BarMinus", 32, 0., 1.6, L1PtScale::nPtBins, L1PtScale::ptBins);
  histos.Add(hHitSpec_PtVsPhi_BarMinus);
  hHitSpec_PtVsPhi_EndPlus = new TH2D("hHitSpec_PtVsPhi_EndPlus","hHitSpec_PtVsPhi_EndPlus", 32, 0., 1.6, L1PtScale::nPtBins, L1PtScale::ptBins);
  histos.Add(hHitSpec_PtVsPhi_EndPlus);
  hHitSpec_PtVsPhi_EndMinus = new TH2D("hHitSpec_PtVsPhi_EndMinus","hHitSpec_PtVsPhi_EndMinus", 32, 0., 1.6, L1PtScale::nPtBins, L1PtScale::ptBins);
  histos.Add(hHitSpec_PtVsPhi_EndMinus);

  hHitSpec_PtVsPhiGen_Plus = new TH2D("hHitSpec_PtVsPhiGen_Plus","hHitSpec_PtVsPhiGen_Plus", 28, 0.8, 2.2, L1PtScale::nPtBins, L1PtScale::ptBins);
  histos.Add(hHitSpec_PtVsPhiGen_Plus);
  hHitSpec_PtVsPhiGen_Minus = new TH2D("hHitSpec_PtVsPhiGen_Minus","hHitSpec_PtVsPhiGen_Minus", 28, -0.2, 1.2, L1PtScale::nPtBins, L1PtScale::ptBins);
  histos.Add(hHitSpec_PtVsPhiGen_Minus);

  hHitSpec_PtVsPhiGenAtPhi0_Plus = new TH2D("hHitSpec_PtVsPhiGenAtPhi0_Plus","hHitSpec_PtVsPhiGenAtPhi0_Plus", 28, 0.8, 2.2, L1PtScale::nPtBins, L1PtScale::ptBins);
  histos.Add(hHitSpec_PtVsPhiGenAtPhi0_Plus);
  hHitSpec_PtVsPhiGenAtPhi0_Minus = new TH2D("hHitSpec_PtVsPhiGenAtPhi0_Minus","hHitSpec_PtVsPhiGenAtPhi0_Minus", 28, -0.2, 1.2, L1PtScale::nPtBins, L1PtScale::ptBins);
  histos.Add(hHitSpec_PtVsPhiGenAtPhi0_Minus);
}

void  AnaHitSpec::run(const EventObj* ev, const TrackObj * simu, const HitSpecObj * hitSpec)
{
  double pt = simu->pt() > 160 ? 150. : simu->pt();
  if (simu->charge() == 1) hHitSpec_PtVsPhiGen_Plus->Fill(simu->phi() , pt);
  if (simu->charge() ==-1) hHitSpec_PtVsPhiGen_Minus->Fill(simu->phi(), pt);

  if (!hitSpec) return;
  uint32_t rawId = hitSpec->rawId();
  if (!rawId) return;
  RPCDetId     idDet ( rawId );
  RPCDetIdUtil idUtil( rawId );
  if (theH.find(rawId) == theH.end()) {
    std::stringstream str, strName;
    strName << "hHitSpec_"<<rawId;
    str << "DetId: "<<rawId;
    if (idUtil.isBarrel() ) {
      str<<", Bar, l="<<idUtil.layer()<<", sec:"<<idDet.sector()<<", roll:"<<idDet.roll(); 
    } else { 
      str<<", End, l="<< idUtil.layer()<<", ring:"<<idDet.ring()<<", chmbr: "<<(idDet.sector()-1)*6+idDet.subsector()<<", roll:"<<idDet.roll();
    }
    theH[rawId] = new TH2D( strName.str().c_str(), str.str().c_str(), 
                           L1PtScale::nPtBins, L1PtScale::ptBins,
                           100, 0.8, 1.2);
  }
  if (idUtil.isBarrel()) {
    if (simu->charge() == 1) hHitSpec_PtVsPhi_BarPlus->Fill(hitSpec->position().phi() , pt);
    if (simu->charge() ==-1) hHitSpec_PtVsPhi_BarMinus->Fill(hitSpec->position().phi() , pt);
  } else {
    if (simu->charge() == 1) hHitSpec_PtVsPhi_EndPlus->Fill(hitSpec->position().phi() , pt);
    if (simu->charge() ==-1) hHitSpec_PtVsPhi_EndMinus->Fill(hitSpec->position().phi() , pt);
  }
  theH[rawId]->Fill( pt, hitSpec->position().phi() );

  if (fabs( hitSpec->position().phi()-1.025) < 0.025) {
    if (simu->charge() == 1) hHitSpec_PtVsPhiGenAtPhi0_Plus->Fill(simu->phi() , pt);
    if (simu->charge() ==-1) hHitSpec_PtVsPhiGenAtPhi0_Minus->Fill(simu->phi(), pt);
  }
}

void AnaHitSpec::resume(TObjArray& histos)
{
  std::cout <<" AnaHitSpec,  SIZE OF HISTOS IS: " << theH.size() << std::endl;
  for (HMap::const_iterator ih = theH.begin(); ih != theH.end(); ++ih) { 
    double nTOT = ih->second->Integral(); 
    double nENT = ih->second->GetEntries();
    if (nTOT > 1000.)  std::cout <<" key: "<< ih->first 
                                <<" name: "<< ih->second->GetName()
                                <<" title: "<< ih->second->GetTitle()
                                <<" Integral: "<< nTOT <<" Entries: "<< nENT<< std::endl; 
    if (ih->second->Integral() > 1000) histos.Add(ih->second); 
  }
}
