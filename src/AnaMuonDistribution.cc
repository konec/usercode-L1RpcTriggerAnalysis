#include "UserCode/L1RpcTriggerAnalysis/interface/AnaMuonDistribution.h"

#include "TProfile.h"
#include "TObjArray.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include <math.h>

using namespace std;
  TH1D *hMuonPt, *hMuonEta, *hMuonPhi;
  TH2D *hMuonPtVsEta_Tk, *hMuonPtVsEta_Ma, *hMuonPtVsEta_Gl;

AnaMuonDistribution::AnaMuonDistribution(TObjArray& histos) 
{
  hMuonPt  = new TH1D("hMuonPt","All global muons Pt;Glb.muon p_{T} [GeV];Muons / bin",L1PtScale::nPtBins,L1PtScale::ptBins);  histos.Add(hMuonPt);
  hMuonEta = new TH1D("hMuonEta","All global muons Eta;Glb.muon #eta;Muons / bin",64, -1.6, 1.6);  histos.Add(hMuonEta);
  hMuonPhi = new TH1D("hMuonPhi","All global muons Phi;Glb.muon #phi [rad];Muons / bin",90,-M_PI,M_PI);  histos.Add(hMuonPhi);

  hMuonPtVsEta_Tk = new TH2D("hMuonPtVsEta_Tk","hMuonPtVsEta_Tk", L1RpcEtaScale::nEtaBins, L1RpcEtaScale::etaBins, L1PtScale::nPtBins, L1PtScale::ptBins); histos.Add(hMuonPtVsEta_Tk);
  hMuonPtVsEta_Ma = new TH2D("hMuonPtVsEta_Ma","hMuonPtVsEta_Ma", L1RpcEtaScale::nEtaBins, L1RpcEtaScale::etaBins, L1PtScale::nPtBins, L1PtScale::ptBins); histos.Add(hMuonPtVsEta_Ma);
  hMuonPtVsEta_Gl = new TH2D("hMuonPtVsEta_Gl","hMuonPtVsEta_Gl", L1RpcEtaScale::nEtaBins, L1RpcEtaScale::etaBins, L1PtScale::nPtBins, L1PtScale::ptBins); histos.Add(hMuonPtVsEta_Gl);

}

void AnaMuonDistribution::run(const MuonObj *muon)
{
  hMuonPt->Fill(muon->pt());
  hMuonEta->Fill(muon->eta());
  hMuonPhi->Fill(muon->phi());
  hMuonPtVsEta_Tk->Fill(muon->eta(), muon->pt());
  if (muon->isMatched() ) hMuonPtVsEta_Ma->Fill(muon->eta(), muon->pt());
  if (muon->isGlobal()) hMuonPtVsEta_Gl->Fill(muon->eta(), muon->pt());
}
