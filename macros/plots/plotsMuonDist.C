#include "TROOT.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TVirtualPad.h"
#include "TPaveStats.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGaxis.h"
#include "TFile.h"

#include "utilsPlotsSaver.h"

TCanvas* pMuonDist_1D()
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  //TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  //ff->cd();

  TCanvas * c = new TCanvas("cMuonDist_1D","Global muons distributions",1400,500);
  c->Divide(3,1);

  c->cd(1); 
  gPad->SetYstat(0.5);
  TH1D* hMuonPt=(TH1D*)gROOT->FindObject("hMuonPt"); 
  if(!hMuonPt) return 0;
  hMuonPt->SetXTitle("Glb.muon p_{T} [GeV/c]");
  hMuonPt->DrawCopy();

  c->cd(2); 
  TH1D* hMuonEta=(TH1D*)gROOT->FindObject("hMuonEta");
  if(!hMuonEta) return 0;
  hMuonEta->SetMinimum(0.);
  hMuonEta->SetXTitle("Glb.muon #eta");
  hMuonEta->DrawCopy();

  c->cd(3); 
  TH1D* hMuonPhi=(TH1D*)gROOT->FindObject("hMuonPhi");
  if(!hMuonPhi) return 0;
  hMuonPhi->SetMinimum(0.);
  hMuonPhi->SetXTitle("Glb.muon #phi [rad]");
  TH1D *hcpy = (TH1D*)hMuonPhi->DrawCopy();
  // for phi distribution move statistics box down
  gPad->Update();
  TPaveStats *st = (TPaveStats*)hcpy->FindObject("stats");
  double y1=st->GetY1NDC(), y2=st->GetY2NDC();
  st->SetY1NDC(y1-0.4);
  st->SetY2NDC(y2-0.4);
  
  return c;
}

TCanvas* pMuonDist_PtVsEtaDist()
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  //TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  //ff->cd();

  TCanvas * c = new TCanvas("cMuonDist_PtVsEtaDist","Global muons pt-eta distributions",1400,500);
  c->Divide(3,1);

  TVirtualPad* p1 = c->cd(1); p1->SetLogy(1); 
  TH2D* hMuonPtVsEta_Tk =  (TH2D*)gROOT->FindObject("hMuonPtVsEta_Tk");
  if(!hMuonPtVsEta_Tk) return 0;
  hMuonPtVsEta_Tk->SetAxisRange(2.,140.,"Y");
  hMuonPtVsEta_Tk->SetXTitle("Glb.muon #eta");
  hMuonPtVsEta_Tk->SetYTitle("Glb.muon p_{T} [GeV/c]");
  hMuonPtVsEta_Tk->DrawCopy("box");

  TVirtualPad* p2 = c->cd(2); p2->SetLogy(1); 
  TH2D* hMuonPtVsEta_Ma =  (TH2D*)gROOT->FindObject("hMuonPtVsEta_Ma"); 
  if(!hMuonPtVsEta_Ma) return 0;
  hMuonPtVsEta_Ma->SetAxisRange(2.,140.,"Y");
  hMuonPtVsEta_Ma->SetXTitle("Glb.muon #eta");
  hMuonPtVsEta_Ma->SetYTitle("Glb.muon p_{T} [GeV/c]");
  hMuonPtVsEta_Ma->DrawCopy("box");

  TVirtualPad* p3 = c->cd(3); p3->SetLogy(1); 
  TH2D* hMuonPtVsEta_Gl =  (TH2D*)gROOT->FindObject("hMuonPtVsEta_Gl");
  if(!hMuonPtVsEta_Gl) return 0;
  hMuonPtVsEta_Gl->SetAxisRange(2.,140.,"Y");
  hMuonPtVsEta_Gl->SetXTitle("Glb.muon #eta");
  hMuonPtVsEta_Gl->SetYTitle("Glb.muon p_{T} [GeV/c]");
  hMuonPtVsEta_Gl->DrawCopy("box");
  return c;
}

void plotsMuonDist()
{
  // store current gStyle options
  int stat=gStyle->GetOptStat();
  float x=gStyle->GetTitleX();
  int digits=TGaxis::GetMaxDigits(); // for TAxis labels

  // make and save plots
  gStyle->SetOptStat(1);
  gStyle->SetTitleX(0.28);
  TGaxis::SetMaxDigits(4); // global option
  utilsPlotsSaver( pMuonDist_1D() );
  gStyle->SetTitleX(0.);
  utilsPlotsSaver( pMuonDist_PtVsEtaDist() );

  // restore previous gStyle settings
  gStyle->SetOptStat(stat);
  gStyle->SetTitleX(x);
  TGaxis::SetMaxDigits(digits);

}
