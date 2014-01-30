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

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"

#include <cmath>
#include <algorithm>

namespace {
  TH1D *hSiMuPt_DIS, *hSiMuEta_DIS, *hSiMuPhi_DIS;
  TH1D *hSiMuPt_INP, *hSiMuEta_INP, *hSiMuPhi_INP;
}

AnaSiMuDistribution::AnaSiMuDistribution(const edm::ParameterSet& cfg)
  : ptMin( cfg.getParameter<double>("ptMin") ),
    ptMax( cfg.getParameter<double>("ptMax") ),
    etaMinRef (cfg.getParameter<double>("absEtaMinRef") ),
    etaMaxRef (cfg.getParameter<double>("absEtaMaxRef") ),
    phiMinRef (cfg.getParameter<double>("phiMinRef") ),
    phiMaxRef (cfg.getParameter<double>("phiMaxRef") ),
    checkMatchedDets (cfg.exists("matchedDets")),
    checkMatchedSectors (cfg.exists("matchedSectors")),
    matchedDets(checkMatchedDets ? cfg.getParameter<  std::vector<uint32_t> >("matchedDets") : std::vector<uint32_t>()),
    matchedSectors(checkMatchedSectors ? cfg.getParameter<  std::vector<uint32_t> >("matchedSectors") : std::vector<uint32_t>())
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

bool AnaSiMuDistribution::filter(const EventObj* ev, 
				 const TrackObj * simu, 
				 const HitSpecObj * hitSpec,
				 const HitSpecObj * hitSpecProp
				 ){
  if (!hitSpec) return false;
  if (!simu) return false;

  hSiMuPt_INP->Fill(simu->pt());
  hSiMuEta_INP->Fill(simu->eta());
  hSiMuPhi_INP->Fill(simu->phi());

  uint32_t rawId = hitSpec->rawId();

  if (fabs(simu->eta()) < etaMinRef) return false;
  if (fabs(simu->eta()) > etaMaxRef) return false;

  //if (simu->phi() < -0.35) return false;
  //if (simu->phi() > -0.25) return false;

  //if (simu->phi() > 0.62) return false;
  //if (simu->phi() < 0.59) return false;

  /*
  if(hitSpec &&       
     hitSpec->position().phi()>0.3 &&
     hitSpec->position().phi()<0.35) return true;
  else return false;
  */
  /*
  if(rawId<1 && hitSpecProp &&       
     hitSpecProp->position().phi()>-0.1 &&
     hitSpecProp->position().phi()<0.4) return true;
  else return false;
  */

  int rpcSector = 0;
  int rpcSubsector = 0;
  int rpcRegion = 0;
  if(checkMatchedSectors){
    //If there is no hit in reference station,
    //accept only events with propagated phi near the
    ///phi acceptance hole
    ///This is reasonable only for muons with pt>~5,
    ///as lower pt muons do not cross ref layer at all
    if(rawId<1 && hitSpecProp){       
      if(simu->pt()>5 && 
	 (hitSpecProp->position().phi()>-0.5 &&
	  hitSpecProp->position().phi()<0.5)
	 ){
	 hSiMuPt_DIS->Fill(simu->pt());
	 hSiMuEta_DIS->Fill(simu->eta());
	 hSiMuPhi_DIS->Fill(simu->phi());	 
	return true;
      }
      else return false;
    }
    
    RPCDetId aId(rawId);
    
    if(aId.region()==0){
      matchedSectors.clear(); 
      matchedSectors.push_back(1);
      matchedSectors.push_back(12);
    }
    else{
      matchedSectors.clear(); 
      matchedSectors.push_back(1);
      matchedSectors.push_back(6);
    } 
    rpcSector = aId.sector();
    rpcSubsector = aId.subsector();
    rpcRegion = aId.region();
  }
  
  if (simu->pt() < ptMin) return false;
  if (simu->pt() > ptMax) return false;
  //if ( fabs(hitSpec->position().eta()) < etaMinRef) return false;
  //if ( fabs(hitSpec->position().eta()) > etaMaxRef) return false;
  
  if ( hitSpec->position().phi() < phiMinRef) return false;
  if ( hitSpec->position().phi() > phiMaxRef) return false;

  if (checkMatchedDets && matchedDets.end()==find(matchedDets.begin(),matchedDets.end(), rawId) ) return false;
  if (checkMatchedSectors && matchedSectors.end()==find(matchedSectors.begin(),matchedSectors.end(), rpcSector) ) return false;
  if (checkMatchedSectors &&  rpcRegion!=0 && 
	((rpcSector==6 && rpcSubsector<3) ||
	(rpcSector==1 && rpcSubsector>2))
	)  return false;

  hSiMuPt_DIS->Fill(simu->pt());
  hSiMuEta_DIS->Fill(simu->eta());
  hSiMuPhi_DIS->Fill(simu->phi());

  return true;
}

