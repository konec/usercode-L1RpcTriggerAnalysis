#include "UserCode/L1RpcTriggerAnalysis/interface/AnaSiMuDistribution.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "TProfile.h"
#include "TObjArray.h"
#include "TH2D.h"
#include "TH1D.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"

#include <cmath>
#include <algorithm>

namespace {
  TH1D *hSiMuPt_DIS, *hSiMuEta_DIS, *hSiMuPhi_DIS;
  TH1D *hSiMuPt_INP, *hSiMuEta_INP, *hSiMuPhi_INP;
}

AnaSiMuDistribution::AnaSiMuDistribution(const edm::ParameterSet& cfg)
  : ptMin( cfg.getParameter<double>("ptMin") ),
    etaMinRef (cfg.getParameter<double>("absEtaMinRef") ),
    etaMaxRef (cfg.getParameter<double>("absEtaMaxRef") ),
    phiMinRef (cfg.getParameter<double>("phiMinRef") ),
    phiMaxRef (cfg.getParameter<double>("phiMaxRef") ),
    checkMatchedDets (cfg.exists("matchedDets")),
    matchedDets(checkMatchedDets ? cfg.getParameter<  std::vector<uint32_t> >("matchedDets") : std::vector<uint32_t>())
{ }

void AnaSiMuDistribution::init(TObjArray& histos)
{
  hSiMuPt_DIS  = new TH1D("hSiMuPt_DIS","All global SiMus Pt;Glb.SiMu p_{T} [GeV];SiMus / bin",L1PtScale::nPtBins,L1PtScale::ptBins);  histos.Add(hSiMuPt_DIS);
  hSiMuEta_DIS = new TH1D("hSiMuEta_DIS","All global SiMus Eta;Glb.SiMu #eta;SiMus / bin",96, -2.4, 2.4);  histos.Add(hSiMuEta_DIS);
  hSiMuPhi_DIS = new TH1D("hSiMuPhi_DIS","All global SiMus Phi;Glb.SiMu #phi [rad];SiMus / bin",90,-M_PI,M_PI);  histos.Add(hSiMuPhi_DIS);
  hSiMuPt_INP  = new TH1D("hSiMuPt_INP","All global SiMus Pt;Glb.SiMu p_{T} [GeV];SiMus / bin",L1PtScale::nPtBins,L1PtScale::ptBins);  histos.Add(hSiMuPt_INP);
  hSiMuEta_INP = new TH1D("hSiMuEta_INP","All global SiMus Eta;Glb.SiMu #eta;SiMus / bin",96, -2.4, 2.4);  histos.Add(hSiMuEta_INP);
  hSiMuPhi_INP = new TH1D("hSiMuPhi_INP","All global SiMus Phi;Glb.SiMu #phi [rad];SiMus / bin",90,-M_PI,M_PI);  histos.Add(hSiMuPhi_INP);
}

bool AnaSiMuDistribution::filter(const EventObj* ev, const TrackObj * simu, const HitSpecObj * hitSpec)
{
  if (!hitSpec) return false;
  if (!simu) return false;
  uint32_t rawId = hitSpec->rawId();

  hSiMuPt_INP->Fill(simu->pt());
  hSiMuEta_INP->Fill(simu->eta());
  hSiMuPhi_INP->Fill(simu->phi());

  if (simu->pt() < ptMin) return false;
  if ( fabs(hitSpec->position().eta()) < etaMinRef) return false;
  if ( fabs(hitSpec->position().eta()) > etaMaxRef) return false;
  if ( hitSpec->position().phi() < phiMinRef) return false;
  if ( hitSpec->position().phi() > phiMaxRef) return false;

  if (checkMatchedDets && matchedDets.end()==find(matchedDets.begin(),matchedDets.end(), rawId) ) return false;

  hSiMuPt_DIS->Fill(simu->pt());
  hSiMuEta_DIS->Fill(simu->eta());
  hSiMuPhi_DIS->Fill(simu->phi());

  return true;
}

