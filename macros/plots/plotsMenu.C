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
#include <map>
#include "utilsPlotsSaver.h"

TCanvas* pMenu(std::string what)
{
  TCanvas* c = new TCanvas(("cMenu"+what).c_str(),("cMenu"+what).c_str(),1500,400); 
  c->SetBottomMargin(0.6);
  c->SetLeftMargin(0.04);
  c->SetRightMargin(0.01);
  TH1D* h = (TH1D*)gROOT->FindObject(("hMenuAlgos"+what).c_str());
  h->SetLineColor(4);
  h->SetFillColor(4);
  h->SetFillStyle(3003);
  h->LabelsOption("v");
  h->GetYaxis()->SetLabelOffset(0.005);
//  h->GetXaxis()->SetRange(550,680);
  h->DrawCopy();
  return c;
}
void plotsMenu()
{
  int digits=TGaxis::GetMaxDigits();
  TGaxis::SetMaxDigits(3);
  utilsPlotsSaver(pMenu("L1"));
  utilsPlotsSaver(pMenu("HLT"));
  TGaxis::SetMaxDigits(digits);
}

