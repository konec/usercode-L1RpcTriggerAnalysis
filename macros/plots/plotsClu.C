#ifndef plotsClu_C
#define plotsClu_C 

#include "TROOT.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TVirtualPad.h"
#include "TPaveStats.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGaxis.h"
#include "TFile.h"
#include "TGraph.h"
#include <iostream>
#include <sstream>
#include <utility>

#include "utilsPlotsSaver.h"
#include "utilsHistoFromGraph.h"
#include "utilsObjArray.h"

TCanvas* pCluDigiRHit()
{
  TCanvas * c = new TCanvas("cCluDigiRHit","cCluDigiRHit",1000,500);
  c->Divide(2,1);
  //TVirtualPad * p = 
  c->cd(1);
  TH2D* hClu_DigiRHit = (TH2D*)gROOT->FindObject("hClu_DigiRHit");
  hClu_DigiRHit->SetXTitle("Cluster from RecHit"); 
  hClu_DigiRHit->SetYTitle("Number of Digis in DU"); 
  hClu_DigiRHit->GetXaxis()->SetRange(1,60);
  hClu_DigiRHit->GetYaxis()->SetRange(1,60);
  hClu_DigiRHit->DrawCopy("box");
  c->cd(2);
  hClu_DigiRHit->GetXaxis()->SetRange(1,10);
  hClu_DigiRHit->GetYaxis()->SetRange(1,10);
  hClu_DigiRHit->DrawCopy("box");
  return c;
}

TCanvas* pCluSize_1D()
{
  TCanvas * c = new TCanvas("cCluSize_1D","cCluSize_1D",1100,500);

  //
  // log scale
  //
  c->Divide(2,1);
  TVirtualPad * p = c->cd(1);
  p->SetLogy();
  p->SetYstat(0.5);
  TH1D* hClu_SizeDigi = (TH1D*)gROOT->FindObject("hClu_SizeDigi");  
  TH1D* hClu_SizeRHit = (TH1D*)gROOT->FindObject("hClu_SizeRHit");  
  hClu_SizeRHit->SetLineColor(2);
  hClu_SizeRHit->SetXTitle("hit strips");
  hClu_SizeRHit->SetYTitle("events [a.u.]");
  hClu_SizeRHit->DrawCopy();
  hClu_SizeDigi->SetLineColor(4);
  hClu_SizeDigi->DrawCopy("same");
  TText t;
  t.SetTextAlign(31);
  t.SetTextSize(0.045);
  std::stringstream mean;
  double meanDigi = static_cast<int>(hClu_SizeDigi->GetMean()*1000)/1000.;
  double meanRHit = static_cast<int>(hClu_SizeRHit->GetMean()*1000)/1000.;
  //mean<<"Mean: "<<hClu_SizeDigi->GetMean();
  mean<<"Mean: "<<meanDigi;
  t.SetTextColor(4);
  t.DrawTextNDC(0.95,0.92, mean.str().c_str());
  mean.str(""); mean<<"Mean: "<<meanRHit;
  t.SetTextColor(2);
  t.SetTextAlign(11);
  t.DrawTextNDC(0.15,0.92, mean.str().c_str());
  //
  // same in lin scale
  //
  p = c->cd(2);
  p->SetLogy(0);
  hClu_SizeRHit->GetXaxis()->SetRange(1,10);
  hClu_SizeRHit->GetYaxis()->SetTitleOffset(1.6);
  hClu_SizeRHit->DrawCopy();
  hClu_SizeDigi->DrawCopy("same");

  return c;
}

TCanvas* pCluSizeVsRun(std::string option="")
{
  string cName = "cCluSizeVsRun"+option;
  TCanvas * c = new TCanvas(cName.c_str(),"cCluSizeVsRun",1000,500);
  TGraph* grClu = (TGraph*)gROOT->FindObject("hGraph_RunClu");
  if(!grClu) return 0;
  TH1D histoClu = runHistoFromGraph(grClu);
  histoClu.SetYTitle("hit strips in DU");
  histoClu.SetMaximum(3.1);
  histoClu.SetMinimum(1.2);
  fillHistoFromGraph(histoClu, grClu);
  histoClu.DrawCopy("E0");

  c->SetTicky(0); // remove axis on the right 
  c->SetGridy(0);
  c->SetRightMargin(gPad->GetLeftMargin());

  if (strcmp(option.c_str(),"") != 0) {
    std::string grName = "gr_"+option;
//    TGraph* graph = (TGraph*)gROOT->FindObject(grName.c_str());
    TGraph* graph = (TGraph*) g_L1RpcGlobalTObjects.FindObject(grName.c_str());
    TH1D hPress(histoClu);
    hPress.Reset("hTMPKOKO");
    fillHistoFromGraph(hPress, graph);
    std::pair<TH1D,TGaxis* > out =getDatOnRef( hPress,   histoClu);
    out.first.DrawCopy("same E0");
    TGaxis * axis = out.second;
    if (strcmp(option.c_str(),"Pressure")==0) axis->SetTitle("Pressure  [mbar]");
    if (strcmp(option.c_str(),"Humidity")==0) axis->SetTitle("Humidity");
    std::string axName="axClu"+option;
    axis->SetName(axName.c_str());
    axis->Draw();
  }

  printRunsOnPlotLinScale(&histoClu);

  return c;
}


void plotsClu()
{
  utilsPlotsSaver( pCluSize_1D() );
  utilsPlotsSaver( pCluDigiRHit() );

  int digits=TGaxis::GetMaxDigits();  // store current style
  TGaxis::SetMaxDigits(6);            // modify style
  utilsPlotsSaver( pCluSizeVsRun( ) );
  utilsPlotsSaver( pCluSizeVsRun("Pressure") );
  utilsPlotsSaver( pCluSizeVsRun("Humidity") );
  TGaxis::SetMaxDigits(digits);       // restore previous style

}

#endif
