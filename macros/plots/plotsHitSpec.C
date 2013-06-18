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
#include "TVirtualPad.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include "utilsPlotsSaver.h"
TCanvas* pHitSpecDet(const std::string& s)
{
  TCanvas* c = new TCanvas( ("HitSpecDet"+s).c_str(), ("HitSpecDet"+s).c_str(), -2);
  c->SetLogx();
  TH2D* h = (TH2D*)gROOT->FindObject(s.c_str());
  h->GetXaxis()->SetRange(4,32);
  std::stringstream str;
  str<<h->GetTitle();
  TText t;
  t.SetTextColor(4);
  t.SetTextAlign(11);
  t.SetTextSize(0.035);
  h->DrawCopy("box");
  t.DrawTextNDC(0.17,0.2, str.str().c_str());
  return c;
}

TCanvas* pHitSpecPos( )
{
  TCanvas* c = new TCanvas("cHitSpecPos","cHitSpecPos",1000,1100);
  c->Divide(2,2);
  TVirtualPad* p; TH2D *h;
  TText t; t.SetTextColor(4);

  p =c->cd(1); p->SetLogy(); p->SetGrid(1,0);
  h = (TH2D*)gROOT->FindObject("hHitSpec_PtVsPhi_BarMinus"); 
  h->GetYaxis()->SetRange(4,33);
  h->DrawCopy("box");
  t.DrawTextNDC(0.17,0.15, "BARREL MU MINUS");

  p =c->cd(2); p->SetLogy(); p->SetGrid(1,0);
  h = (TH2D*)gROOT->FindObject("hHitSpec_PtVsPhi_BarPlus"); 
  h->GetYaxis()->SetRange(4,32);
  h->DrawCopy("box");
  t.DrawTextNDC(0.17,0.15, "BARREL MU PLUS");

  p =c->cd(3); p->SetLogy(); p->SetGrid(1,0);
  h = (TH2D*)gROOT->FindObject("hHitSpec_PtVsPhi_EndMinus"); 
  h->GetYaxis()->SetRange(4,32);
  h->DrawCopy("box");
  t.DrawTextNDC(0.17,0.15, "ENDCAP MU MINUS");

  p =c->cd(4); p->SetLogy(); p->SetGrid(1,0);
  h = (TH2D*)gROOT->FindObject("hHitSpec_PtVsPhi_EndPlus"); 
  h->GetYaxis()->SetRange(4,32);
  h->DrawCopy("box");
  t.DrawTextNDC(0.17,0.15, "ENDCAP MU PLUS");

  return c;
}

TCanvas* pHitSpecPosGen( )
{
  TCanvas* c = new TCanvas("cHitSpecPosGen","cHitSpecPosGen",1200,600);
  c->Divide(2,1);
  TVirtualPad* p; TH2D *h; TH2D *hAtPhi0;
  TText t; t.SetTextColor(4);

  p =c->cd(1); p->SetLogy(); p->SetGrid(0,1);
  h = (TH2D*)gROOT->FindObject("hHitSpec_PtVsPhiGen_Minus"); 
  hAtPhi0 = (TH2D*)gROOT->FindObject("hHitSpec_PtVsPhiGenAtPhi0_Minus"); 
  h->GetYaxis()->SetRange(4,33);
  h->GetXaxis()->SetNdivisions(505); h->GetXaxis()->SetLabelSize(0.04);
  h->DrawCopy("box");
  hAtPhi0->SetLineColor(2);
  hAtPhi0->DrawCopy("box same");
  t.DrawTextNDC(0.17,0.15, "BARREL MU MINUS");
  std::cout <<h->GetTitle() << std::endl;
  for (int iy = 1; iy <=h->GetNbinsY(); iy++) {
  std::cout <<" pt: " << h->GetYaxis()->GetBinLowEdge(iy);
  double xmin=100.;
  double xAtMax = 0.; double valAtMax = 0.;
  double xmax=-100.;
  for (int ix = 1; ix <=h->GetNbinsX(); ix++) {
     double val = h->GetBinContent(ix,iy); 
     double xbmin = h->GetXaxis()->GetBinLowEdge(ix);
     double xcent = h->GetXaxis()->GetBinCenter(ix);
     double xbmax = h->GetXaxis()->GetBinUpEdge(ix);
     if (val > valAtMax) { valAtMax = val; xAtMax = xcent; }
     if (val > 1 && (xbmin < xmin)) xmin = xbmin;    
     if (val > 1 && (xbmax > xmax)) xmax = xbmax;    
  }
  std::cout <<"  set DPHI0 = "<<1.025-xAtMax 
            <<"; set DPHI_MARGIN = " <<std::max( (xAtMax-xmin), (xmax-xAtMax))
<<";" << std::endl;
  }

  p =c->cd(2); p->SetLogy(); p->SetGrid(1,1);
  h = (TH2D*)gROOT->FindObject("hHitSpec_PtVsPhiGen_Plus"); 
  hAtPhi0 = (TH2D*)gROOT->FindObject("hHitSpec_PtVsPhiGenAtPhi0_Plus"); 
  h->GetYaxis()->SetRange(4,32);
  h->GetXaxis()->SetNdivisions(505); h->GetXaxis()->SetLabelSize(0.04);
  h->DrawCopy("box");
  hAtPhi0->SetLineColor(2);
  hAtPhi0->DrawCopy("box same");
  t.DrawTextNDC(0.17,0.15, "BARREL MU PLUS");
  std::cout <<h->GetTitle() << std::endl;
  for (int iy = 1; iy <=h->GetNbinsY(); iy++) {
  std::cout <<" pt: " << h->GetYaxis()->GetBinLowEdge(iy);
  double xmin=100.;
  double xAtMax = 0.; double valAtMax = 0.;
  double xmax=-100.;
  for (int ix = 1; ix <=h->GetNbinsX(); ix++) {
     double val = h->GetBinContent(ix,iy); 
     double xbmin = h->GetXaxis()->GetBinLowEdge(ix);
     double xcent = h->GetXaxis()->GetBinCenter(ix);
     double xbmax = h->GetXaxis()->GetBinUpEdge(ix);
     if (val > valAtMax) { valAtMax = val; xAtMax = xcent; }
     if (val > 1 && (xbmin < xmin)) xmin = xbmin;    
     if (val > 1 && (xbmax > xmax)) xmax = xbmax;    
  }
  std::cout <<"  set DPHI0 = "<<1.025-xAtMax 
            <<"; set DPHI_MARGIN = " <<std::max( (xAtMax-xmin), (xmax-xAtMax))
<<";" << std::endl;
  }

  return c;
}


void plotsHitSpec()
{
  utilsPlotsSaver(pHitSpecPos());
  utilsPlotsSaver(pHitSpecPosGen());
  utilsPlotsSaver( pHitSpecDet("hHitSpec_637602109"));
  utilsPlotsSaver( pHitSpecDet("hHitSpec_637634877"));
  utilsPlotsSaver( pHitSpecDet("hHitSpec_637599914"));
  utilsPlotsSaver( pHitSpecDet("hHitSpec_637632682"));

//  utilsPlotsSaver(pHitSpecDet("hHitSpec_637634749"));
//  utilsPlotsSaver(pHitSpecDet("hHitSpec_637601981"));
//  utilsPlotsSaver(pHitSpecDet("hHitSpec_637620266"));
//  utilsPlotsSaver(pHitSpecDet("hHitSpec_637616170"));
//  utilsPlotsSaver(pHitSpecDet("hHitSpec_637653034"));
//  utilsPlotsSaver(pHitSpecDet("hHitSpec_637648938"));
}
