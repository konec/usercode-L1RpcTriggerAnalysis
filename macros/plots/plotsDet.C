#ifndef plotsDet_C
#define plotsDet_C

#include "TROOT.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TText.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"
#include <iomanip>
#include <sstream>

#include "utilsPlotsSaver.C"
#include "utilsDivideErr.C"

void pDet_MeanPrinter(TH1* h)
{
  if(!h) return;
  std::stringstream mean;
  mean<<"Mean: "<<h->GetMean();
  TText t;
  t.SetTextColor(4);
  t.SetTextAlign(31);
  t.SetTextSize(0.045);
  t.DrawTextNDC(0.95,0.92, mean.str().c_str());
}

TCanvas* pDet_EfficRoll()
{
  TCanvas* c = new TCanvas("cDet_EfficRoll","cDet_EfficRoll",1200,600);
  TVirtualPad* p; TH1D* h;
  c->Divide(2,1);

  p= c->cd(1); p->SetLogy(1);
  h  = (TH1D*)  gROOT->FindObject("hDet_EfficRoll");
  if(!h) return 0;
  h->SetXTitle("Efficiency"); h->SetYTitle("# det. units");
  h->SetMinimum(0.7);
  h->SetStats(kFALSE);
  h->DrawCopy();
  pDet_MeanPrinter(h);

  p= c->cd(2); p->SetLogy(1);
  h  = (TH1D*)  gROOT->FindObject("hDet_EfficRollWeighted");
  if(!h) return 0;
  h->SetXTitle("Efficiency"); h->SetYTitle("# det. units");
  h->SetMinimum(0.7);
  h->SetStats(kFALSE);
  h->DrawCopy();
  pDet_MeanPrinter(h);
  return c;
  
}
TCanvas* pDet_EfficPart()
{
  TCanvas* c = new TCanvas("cDet_EfficPart","cDet_EfficPart",600,600);
  c->SetGrid(0,1);
  TH1D* h[6];
  std::stringstream str;
  TH1D* hBase =  (TH1D*)gROOT->FindObject("hDet_PartHit1");
  if(!hBase) return 0;
  hBase->SetNameTitle("hDet_Part","hDet_Part");
  hBase->Reset(); hBase->SetMaximum(1.02); hBase->SetMinimum(0.84);
  hBase->DrawCopy();

  TLegend* leg = new TLegend(0.75, 0.7, 0.93,.9);

  for (unsigned int i=1; i<=6; i++) {
    str.str(""); str << "hDet_PartHit"<<i; 
    TH1D* h1= (TH1D*)gROOT->FindObject( str.str().c_str());
    str.str(""); str << "hDet_PartDet"<<i; 
    TH1D* h2= (TH1D*)gROOT->FindObject( str.str().c_str());
    h[i-1]=DivideErr(h1,h2,(str.str()+"_Eff").c_str(),"B");
    if(!h[i-1]) return 0;
    h[i-1]->SetMarkerColor(i);
    h[i-1]->SetMarkerStyle(21+i);
    h[i-1]->DrawCopy("same");
    str.str(""); str << "Disk/Layer "<<i;
    leg->AddEntry(h[i-1], str.str().c_str());
  }
  leg->Draw();
  return c;
}

TCanvas* pDet_EfficHistory()
{
  TCanvas* c = new TCanvas("cDet_EfficHistory","cDet_EfficHistory",600,600);
  TGraph* gr = (TGraph*)gROOT->FindObject("hGraph_DetEff");
  if(!gr) return 0;
  gr->SetMarkerStyle(25);
  gr->SetMarkerColor(2);
  gr->SetMaximum(1.1);
  gr->SetMinimum(0.0);
  gr->Draw("Paint E");
  c->Update();
  //gr->GetHistogram()->GetXaxis()->SetLabelOffset(1.006);
  gr->GetHistogram()->GetXaxis()->SetNdivisions(-10);
  gr->GetHistogram()->GetXaxis()->SetLabelSize(0.02);
  //  gr->GetHistogram()->GetXaxis()->LabelsOption("V");
  gr->GetHistogram()->GetXaxis()->SetTitle("Det Unit Id (raw)");
  gr->GetHistogram()->GetYaxis()->SetTitle("Efficiency");
  c->Modified();
  return c; 
}

TCanvas* pDet_Coinc()
{
  TCanvas* c = new TCanvas("cDet_Coinc","cDet_Coinc",600,600);
  TH1D *hn, *hd;
  hd = (TH1D*)gROOT->FindObject("hDet_Muon");
  TLegend* l = new TLegend(0.5, 0.2, 0.94,.4);
  
  hn = (TH1D*)gROOT->FindObject("hDet_CoincDet");
  TH1D* h =DivideErr(hn, hd,"hDet_ConicDetEff", "B");
  if(!h) return 0;
  h->SetLineColor(2);
  h->DrawCopy("h");
  l->AddEntry(h,"detector coincidence");

  hn =(TH1D*)gROOT->FindObject("hDet_CoincHit");
  h =DivideErr(hn, hd,"hDet_ConicHitEff", "B"); 
  if(!h) return 0;
  h->SetLineColor(3);
  h->DrawCopy("same histo");
  l->AddEntry(h,"hit coincidence");
  //  h->Print("all");

  hn =(TH1D*)gROOT->FindObject("hEmu_Wide");
  hd =(TH1D*)gROOT->FindObject("hEmu_Muon");
  h =DivideErr(hn, hd,"hDet_EmuEff", "B");
  if(!h) return 0;
  h->SetLineColor(4);
  h->DrawCopy("same histo");
  l->AddEntry(h,"wide pattern");

  hn =(TH1D*)gROOT->FindObject("hEmu_L1Rpc");
  h =DivideErr(hn, hd,"hDet_L1RpcEff", "B");     
  if(!h) return 0;
  h->SetLineColor(6);
  h->DrawCopy("same histo");
  l->AddEntry(h,"L1Rpc");

  l->Draw();

/*
  TGraphAsymmErrors* gr1 = new TGraphAsymmErrors();
  gr1->Divide(hDet_CoincDet, hDet_Muon); 
  gr1->Draw("same P");
  gr1->Print("all");
*/
  return c;
}

void plotsDet()
{
  utilsPlotsSaver( pDet_EfficRoll() );
  utilsPlotsSaver( pDet_EfficPart() );
  utilsPlotsSaver( pDet_EfficHistory() );  
  utilsPlotsSaver( pDet_Coinc() );
}

#endif
