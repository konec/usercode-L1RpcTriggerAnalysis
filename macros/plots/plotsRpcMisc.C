#include "TROOT.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TText.h"
#include "TString.h"
#include "TGaxis.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <map>
#include "utilsPlotsSaver.h"
#include "utilsHistoFromGraph.h"
#include "utilsObjArray.h"

//#include "utilsDivideErr.C"


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
    if(yTmp>1E-5
//  && error/yTmp<0.05 
     ) nGoodRuns++;
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
    if(yTmp<1e-5 
// || error/yTmp>0.05 
     ) continue;
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


TCanvas* pRpcMiscEffVsRun(std::string option="")
{
  string cName = "cRpcMiscEffVsRun"+option;
  TCanvas * c = new TCanvas(cName.c_str(),"cRpcMiscEffVsRun",1000,500);
  TGraph* grEff = (TGraph*)gROOT->FindObject("hGraph_RunEff");
  if(!grEff) return 0;
  TH1D histoEff = runHistoFromGraph(grEff);
  histoEff.SetYTitle("L1Rpc efficiency");
  histoEff.SetMaximum(0.95);
  histoEff.SetMinimum(0.75);
  fillHistoFromGraph(histoEff, grEff);
  histoEff.DrawCopy("E0");

  c->SetTicky(0); // remove axis on the right 
  c->SetGridy(0);
  c->SetRightMargin(gPad->GetLeftMargin());

  if (strcmp(option.c_str(),"") != 0) {
    std::string grName = "gr_"+option;
//    TGraph* graph = (TGraph*)gROOT->FindObject(grName.c_str());
    TGraph* graph = (TGraph*) g_L1RpcGlobalTObjects.FindObject(grName.c_str());
    TH1D hPress(histoEff);
    hPress.Reset("hTMPKOKO");
    fillHistoFromGraph(hPress, graph);
    std::pair<TH1D,TGaxis* > out =getDatOnRef( hPress,   histoEff);
    out.first.DrawCopy("same E0");
    TGaxis * axis = out.second;
    if (strcmp(option.c_str(),"Pressure")==0) axis->SetTitle("Pressure  [mbar]");
    if (strcmp(option.c_str(),"Humidity")==0) axis->SetTitle("Humidity");
    std::string axName="axEff"+option;
    axis->SetName(axName.c_str());
    axis->Draw();
  }

  printRunsOnPlotLinScale(&histoEff);

  return c;
  
}
/////////////////////////////////////////////////////////
TCanvas* pRpcMiscPurVsRun(std::string option="")
{
  string cName = "cRpcMiscPurVsRun"+option;
  TCanvas * c = new TCanvas(cName.c_str(),"cRpcMiscPurVsRun",500,500);
  TGraph* grPur = (TGraph*)gROOT->FindObject("hGraph_RunPur");
  if(!grPur) return 0;
  TH1D histoPur = runHistoFromGraph(grPur);
  histoPur.SetYTitle("good/bad events");
  histoPur.SetMarkerStyle(25);
  histoPur.SetMarkerColor(2);
  histoPur.SetMaximum(400);
//  histoPur.SetMaximum(0.8);
  histoPur.SetMinimum(0.1);
  histoPur.GetXaxis()->SetNdivisions(-205);
  histoPur.GetXaxis()->SetLabelSize(0.045);
  histoPur.GetYaxis()->SetTitleOffset(1.2);
//  histoPur.GetYaxis()->SetAxisColor(2);
//  histoPur.GetYaxis()->SetLabelColor(2);
//  histoPur.GetYaxis()->SetTitleColor(2);
  fillHistoFromGraph(histoPur, grPur);
  histoPur.DrawCopy("E0");

  c->SetTicky(0); // remove axis on the right 
  c->SetGridy(0);
  c->SetRightMargin(gPad->GetLeftMargin());

  if (strcmp(option.c_str(),"") != 0) {
    std::string grName = "gr_"+option;
    TGraph* gr_Pressure = (TGraph*)gROOT->FindObject(grName.c_str());
    TH1D hPress(histoPur);
    hPress.Reset("hTMPKOKO");
    fillHistoFromGraph(hPress, gr_Pressure);
    std::pair<TH1D,TGaxis* > out =getDatOnRef( hPress,   histoPur);
    out.first.DrawCopy("same E0");
    TGaxis * axis = out.second;
    if (strcmp(option.c_str(),"Pressure")==0) axis->SetTitle("Pressure  [mbar]");
    if (strcmp(option.c_str(),"Humidity")==0) axis->SetTitle("Humidity");
    std::string axName="axPur"+option;
    axis->SetName(axName.c_str());
    axis->Draw();
  }

  return c;
}

//////////////////////////////////////////////////////////
TCanvas* pRpcMiscEffVsClu()
{
  TCanvas * c = new TCanvas("cRpcMiscEffVsClu","cRpcMiscEffVsClu",500,500);
  TGraph* gr_Eff = (TGraph*)gROOT->FindObject("hGraph_RunEff");
  TGraph* gr_Clu = (TGraph*)gROOT->FindObject("hGraph_RunClu");
  std::cout <<"HERE "<<gr_Eff->GetN()<<" "<<gr_Clu->GetN()<<std::endl;
  if ( gr_Eff->GetN() !=  gr_Clu->GetN()) return c;
  
//  TGraphErrors * grEffClu = (TGraphErrors*)gROOT->FindObject("grEffClu");
//  if (grEffClu) grEffClu->Delete();
  TGraphErrors * grEffClu = new TGraphErrors;
  grEffClu->SetName("grEffClu");
  grEffClu->Set( gr_Eff->GetN());
  for (Int_t i=0; i < gr_Eff->GetN(); ++i) {
    double rTMP1, rTMP2, vEff, vClu;
    gr_Eff->GetPoint(i,rTMP1, vEff);
    gr_Clu->GetPoint(i,rTMP2, vClu);
    double vEffErr=  gr_Eff->GetErrorY(i);
    double vCluErr=  gr_Clu->GetErrorY(i);
    unsigned int iRun1 = static_cast<unsigned int>( rTMP1+1.e-6);
    unsigned int iRun2 = static_cast<unsigned int>( rTMP2+1.e-6);
    if (iRun1 != iRun2) { std::cout <<"RUN1  RUN2, problem, exit"<<std::endl; return c; }
    
    grEffClu->SetPoint(i, vClu, vEff);
    grEffClu->SetPointError(i, vCluErr, vEffErr);
  }
  TH1D hdummy("hDummy","hDummy", 20, 2.0, 2.4); 
  hdummy.SetMinimum(0.74); 
  hdummy.SetMaximum(0.95); 
  hdummy.GetXaxis()->SetNdivisions(-204);
  hdummy.SetXTitle("Cluster size");
  hdummy.SetYTitle("L1Rpc efficiency");
  hdummy.GetYaxis()->SetTitleOffset(1.7);
  hdummy.DrawCopy("axis");
  grEffClu->SetMarkerStyle(25);
  grEffClu->SetMarkerColor(2);
  grEffClu->Draw("same P E0"); // draw errors, disable clipping
  c->Update();
/*
  grEffClu->GetHistogram()->GetXaxis()->SetNdivisions(-5);
  grEffClu->GetHistogram()->GetXaxis()->SetLabelSize(0.035);
  if( region!="" ){ // barrel or endcap
    gr->GetHistogram()->GetYaxis()->SetTitle(Form("%s efficiency",region.Data()));
  } else { // total
    gr->GetHistogram()->GetYaxis()->SetTitle("Efficiency");
  }
*/
  c->Modified();


  return c;
}
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
TCanvas* pRpcMiscPurVsClu()
{
  TCanvas * c = new TCanvas("cRpcMiscPurVsClu","cRpcMiscPurVsClu",500,500);
  TGraph* gr_Pur = (TGraph*)gROOT->FindObject("hGraph_RunPur");
  TGraph* gr_Clu = (TGraph*)gROOT->FindObject("hGraph_RunClu");
  if ( gr_Pur->GetN() !=  gr_Clu->GetN()) return c;
  
  TGraphErrors * grPurClu = new TGraphErrors;
  grPurClu->SetName("grPurClu");
  grPurClu->Set( gr_Pur->GetN());
  for (Int_t i=0; i < gr_Pur->GetN(); ++i) {
    double rTMP1, rTMP2, vPur, vClu;
    gr_Pur->GetPoint(i,rTMP1, vPur);
    gr_Clu->GetPoint(i,rTMP2, vClu);
    double vPurErr=  gr_Pur->GetErrorY(i);
    double vCluErr=  gr_Clu->GetErrorY(i);
    unsigned int iRun1 = static_cast<unsigned int>( rTMP1+1.e-6);
    unsigned int iRun2 = static_cast<unsigned int>( rTMP2+1.e-6);
    if (iRun1 != iRun2) { std::cout <<"RUN1  RUN2, problem, exit"<<std::endl; return c; }
    
    grPurClu->SetPoint(i, vClu, vPur);
    grPurClu->SetPointError(i, vCluErr, vPurErr);
  }
  TH1D hdummy("hDummy","hDummy", 20, 2.0, 2.6); 
  hdummy.SetMinimum(0.5); 
  hdummy.SetMaximum(400); 
  hdummy.GetXaxis()->SetNdivisions(-204);
  hdummy.SetXTitle("Cluster size");
  hdummy.SetYTitle("L1Rpc Purity");
  hdummy.GetYaxis()->SetTitleOffset(1.7);
  hdummy.DrawCopy("axis");
  grPurClu->SetMarkerStyle(25);
  grPurClu->SetMarkerColor(2);
  grPurClu->Draw("same P E0"); // draw errors, disable clipping
  c->Update();
/*
  grPurClu->GetHistogram()->GetXaxis()->SetNdivisions(-5);
  grPurClu->GetHistogram()->GetXaxis()->SetLabelSize(0.035);
  if( region!="" ){ // barrel or endcap
    gr->GetHistogram()->GetYaxis()->SetTitle(Form("%s Puriciency",region.Data()));
  } else { // total
    gr->GetHistogram()->GetYaxis()->SetTitle("Puriciency");
  }
*/
  c->Modified();


  return c;
}
//////////////////////////////////////////////////////////

TCanvas* pRpcMiscRunHisto(TString region="")
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  //TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  //ff->cd();

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
  gr->SetMinimum(0.75);
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
  //TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  //ff->cd();

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
  h->DrawCopy("hist text0");
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
//  utilsPlotsSaver(pRpcMiscWrongPT());
  
//  gStyle->SetPaintTextFormat(".2E");  // modify text value style
//  utilsPlotsSaver(pRpcMiscTime());    // trigger timing
//  gStyle->SetPaintTextFormat();       // restore default style

  
//  int digits=TGaxis::GetMaxDigits();  // store current style
//  TGaxis::SetMaxDigits(4);            // modify style
//  utilsPlotsSaver(pRpcMiscRunHisto());// total eff
//  TGaxis::SetMaxDigits(digits);       // restore previous style

//  utilsPlotsSaver(pRpcMiscEffandPressVsRun());        // total eff

  //utilsPlotsSaver(pRpcMiscEffandPressVsRun("Barrel"));// barrel eff + press
  //utilsPlotsSaver(pRpcMiscEffandPressVsRun("Endcap"));// endcap eff + press
  //utilsPlotsSaver(pRpcMiscEffandClSizeVsRun("Barrel")); 
  //utilsPlotsSaver(pRpcMiscEffandClSizeVsRun("Endcap")); 

//  utilsPlotsSaver(pRpcMiscEffandHumidityVsRun());     // total eff + humidity 

  //utilsPlotsSaver(pRpcMiscEffandHumidityVsRun("Barrel"));// barrel eff + humidity 
  //utilsPlotsSaver(pRpcMiscEffandHumidityVsRun("Endcap"));// endcap eff + humidity 
/*
*/
   utilsPlotsSaver(pRpcMiscEffVsClu());
   utilsPlotsSaver(pRpcMiscPurVsRun());
   utilsPlotsSaver(pRpcMiscPurVsClu());

   utilsPlotsSaver(pRpcMiscEffVsRun("Pressure"));
   utilsPlotsSaver(pRpcMiscEffVsRun("Humidity"));
//   utilsPlotsSaver(pRpcMiscEffVsRun());
}
