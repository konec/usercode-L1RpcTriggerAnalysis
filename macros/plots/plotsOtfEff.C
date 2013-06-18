#include "TROOT.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TVirtualPad.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLegend.h"

#include <iostream>
#include <sstream>
#include "utilsPlotsSaver.h"

TCanvas* pOtfEff(bool logY)
{
  TCanvas *c;
  if (logY) {
   c = new TCanvas("cOtfEff_Log","L1 OTF efficiency",600,600);
   c->SetLogy(1);
  } else {
    c = new TCanvas("cOtfEff_Lin","L1 OTF efficiency",600,600);
    c->SetLogy(0);
  }
  c->SetLogx(1); c->SetGrid(0,1);
  TH1D* hN =  (TH1D*)gROOT->FindObject("hOtfEffPt_N");
  TH1D* hN0 =  (TH1D*)gROOT->FindObject("hOtfEffPt0_N");
  TH1D* hD =  (TH1D*)gROOT->FindObject("hOtfEffPt_D");
  std::cout <<" adresy: "<<hN<<" "<< hN0<<" "<< hD << std::endl;
  TH1D* h = (TH1D*)hD->Clone("hOtfEff_Res");
  TH1D* h0 = (TH1D*)hD->Clone("hOtfEff_Res0");
  h->SetDirectory(0);
  h0->SetDirectory(0);
  h->Reset();
  h0->Reset();
  h->Divide(hN,hD,1.,1.,"B");
  h0->Divide(hN0,hD,1.,1.,"B");
  h->SetLineColor(kRed);
  h->SetLineStyle(1);
  h->SetLineWidth(2);
  h->SetMarkerStyle(24);
  h->SetMarkerSize(1.6);
  h->SetMarkerColor(kRed);
  h->GetXaxis()->SetRange(4,32);
  h->SetMaximum(1.1);
  h->SetMinimum(1.e-4);
  h->DrawCopy();
  h0->SetLineColor(kBlue);
  h0->DrawCopy("same");
  h->Delete();
  return c; 
}


void plotsOtfEff()
{
 utilsPlotsSaver( pOtfEff(false) );
 utilsPlotsSaver( pOtfEff(true) );
}

