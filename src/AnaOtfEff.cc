#include "UserCode/L1RpcTriggerAnalysis/interface/AnaOtfEff.h"

#include "TObjArray.h"
#include "TH1D.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"

namespace {
  TH1D* hOtfEffPt_D, *hOtfEffPt_N, *hOtfEffPt0_N;
}

AnaOtfEff::AnaOtfEff(const edm::ParameterSet& cfg)
  : debug(false),
    ptCut( cfg.getParameter<double>("ptCut") )
{ }

void AnaOtfEff::init(TObjArray& histos)
{
  hOtfEffPt_D = new TH1D("hOtfEffPt_D","hOtfEffPt_D",L1PtScale::nPtBins, L1PtScale::ptBins); histos.Add(hOtfEffPt_D);
  hOtfEffPt0_N = new TH1D("hOtfEffPt0_N","hOtfEffPt0_N",L1PtScale::nPtBins, L1PtScale::ptBins); histos.Add(hOtfEffPt0_N);
  hOtfEffPt_N = new TH1D("hOtfEffPt_N","hOtfEffPt_N",L1PtScale::nPtBins, L1PtScale::ptBins); histos.Add(hOtfEffPt_N);
}

void AnaOtfEff::run(const EventObj* event, const TrackObj *muon, const L1Obj& l1)
{
//  double etaMu = fabs(muon->eta());
  double ptMu  = muon->pt();

  hOtfEffPt_D->Fill(ptMu);
  if (l1.isValid() && l1.type==L1Obj::OTF && l1.pt > ptCut) hOtfEffPt_N->Fill(ptMu);
  if (l1.isValid() && l1.type==L1Obj::OTF && l1.pt > 0.)    hOtfEffPt0_N->Fill(ptMu);

}

