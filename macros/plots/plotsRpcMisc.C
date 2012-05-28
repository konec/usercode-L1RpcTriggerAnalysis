#include "TROOT.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TText.h"
#include <iomanip>
#include <sstream>
#include <cmath>
#include "utilsPlotsSaver.C"
#include "utilsDivideErr.C"
//#include <map>

TH1D * getEffVsRunHisto(TGraphErrors *hGraph){

  int nPoints = hGraph->GetN();
  ////Count good Runs(=with small error on eff)
  Double_t xTmp, yTmp;
  Double_t error = 0;
  int nGoodRuns = 0;
  for(int i=0;i<nPoints;++i){
    hGraph->GetPoint(i,xTmp,yTmp);
    error = hGraph->GetErrorY(i);
    if(yTmp>1E-5 && error/yTmp<0.05) nGoodRuns++;
  }
  ///

  TH1D *histo = new TH1D("histo","",nGoodRuns,-0.5,nGoodRuns-0.5);
  histo->SetMaximum(1.);
  histo->SetMinimum(0.65); 
  histo->SetYTitle("L1 RPC Efficiency");
  histo->SetLineWidth(2);

  nGoodRuns = 0;
  for(int i=0;i<nPoints;++i){
    hGraph->GetPoint(i,xTmp,yTmp);
    error = hGraph->GetErrorY(i);
    if(yTmp<1e-5 || error/yTmp>0.05) continue;
    histo->SetBinContent(nGoodRuns+1,yTmp);
    histo->SetBinError(nGoodRuns+1,error);
    histo->GetXaxis()->SetBinLabel(nGoodRuns+1,Form("%d",int(xTmp)));
    nGoodRuns++;
  }

  return histo;
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
/*
TH1D *getPressVsRunHisto(TH1D *histo){

 ////Load pressure graph, and transfer into map
  TFile *file = new TFile("PressureGraph.root");
  TGraph *graph = (TGraph*)file->Get("Graph");
  Double_t xTmp, yTmp;
  std::map<int,float> aMap;
  float max = -9999;
  float min = 9999;
  for(int i=0;i<graph->GetN();++i){
    graph->GetPoint(i,xTmp,yTmp);
    aMap[xTmp] = yTmp;
    if(yTmp>max) max = yTmp;
    if(yTmp<min && yTmp>100) min = yTmp;
  }
  /////////////////////// 
  float axisMin = histo->GetMinimum();
  float axisMax = histo->GetMaximum();
  /////
  TH1D *h = (TH1D*)histo->Clone("h");
  h->Reset();
  for(int i=1;i<=h->GetNbinsX();++i){
    int run = atoi(h->GetXaxis()->GetBinLabel(i));
    float val = 0.0;    
    if(aMap.find(run)!=aMap.end()) val = aMap[run];
    float valNormalised =  (axisMax - axisMin)/(max - min)*(val - min) + axisMin;
    if(fabs(val)<1e-5) valNormalised = 0.0;
    h->SetBinContent(i,valNormalised);           
  }
  h->SetBinContent(0,min);       
  h->SetBinContent(h->GetNbinsX()+1,max);  
  h->SetLineColor(2);
  h->SetMarkerColor(2);
  h->SetMarkerStyle(21);
  
  return h;
}
*/

/*
TH1F *getPressVsRunHisto(TH1F *histo)
{
  std::cout << "HERE" << std::endl;
 ////Load pressure graph, and transfer into map
  TFile *file = new TFile("PressureGraph.root");
  TGraph *graph = (TGraph*)file->Get("Graph");
  Double_t xTmp, yTmp;
  std::map<int,float> aMap;
  float max = -9999;
  float min = 9999;
  for(int i=0;i<graph->GetN();++i){
    graph->GetPoint(i,xTmp,yTmp);
    std::cout <<"xTmp : "<<xTmp<<" yTmp: "<<yTmp<<std::endl;
    aMap[xTmp] = yTmp;
    if(yTmp>max) max = yTmp;
    if(yTmp<min && yTmp>100) min = yTmp;
  }
  /////////////////////// 
  float axisMin = histo->GetMinimum();
  float axisMax = histo->GetMaximum();
  /////
  std::cout <<"K1" << std::endl;
  TH1F *h= (TH1F*)histo->Clone("hPressHisto");
  std::cout <<"K2" << std::endl;
  h->Reset();
  for(int i=1;i<=h->GetNbinsX();++i){
    int run = atoi(h->GetXaxis()->GetBinLabel(i));
    float val = 0.0;
    if(aMap.find(run)!=aMap.end()) val = aMap[run];
    float valNormalised =  (axisMax - axisMin)/(max - min)*(val - min) + axisMin;
    if(fabs(val)<1e-5) valNormalised = 0.0;
    h->SetBinContent(i,valNormalised);
  }
  std::cout <<"K3" << std::endl;
  h->SetBinContent(0,min);
  h->SetBinContent(h->GetNbinsX()+1,max);
  h->SetLineColor(2);
  h->SetMarkerColor(2);
  h->SetMarkerStyle(21);
  
  std::cout <<"K4" << std::endl;
  return h;
}
*/


void pRpcMiscWrongPT_helper(TH2D* h)
{
  std::stringstream entr;
  entr<<"Entries: "<<h->GetEntries();
  TText t;
  t.SetTextColor(4);
  t.SetTextAlign(31);
  t.SetTextSize(0.045);
  t.DrawText(2.1,6.3, entr.str().c_str());
}

TCanvas* pRpcMiscRunHisto()
{
  TCanvas* c = new TCanvas("cRpcMiscRunHisto","cRpcMiscRunHisto",1200,600);
  c->Divide(2,1);
  TVirtualPad* p=c->cd(1); p->SetGrid(1,0);
  TH1D* h = (TH1D*) gROOT->FindObject("hRpcMisc_EffRun");
  h->SetTitle();
  h->DrawCopy();

  std::stringstream msg;
  TText t; t.SetTextColor(4); t.SetTextAlign(31); t.SetTextSize(0.045);
  msg.str(""); msg<<"#Runs: "<<h->GetEntries();
  t.DrawTextNDC(0.5,0.57, msg.str().c_str());
  double mean = h->GetMean();
  msg.str(""); msg<<"<eff> = "<< std::floor(mean*1000+0.5)/1000.;
  t.DrawTextNDC(0.5,0.5, msg.str().c_str());
  msg.str(""); msg<<"RMS: "<<std::floor(h->GetRMS()*1000)/1000.;
  t.DrawTextNDC(0.5,0.43, msg.str().c_str());
  

  p=c->cd(2); p->SetGrid(0,1);
  TGraph* gr = (TGraph*)gROOT->FindObject("hGraph_RunEff");
  gr->SetMarkerStyle(25);
  gr->SetMarkerColor(2);
  gr->SetMaximum(1.0);
  gr->SetMinimum(0.8);
  gr->Draw("Paint E");
  c->Update();
  gr->GetHistogram()->GetXaxis()->SetNdivisions(-10);
  gr->GetHistogram()->GetXaxis()->SetLabelSize(0.025);
  gr->GetHistogram()->GetXaxis()->SetTitle("Det Unit Id (raw)");
  gr->GetHistogram()->GetYaxis()->SetTitleOffset(1.6);
  gr->GetHistogram()->GetYaxis()->SetTitle("efficiency");
  c->Modified();

//  TH1F* hhh = gr->GetHistogram();
//  getPressVsRunHisto( hhh);
  //getPressVsRunHisto(gr->GetHistogram());
  return c;
}

TCanvas* pTMP()
{
   TCanvas* c = new TCanvas("cTMP","cTMP",1200,600);
TGraphErrors* gr = (TGraphErrors*)gROOT->FindObject("hGraph_RunEff");
  TH1D* hAll = getEffVsRunHisto(gr);
  hAll->GetXaxis()->LabelsOption("v");
  hAll->GetYaxis()->SetTitleOffset(1.3);
  hAll->Draw();
  getPressVsRunHisto(hAll)->Draw("same P");
  return c;

}

TCanvas* pRpcMiscTime()
{
  TCanvas* c = new TCanvas("cRpcMiscTime","cRpcMiscTime",600,600);
  c->SetGrid(0,1);
  c->SetLogy(1);
  TH1D* h = (TH1D*) gROOT->FindObject("hRpcMisc_Time");
  h->Scale(1./h->GetMaximum());
  h->SetLineColor(4);
  h->SetLineWidth(2);
  h->GetXaxis()->SetNdivisions(106);
  h->SetXTitle("L1Rpc_BX vs CMS_BX"); 
  h->SetYTitle("fraction of RPC triggers"); 
  h->Draw("hist text");
  return c;
}

TCanvas* pRpcMiscWrongPT()
{
  TCanvas* c = new TCanvas("cRpcMiscWrongPT","cRpcMiscWrongPT",1200,600);
  c->Divide(2,1);

  c->cd(1); 
  TH2D* h;
  h=(TH2D*) gROOT->FindObject("hRpcMisc_US");
 // h->GetXaxis()->SetNdivisions(117);
  h->SetXTitle("eta"); h->SetYTitle("phi");
  h->DrawCopy("box");
  pRpcMiscWrongPT_helper(h);
 

  c->cd(2); 
  h=(TH2D*) gROOT->FindObject("hRpcMisc_OS");
  h->SetXTitle("eta"); h->SetYTitle("phi");
  h->DrawCopy("box");
  pRpcMiscWrongPT_helper(h);

  return c;
}


void plotsRpcMisc()
{
  utilsPlotsSaver(pRpcMiscWrongPT());
  utilsPlotsSaver(pRpcMiscTime());
  utilsPlotsSaver(pRpcMiscRunHisto());
//  utilsPlotsSaver(pTMP());
}
