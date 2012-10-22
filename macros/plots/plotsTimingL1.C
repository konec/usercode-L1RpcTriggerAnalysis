#include "TROOT.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TText.h"
#include "TLatex.h"
#include "TString.h"
#include "TGaxis.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <map>
#include "utilsPlotsSaver.h"
#include "utilsHistoFromGraph.h"

TCanvas* pTimingL1(const std::string& what)
{
  std::string timing = "TimingL1_";
  TCanvas* c = new TCanvas( ("p"+timing+what).c_str(),(timing+what).c_str() ,600,600);
  c->SetLogy();
  c->SetGrid(0,1);

  TH1D* h = (TH1D*)gROOT->FindObject( ("h"+timing+what).c_str() );
  double atPeak = h->GetBinContent(3);
  h->Scale(1./atPeak);
  h->SetLineWidth(2);
  h->SetMarkerStyle(1);
  h->SetLineColor(4);
  h->SetFillColor(4);
  h->SetFillStyle(3003);
  h->GetXaxis()->SetNdivisions(5);
  h->GetXaxis()->SetTitle("First candidate arrival [BX]");
  h->GetYaxis()->SetTitle("Fraction of triggers");

  h->DrawCopy("hist text");

  TLatex t;
  t.SetNDC(true);t.SetTextSize( 0.05); t.SetTextColor(2); 
  t.SetText(0.2, 0.8, what.c_str()); t.DrawClone();

  return c;
}

TCanvas* pTimingL1Corr(const std::string& what)
{
  std::string timing = "TimingL1_";
  TCanvas* c = new TCanvas( ("p"+timing+what).c_str(),(timing+what).c_str() ,600,600);
  TH2D* h = (TH2D*)gROOT->FindObject( ("h"+timing+what).c_str() );
  h->SetLineWidth(2);
  h->SetLineColor(4);
  h->GetXaxis()->SetNdivisions(5);
  h->GetYaxis()->SetNdivisions(5);
  h->GetXaxis()->SetTitle("First RPC candidate arrival [BX]");
  if (what=="RpcVsDt") h->GetYaxis()->SetTitle("First DT  candidate arrical [BX]");
  if (what=="RpcVsCsc") h->GetYaxis()->SetTitle("First CSC  candidate arrical [BX]");

  h->DrawCopy("box text");

  TLatex t;
  t.SetNDC(true);t.SetTextSize( 0.05); t.SetTextColor(2);
  t.SetText(0.2, 0.8, what.c_str()); t.DrawClone();

  return c;
}


void plotsTimingL1()
{
  utilsPlotsSaver(pTimingL1("Rpc"));   
  utilsPlotsSaver(pTimingL1("Dt"));   
  utilsPlotsSaver(pTimingL1("Csc"));   
  utilsPlotsSaver(pTimingL1Corr("RpcVsDt"));
  utilsPlotsSaver(pTimingL1Corr("RpcVsCsc"));
}
