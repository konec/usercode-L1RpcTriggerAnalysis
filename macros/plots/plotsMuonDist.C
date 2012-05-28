#include "TROOT.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TVirtualPad.h"
#include "TH1D.h"
#include "TH2D.h"

#include "utilsPlotsSaver.C"

TCanvas* pMuonDist1D()
{

  TCanvas * c = new TCanvas("cMuonDist1D","Global muons distributions",1200,400);
  c->Divide(3,1);

  c->cd(1); 
  TH1D* hMuonPt=(TH1D*)gROOT->FindObject("hMuonPt"); 
  hMuonPt->DrawCopy();

  c->cd(2); 
  TH1D* hMuonEta=(TH1D*)gROOT->FindObject("hMuonEta");
  hMuonEta->SetMinimum(0.);
  hMuonEta->DrawCopy();

  c->cd(3);
  TH1D* hMuonPhi=(TH1D*)gROOT->FindObject("hMuonPhi");
  hMuonPhi->SetMinimum(0.);
  hMuonPhi->DrawCopy();
  return c;
}

TCanvas* pMuonDistPtVsEtaDist()
{
  TCanvas * c = new TCanvas("cMuonDistPtVsEtaDist","Global muons pt-eta distributions",1200,400);
  c->Divide(3,1);

  TVirtualPad* p1 = c->cd(1); p1->SetLogy(1); 
  TH2D* hMuonPtVsEta_Tk =  (TH2D*)gROOT->FindObject("hMuonPtVsEta_Tk"); 
  hMuonPtVsEta_Tk->SetAxisRange(2.,140.,"Y");
  hMuonPtVsEta_Tk->DrawCopy("box");

  TVirtualPad* p2 = c->cd(2); p2->SetLogy(1); 
  TH2D* hMuonPtVsEta_Ma =  (TH2D*)gROOT->FindObject("hMuonPtVsEta_Ma"); 
  hMuonPtVsEta_Ma->SetAxisRange(2.,140.,"Y");
  hMuonPtVsEta_Ma->DrawCopy("box");

  TVirtualPad* p3 = c->cd(3); p3->SetLogy(1); 
  TH2D* hMuonPtVsEta_Gl =  (TH2D*)gROOT->FindObject("hMuonPtVsEta_Gl"); 
  hMuonPtVsEta_Gl->SetAxisRange(2.,140.,"Y");
  hMuonPtVsEta_Gl->DrawCopy("box");
  return c;
}

void plotsMuonDist()
{
  gStyle->SetOptStat(1);
  gStyle->SetTitleX(0.28);
  utilsPlotsSaver( pMuonDist1D() );
  gStyle->SetTitleX(0.);
  utilsPlotsSaver( pMuonDistPtVsEtaDist() );
}
