#include "TROOT.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TText.h"
#include "TString.h"
#include "TGaxis.h"
#include <iomanip>
#include <sstream>
#include <cmath>
#include <map>
#include "utilsPlotsSaver.C"
#include "utilsDivideErr.C"
#include "utilsHistoFromGraph.C"

TH1D * getEffVsRunHisto(TGraphErrors *hGraph){
  if(!hGraph) return 0;
  int nPoints = hGraph->GetN();
  ////Count good Runs(=with small error on eff)
  Double_t xTmp, yTmp;
  Double_t error = 0;
  int nGoodRuns = 0;
  for(int i=0;i<nPoints;++i){
    hGraph->GetPoint(i,xTmp,yTmp);
    error = hGraph->GetErrorY(i);
    if(yTmp>1E-5 /* && error/yTmp<0.05 */) nGoodRuns++;
  }
  ///

  TH1D *histo = new TH1D("histo","",nGoodRuns,-0.5,nGoodRuns-0.5);
  histo->SetMaximum(1.);
  histo->SetMinimum(0.65); 
  //histo->SetMinimum(0.40); 
  histo->SetYTitle("L1 RPC efficiency");
  histo->SetLineWidth(2);

  nGoodRuns = 0;
  for(int i=0;i<nPoints;++i){
    hGraph->GetPoint(i,xTmp,yTmp);
    error = hGraph->GetErrorY(i);
    if(yTmp<1e-5 /* || error/yTmp>0.05 */) continue;
    histo->SetBinContent(nGoodRuns+1,yTmp);
    histo->SetBinError(nGoodRuns+1,error);
    histo->GetXaxis()->SetBinLabel(nGoodRuns+1,Form("%d",int(xTmp)));
    nGoodRuns++;
  }

  return histo;
}
/////////////////////////////////////////////////////////

void pRpcMiscWrongPT_helper(TH2D* h)
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  ff->cd();

  if(!h) return;
  std::stringstream entr;
  entr<<"Entries: "<<h->GetEntries();
  TText t;
  t.SetTextColor(4);
  t.SetTextAlign(31);
  t.SetTextSize(0.045);
  t.DrawText(2.1,6.3, entr.str().c_str());
}
/////////////////////////////////////////////////////////

TCanvas* pRpcMiscRunHisto(TString region="")
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  ff->cd();

  if(region!="Barrel" && region !="Endcap" && region!="" /* Total */) return 0;

  TCanvas*c ;
  if( region!="" ) {
    c = new TCanvas(Form("cRpcMisc_RunHisto%s",region.Data()),
		    Form("%s efficiency per run distributions",region.Data()),
		    1200,600);
  } else {
    c = new TCanvas("cRpcMisc_RunHisto","Efficiency per run distributions",1200,600);
  }
  c->Divide(2,1);
  TVirtualPad* p=c->cd(1); p->SetGrid(1,0);
  TH1D* h;
  if( region!="" ) { // barrel or endcap
    h = (TH1D*) gROOT->FindObject(Form("hRpcMisc_EffRun_%.3s",region.Data()));
  } else { // total
    h = (TH1D*) gROOT->FindObject("hRpcMisc_EffRun");
  }
  if(!h) return 0;
  h->SetTitle("");
  if( region!="" ) { // barrel or endcap
    h->SetXTitle(Form("%s efficiency",region.Data()));
  } else {
    h->SetXTitle("Efficiency");
  }
  h->GetXaxis()->SetNdivisions(20405,false);
  //h->GetYaxis()->SetTitleOffset(h->GetYaxis()->GetTitleOffset()*1.5);
  //p->SetLeftMargin(p->GetLeftMargin()*1.5);
  h->DrawCopy();

  std::stringstream msg;
  TText t; t.SetTextColor(4); t.SetTextAlign(31); t.SetTextSize(0.045);
  msg.str(""); msg<<"#Runs: "<<h->GetEntries();
  t.DrawTextNDC(0.6,0.57, msg.str().c_str());
  double mean = h->GetMean();
  msg.str(""); msg<<"<eff> = "<< std::floor(mean*1000+0.5)/1000.;
  t.DrawTextNDC(0.6,0.5, msg.str().c_str());
  msg.str(""); msg<<"RMS: "<<std::floor(h->GetRMS()*1000)/1000.;
  t.DrawTextNDC(0.6,0.43, msg.str().c_str());
  

  p=c->cd(2); p->SetGrid(0,1);
  TGraph* gr;
  if( region!="" ) { // barrel or endcap
    gr = (TGraph*)gROOT->FindObject(Form("hGraph_RunEff_%.3s",region.Data()));
  } else { // total
    gr = (TGraph*)gROOT->FindObject("hGraph_RunEff");
  }
  if(!gr) return 0;
  gr->SetMarkerStyle(25);
  gr->SetMarkerColor(2);
  gr->SetMaximum(1.0);
  gr->SetMinimum(0.8);
  gr->Draw("Paint E0"); // draw errors, disable clipping
  c->Update();
  gr->GetHistogram()->GetXaxis()->SetNdivisions(-10);
  gr->GetHistogram()->GetXaxis()->SetLabelSize(0.025);
  gr->GetHistogram()->GetXaxis()->SetTitle("Run");
  gr->GetHistogram()->GetYaxis()->SetTitleOffset(1.6);
  if( region!="" ){ // barrel or endcap
    gr->GetHistogram()->GetYaxis()->SetTitle(Form("%s efficiency",region.Data()));
  } else { // total
    gr->GetHistogram()->GetYaxis()->SetTitle("Efficiency");
  }
  c->Modified();

  return c;
}
/////////////////////////////////////////////////////////

TCanvas* pRpcMiscEffandPressVsRun(TString region="")
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  ff->cd();

  if(region!="Barrel" && region !="Endcap" && region!="" /* Total */) return 0;
  TCanvas* c;
  TGraphErrors* gr;
  if( region!="" ) { // barrel or endcap
    c = new TCanvas(Form("cRpcMisc_EffandPressVsRun%s",region.Data()),
		    Form("%s efficiency and UXC pressure vs run",region.Data()),
		    1200,600);
    // TODO !!!!!
    // Need to be modified once separate barrel and endcap eff are available
    //gr = (TGraphErrors*)gROOT->FindObject(Form("hGraph_RunEff_%.3s",region.Data()));
    gr = (TGraphErrors*)gROOT->FindObject("hGraph_RunEff"); // temporary
  } else {  // total
    c = new TCanvas("cRpcMisc_EffandPressVsRun","Efficiency and UXC pressure vs run",1200,600);
    gr = (TGraphErrors*)gROOT->FindObject("hGraph_RunEff");
  }
  TH1D* hAll = getEffVsRunHisto(gr);
  if(!hAll) return 0;
  hAll->GetXaxis()->LabelsOption("v");
  hAll->GetYaxis()->SetTitleOffset(1.3);
  if( region!="" ) {
    hAll->SetYTitle(Form("%s %s",region.Data(),hAll->GetYaxis()->GetTitle()));
  }
  hAll->Draw("E0"); // draw error bars, turn off symbol clipping
  TH1D * hpress = getPressVsRunHisto(hAll);
  if(hpress) {
    hpress->Draw("same P E0");  // draw error bars, turn off symbol clipping 
    // draw additional axis
    gPad->SetTicky(0); // remove axis on the right 
    gPad->SetGridy(1);
    gPad->SetRightMargin(gPad->GetLeftMargin());
    gPad->Update();
    TGaxis *a = new TGaxis(hAll->GetXaxis()->GetXmax(),
			   hAll->GetMinimum(),
			   hAll->GetXaxis()->GetXmax(),
			   hAll->GetMaximum()-1E-5,
			   hpress->GetBinContent(0),
			   hpress->GetBinContent(hpress->GetNbinsX()+1),
			   510,"+SLI");
    a->SetName("a");
    a->SetLineColor(hpress->GetLineColor());
    a->SetLabelColor(hpress->GetLineColor());
    a->SetTitleColor(hpress->GetLineColor());
    a->SetLabelSize(hAll->GetYaxis()->GetLabelSize());
    a->SetLabelOffset(hAll->GetYaxis()->GetLabelOffset());
    a->SetLabelFont(hAll->GetYaxis()->GetLabelFont());
    a->SetTitleSize(hAll->GetYaxis()->GetTitleSize());
    a->SetTitleOffset(hAll->GetYaxis()->GetTitleOffset());
    a->SetTitleFont(hAll->GetYaxis()->GetTitleFont());
    a->SetTitle("Pressure  [mbar]");
    a->Draw();
  }
  //user comments
  printRunsOnPlot(hAll);

  return c;

}
/////////////////////////////////////////////////////////

TCanvas* pRpcMiscEffandHumidityVsRun(TString region="")
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  ff->cd();

  if(region!="Barrel" && region !="Endcap" && region!="" /* Total */) return 0;
  TCanvas* c;
  TGraphErrors* gr;
  if( region!="" ) { // barrel or endcap
    c = new TCanvas(Form("cRpcMisc_EffandHumidityVsRun%s",region.Data()),
		    Form("%s efficiency and UXC humidity vs run",region.Data()),
		    1200,600);
    // TODO !!!!!
    // Need to be modified once separate barrel and endcap eff are available
    //gr = (TGraphErrors*)gROOT->FindObject(Form("hGraph_RunEff_%.3s",region.Data()));
    gr = (TGraphErrors*)gROOT->FindObject("hGraph_RunEff"); // temporary
  } else {  // total
    c = new TCanvas("cRpcMisc_EffandHumidityVsRun","Efficiency and UXC humidity vs run",1200,600);
    gr = (TGraphErrors*)gROOT->FindObject("hGraph_RunEff");
  }
  TH1D* hAll = getEffVsRunHisto(gr);
  if(!hAll) return 0;
  hAll->GetXaxis()->LabelsOption("v");
  hAll->GetYaxis()->SetTitleOffset(1.3);
  if( region!="" ) {
    hAll->SetYTitle(Form("%s %s",region.Data(),hAll->GetYaxis()->GetTitle()));
  }
  hAll->Draw("E0"); // draw error bars, turn off symbol clipping
  TH1D * hRH = getHumidityVsRunHisto(hAll);
  if(hRH) {
    hRH->Draw("same P E0");  // draw error bars, turn off symbol clipping
    // draw additional axis
    gPad->SetTicky(0); // remove axis on the right 
    gPad->SetGridy(1);
    gPad->SetRightMargin(gPad->GetLeftMargin());
    gPad->Update();
    TGaxis *a = new TGaxis(hAll->GetXaxis()->GetXmax(),
			   hAll->GetMinimum(),
			   hAll->GetXaxis()->GetXmax(),
			   hAll->GetMaximum()-1E-5,
			   hRH->GetBinContent(0),
			   hRH->GetBinContent(hRH->GetNbinsX()+1),
			   510,"+SLI");
    a->SetName("a");
    a->SetLineColor(hRH->GetLineColor());
    a->SetLabelColor(hRH->GetLineColor());
    a->SetTitleColor(hRH->GetLineColor());
    a->SetLabelSize(hAll->GetYaxis()->GetLabelSize());
    a->SetLabelOffset(hAll->GetYaxis()->GetLabelOffset());
    a->SetLabelFont(hAll->GetYaxis()->GetLabelFont());
    a->SetTitleSize(hAll->GetYaxis()->GetTitleSize());
    a->SetTitleOffset(hAll->GetYaxis()->GetTitleOffset());
    a->SetTitleFont(hAll->GetYaxis()->GetTitleFont());
    a->SetTitle("Relative humidity  [%]");
    a->Draw();
  }
  //user comments
  printRunsOnPlot(hAll);

  return c;

}
/////////////////////////////////////////////////////////

TCanvas* pRpcMiscEffandClSizeVsRun(TString region="")
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  ff->cd();

  if(region!="Barrel" && region !="Endcap") return 0;
  TCanvas* c = new TCanvas(Form("cRpcMisc_EffandClSizeVsRun%s",region.Data()),
			   Form("%s efficiency and cluster size vs run",region.Data()),
			   1200,600);
  // TODO !!!!!
  // Need to be modified once separate barrel and endcap eff are available
  // TGraphErrors* gr = (TGraphErrors*)gROOT->FindObject(Form("hGraph_RunEff_%.3s",region.Data()));
  TGraphErrors* gr = (TGraphErrors*)gROOT->FindObject("hGraph_RunEff");
  TH1D* hAll = getEffVsRunHisto(gr);
  if(!hAll) return 0;
  hAll->GetXaxis()->LabelsOption("v");
  hAll->GetYaxis()->SetTitleOffset(1.3);
  if( region!="" ) {
    hAll->SetYTitle(Form("%s %s",region.Data(),hAll->GetYaxis()->GetTitle()));
  }
  hAll->Draw("E0");  // draw error bars, turn off symbol clipping
  TH1D * hcluster = 
    getClusterSizeVsRunHisto(hAll,
			     Form("ClusterSizeGraph_%s.root",region.Data()));
  if(hcluster) {
    hcluster->Draw("same P E0");  // draw error bars, turn off symbol clipping
    // draw additional axis
    gPad->SetTicky(0); // remove axis on the right 
    gPad->SetGridy(1);
    gPad->SetRightMargin(gPad->GetLeftMargin());
    gPad->Update();
    
    TGaxis *a = new TGaxis(hAll->GetXaxis()->GetXmax(),
			   hAll->GetMinimum(),
			   hAll->GetXaxis()->GetXmax(),
			   hAll->GetMaximum()-1E-5,
			   hcluster->GetBinContent(0),
			   hcluster->GetBinContent(hcluster->GetNbinsX()+1),
			   510,"+SL");
    a->SetName("a");
    a->SetLineColor(hcluster->GetLineColor());
    a->SetLabelColor(hcluster->GetLineColor());
    a->SetTitleColor(hcluster->GetLineColor());
    a->SetLabelSize(hAll->GetYaxis()->GetLabelSize());
    a->SetLabelOffset(hAll->GetYaxis()->GetLabelOffset());
    a->SetLabelFont(hAll->GetYaxis()->GetLabelFont());
    a->SetTitleSize(hAll->GetYaxis()->GetTitleSize());
    a->SetTitleOffset(hAll->GetYaxis()->GetTitleOffset());
    a->SetTitleFont(hAll->GetYaxis()->GetTitleFont());
    a->SetTitle(Form("%s cluster size [strips]",region.Data()));
    a->Draw();
  }  
  //user comments
  printRunsOnPlot(hAll);
  
  return c;

}
/////////////////////////////////////////////////////////

TCanvas* pRpcMiscTime()
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  ff->cd();

  TCanvas* c = new TCanvas("cRpcMisc_Time","L1 RPC trigger timing",600,600);
  c->SetGrid(0,1);
  c->SetLogy(1);
  TH1D* h = (TH1D*) gROOT->FindObject("hRpcMisc_Time");
  if(!h) return 0;
  h->Scale(1./h->GetMaximum());
  h->SetLineColor(4);
  h->SetLineWidth(2);
  //gStyle->SetOptTitle(0);
  h->GetXaxis()->SetNdivisions(106);
  h->SetXTitle("L1Rpc_BX vs CMS_BX"); 
  h->SetYTitle("Fraction of RPC triggers");
  h->Draw("hist text0");
  return c;
}
/////////////////////////////////////////////////////////

TCanvas* pRpcMiscWrongPT()
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  ff->cd();

  TCanvas* c = new TCanvas("cRpcMisc_WrongPT","cRpcMisc_WrongPT",1200,600);
  c->Divide(2,1);

  c->cd(1); 
  TH2D* h;
  h=(TH2D*) gROOT->FindObject("hRpcMisc_US");
  if(!h) return 0;
  // h->GetXaxis()->SetNdivisions(117);
  h->SetXTitle("eta"); h->SetYTitle("phi");
  h->DrawCopy("box");
  pRpcMiscWrongPT_helper(h);
 
  c->cd(2); 
  h=(TH2D*) gROOT->FindObject("hRpcMisc_OS");
  if(!h) return 0;
  h->SetXTitle("eta"); h->SetYTitle("phi");
  h->DrawCopy("box");
  pRpcMiscWrongPT_helper(h);

  return c;
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

void plotsRpcMisc()
{
  utilsPlotsSaver(pRpcMiscWrongPT());

  
  gStyle->SetPaintTextFormat(".2E");  // modify text value style
  utilsPlotsSaver(pRpcMiscTime());    // trigger timing
  gStyle->SetPaintTextFormat();       // restore default style

  
  int digits=TGaxis::GetMaxDigits();  // store current style
  TGaxis::SetMaxDigits(4);            // modify style
  utilsPlotsSaver(pRpcMiscRunHisto());// total eff
  TGaxis::SetMaxDigits(digits);       // restore previous style

  utilsPlotsSaver(pRpcMiscEffandPressVsRun());        // total eff
  //utilsPlotsSaver(pRpcMiscEffandPressVsRun("Barrel"));// barrel eff + press
  //utilsPlotsSaver(pRpcMiscEffandPressVsRun("Endcap"));// endcap eff + press
  //utilsPlotsSaver(pRpcMiscEffandClSizeVsRun("Barrel")); 
  //utilsPlotsSaver(pRpcMiscEffandClSizeVsRun("Endcap")); 
  utilsPlotsSaver(pRpcMiscEffandHumidityVsRun());     // total eff + humidity 
  //utilsPlotsSaver(pRpcMiscEffandHumidityVsRun("Barrel"));// barrel eff + humidity 
  //utilsPlotsSaver(pRpcMiscEffandHumidityVsRun("Endcap"));// endcap eff + humidity 
}
