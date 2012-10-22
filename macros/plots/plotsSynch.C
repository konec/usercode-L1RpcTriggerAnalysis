#include "TROOT.h"

#include "TCanvas.h"
#include "TStyle.h"
#include "TVirtualPad.h"
#include "TText.h"
#include "TLatex.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TGaxis.h"
#include "TGraph.h"
#include <iostream>
#include <sstream>

#include "utilsPlotsSaver.h"
#include "utilsHistoFromGraph.h"
#include "utilsObjArray.h"


TCanvas* pSynch_delaySummary() 
{
  TCanvas * c = new TCanvas("cSynch_delaySummary","cSynch_delaySummary",600,600);
  TH1D* h = (TH1D*)gROOT->FindObject("hSynch_delaySummary");

  double integral = h->Integral();
  h->SetTitle("RPC hit synchronization;Chamber hit timing wrt L1A [BX];Hits / bin");
  h->Sumw2();
  h->Scale(1.0/integral); // normalize to 1
  h->SetYTitle("Fraction of hits");
  h->GetXaxis()->SetRange(1,7);


  h->SetTitleSize(0.045,"Y");
  h->SetTitleSize(0.045,"X");
  h->SetTitleOffset(1.5,"Y");
  h->SetTitleOffset(1.2,"X");

  h->SetMarkerSize(1.7);  // for TEXT bin values
  h->SetMarkerColor(4);   // for TEXT bin valued
//  h->SetStats(false);


  double meanX = h->GetMean();
  double rmsX = h->GetRMS();

  h->SetLineStyle(1);
  h->SetLineWidth(2);
  h->SetMarkerStyle(1);
  h->SetLineColor(4);
  h->SetFillColor(4); 
  h->SetFillStyle(3003);
  h->GetXaxis()->SetNdivisions(10);
  h->DrawCopy("E1 E0");
  h->DrawCopy("HIST SAME");

  c->SetLeftMargin(0.17);
  c->SetBottomMargin(0.15);
  c->SetGrid(0,1);
  c->SetRightMargin(0.02);
  c->SetLogy();

  double xpos=0.20;
  double ypos=0.85;
  unsigned int ioff=0;
  const Float_t legendFontSize = 0.030;
  const Float_t legendOffset   = 0.030;

  char label[200];
  TLatex t;
  t.SetNDC(true);t.SetTextSize(legendFontSize); 
  // avg
  sprintf(label,"Avg time = %6.4f BX", meanX);
  t.SetText(xpos, ypos-ioff*legendOffset, label);  ioff++;
  t.SetTextColor(2); t.DrawClone();
  //rms
  sprintf(label,"Rms = %6.4f BX", rmsX);
  t.SetText(xpos, ypos-ioff*legendOffset, label);  ioff++;
  t.SetTextColor(4); t.DrawClone();

  return c;
}

TCanvas* pSynch_delaySpread()
{
  TCanvas * c = new TCanvas("cSynch_delaySpread","cSynch_delaySpread",600,600);
  TH2D* h = (TH2D*)gROOT->FindObject("hSynch_delaySpread");

  int nLB_total = (int) h->Integral()-1;
  int nLB_dead = (int) h->GetBinContent(1,1) -1;
  gStyle->SetPalette(1);
  c->SetLogz();
  c->SetGrid(1,1);
  c->SetRightMargin(0.2);
  c->SetLeftMargin(0.17);
  c->SetBottomMargin(0.15);
  c->Draw();
  h->SetTitleSize(0.045,"Y");
  h->SetTitleSize(0.045,"X");
  h->SetTitleSize(0.045,"Z");
  h->SetLabelSize(0.038,"X");
  h->SetLabelSize(0.038,"Y");
  h->SetLabelSize(0.038,"Z");
  h->SetTitleOffset(1.5,"Y");
  h->SetTitleOffset(1.2,"X");
  h->SetTitleOffset(1.2,"Z");
  h->SetMaximum(nLB_total);
  h->GetXaxis()->CenterTitle(true);
  h->SetMarkerSize(1.0);  // for TEXT bin values
  h->SetMarkerColor(1);   // for TEXT bin valued
  h->SetLineStyle(1);
  h->SetMarkerStyle(1);
  h->SetStats(false);

  h->GetXaxis()->SetRangeUser(-0.12, 0.12);
  h->GetYaxis()->SetRangeUser(0.0, 0.45);

  h->GetXaxis()->SetNdivisions(410);
  h->GetYaxis()->SetNdivisions(410);
  h->DrawCopy("COLZ");
  h->DrawCopy("TEXT0,SAME");

  double xpos=0.20;
  double ypos=0.85;
  unsigned int ioff=0;
  const Float_t legendFontSize = 0.030;
  const Float_t legendOffset   = 0.030;

  char label[200];
  TLatex t;
  t.SetNDC(true);t.SetTextSize(legendFontSize);t.SetTextColor(4); 

  t.SetText(xpos, ypos-ioff*legendOffset, "Number of Link Boards:");  ioff++;
  t.DrawClone();
  sprintf(label,"%d - total", nLB_total);
  t.SetText(xpos, ypos-ioff*legendOffset, label);  ioff++;
  t.DrawClone();
  sprintf(label,"%d - w/o hits", nLB_dead);
  t.SetText(xpos, ypos-ioff*legendOffset, label);  ioff++;
  t.DrawClone();

  return c;
}

TCanvas* pSynch_top10(const std::string & what)
{
  
  std::string name = "Synch_top"+what;
  TCanvas * c = new TCanvas( ("c"+name).c_str(),("c"+name).c_str(),650,600);
  
  TH2D* h=(TH2D*)(gROOT->FindObject( ("h"+name).c_str()) );
  int PEAK_BIN = 3;
  for(Int_t nbx=0; nbx<h->GetNbinsX(); nbx++) {
    char label[5];
    sprintf(label,"%d",nbx-PEAK_BIN);
    h->GetXaxis()->SetBinLabel(nbx+1,label);
  }
  h->SetTitle( ("Top 10" + what + " LBs").c_str() );
  h->GetXaxis()->SetTitle("Chamber hit timing wrt L1 [BX]");
  h->GetXaxis()->SetRange(1,7);
  c->SetLeftMargin(0.4);
  h->SetStats(0);
  h->SetFillStyle(0);
  h->SetMarkerSize(1.5);
  h->SetMarkerColor(4);
  h->SetTitleSize(0.04,"X");
  h->SetTitleOffset(1.05,"X");
  h->LabelsOption("h","X");

  h->DrawCopy("TEXT0");
  return c;
}

TCanvas* pSynch_NotComplete()
{
  TCanvas * c = new TCanvas("cSynch_notComplete","cSynch_notComplete",1200,300);
  c->Divide(3,1);
  for (unsigned int fed=790; fed <=792; fed++) {
    TVirtualPad * pad = c->cd(fed-789);
    pad->SetTopMargin(0.1);
    pad->SetGrid(1,1);
    std::stringstream s; 
    s<<"hSynch_notComplete"<<fed;
    TH1D * h = (TH1D*)(gROOT->FindObject( s.str().c_str()) );
    h->GetYaxis()->SetNdivisions(510);
    h->GetYaxis()->SetNdivisions(605);
    h->GetXaxis()->SetTitleOffset(1.0);
    h->GetYaxis()->SetTitleOffset(1.0);
    h->DrawCopy("box");
    s.str(""); s<<"Fed: "<<fed;
    TText t; t.SetTextColor(4); t.SetTextAlign(11); t.SetTextSize(0.06);
    t.DrawTextNDC(0.16,0.92, s.str().c_str());

  }
  return c;
}

void plotsSynch()
{
  utilsPlotsSaver( pSynch_top10("Spread") );
  utilsPlotsSaver( pSynch_top10("Occup") );
  utilsPlotsSaver( pSynch_NotComplete() );
  utilsPlotsSaver( pSynch_delaySummary() );
  utilsPlotsSaver( pSynch_delaySpread() );
}
