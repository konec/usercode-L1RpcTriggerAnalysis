#include "TROOT.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TText.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TString.h"
#include "TGaxis.h"
#include "TVirtualPad.h"
#include <iomanip>
#include <sstream>
#include <cmath>
#include "utilsPlotsSaver.h"

TCanvas* pSiMuDist_1D( )
{

  TCanvas * c = new TCanvas("cSiMuDist_1D","Global SiMus distributions",1400,500);
  c->Divide(3,1);
//
// p1
//
  TVirtualPad * p1 = c->cd(1); p1->SetLogy(1); p1->SetLogx(1);
  TH1D* hSiMuPt_DIS = (TH1D*) gROOT->FindObject("hSiMuPt_DIS");
  TH1D* hSiMuPt_INP = (TH1D*) gROOT->FindObject("hSiMuPt_INP");
  hSiMuPt_INP->SetMinimum(0.9);
  hSiMuPt_INP->SetXTitle("simu p_{T} [GeV/c]");
  hSiMuPt_INP->SetLineColor(2);
  hSiMuPt_INP->GetXaxis()->SetRange(3,33);
  hSiMuPt_INP->DrawCopy();

  TLegend l(0.3, 0.15, 0.93,.28);
  int nev;
  std::stringstream str;

  nev= hSiMuPt_INP->GetEntries();
  str.str(""); str<<"all SiMus (ev: "<<std::setprecision(2) << scientific << (double) nev<<")";
  l.AddEntry(hSiMuPt_INP, str.str().c_str());

  nev= hSiMuPt_DIS->GetEntries();
  str.str(""); str<<"kinematic sel. (ev: "<< std::setprecision(2) << scientific << (double)nev<<")";
  hSiMuPt_DIS->SetLineColor(8);
  hSiMuPt_DIS->DrawCopy("same");
  l.AddEntry(hSiMuPt_DIS, str.str().c_str() );

  l.DrawClone("same");

//
// p2
//
  TVirtualPad * p2 = c->cd(2); p2->SetLogy(1);
  TH1D* hSiMuEta_DIS=(TH1D*)gROOT->FindObject("hSiMuEta_DIS");
  TH1D* hSiMuEta_INP=(TH1D*)gROOT->FindObject("hSiMuEta_INP");
  hSiMuEta_DIS->SetMaximum( 1.2*hSiMuEta_INP->GetMaximum());
  hSiMuEta_DIS->SetLineColor(8);
  hSiMuEta_DIS->SetXTitle("simu #eta");
  hSiMuEta_DIS->DrawCopy();
  hSiMuEta_INP->SetLineColor(2);
  hSiMuEta_INP->DrawCopy("same");

//
// p3
//
  c->cd(3);
  TH1D* hSiMuPhi_DIS=(TH1D*)gROOT->FindObject("hSiMuPhi_DIS");
  TH1D* hSiMuPhi_INP=(TH1D*)gROOT->FindObject("hSiMuPhi_INP");
  hSiMuPhi_DIS->Scale(1./hSiMuPhi_DIS->Integral());
  hSiMuPhi_DIS->SetMinimum(0.);
  hSiMuPhi_DIS->SetXTitle("simu #phi [rad]");
  hSiMuPhi_DIS->SetYTitle("arbitrary units");
  hSiMuPhi_DIS->SetLineColor(8);
  hSiMuPhi_DIS->DrawCopy();
  hSiMuPhi_INP->Scale(1./hSiMuPhi_INP->Integral());
  hSiMuPhi_INP->SetLineColor(2);
  hSiMuPhi_INP->DrawCopy("same");

  return c;
}

void plotsSiMuDistribution()
{
  utilsPlotsSaver( pSiMuDist_1D() );
}

