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
#include <algorithm>
#include "utilsPlotsSaver.h"

TCanvas* pEventBx()
{
  TCanvas* c = new TCanvas("cEventBX","cEventBX",1500,800);
  c->Divide(1,2);
  TVirtualPad * pad = c->cd(1);
  pad->SetLogy();
  TH1D* h = (TH1D*)gROOT->FindObject("hEvent_BX_val");
  //TH1D* h = (TH1D*)gROOT->FindObject("hEvent_BX");
  //h->GetXaxis()->SetRange(1,300);
  if (h) {
    h->SetMinimum(0.5);
     h->DrawCopy();
  }
  pad =  c->cd(2);
  pad->SetLogy();
  TH1D* h_nm2 = (TH1D*)gROOT->FindObject("hEvent_BX_nm2");
  h_nm2->SetMinimum(0.5);
  h_nm2->SetLineColor(2);
//  h_nm2->GetXaxis()->SetRange(1,1000);
  h_nm2->DrawCopy();
  TH1D* h_np2 = (TH1D*)gROOT->FindObject("hEvent_BX_np2");
  h_np2->SetLineColor(4);
  h_np2->DrawCopy("same");

  std::vector <int> hits;
  for (int i=1; i <= h->GetNbinsX(); i++) if (h->GetBinContent(i) > 0) hits.push_back( i-1); 
  std::sort(hits.begin(), hits.end());

//  int i=0;  
//  for ( std::vector <int>::const_iterator it=hits.begin(); it != hits.end(); it++) { 
//   std::cout <<*it<<", ";
//   if (++i > 20) {std::cout << std::endl; i=0; }
//  }
//  std::cout << std::endl;
  std::cout<<"SIZE IS:"<<hits.size() << std::endl;
  return c;
}

void plotsEvent()
{
   utilsPlotsSaver( pEventBx() );
}
