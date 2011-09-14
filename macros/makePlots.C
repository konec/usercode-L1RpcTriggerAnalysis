#include "TROOT.h"
#include "TSystem.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TFile.h"
#include "TStyle.h"
#include "TGaxis.h"
#include "TLine.h"
#include <map>
#include "FigUtils.C"


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
TCanvas * getDefaultCanvas(string name="c1", string title="c1", float x=10,float y=30,float w=650,float h=600){

  TCanvas *c1 = new TCanvas(name.c_str(),title.c_str(),x,y,w,h);
  c1->SetGrid(0,0);
  c1->SetFillStyle(4000);
  c1->SetLeftMargin(0.16);
  c1->SetGrid(1,1);
  c1->SetTicky();
  //c1->SetTickx();

  return c1;

}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void setupLegend(TLegend *leg){

  leg->SetFillStyle(4000);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);
  leg->SetTextSize(0.05);
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
TGraphErrors * getEffChangeVsRun(TGraphErrors *hGraph){

  int nPoints = hGraph->GetN();
  Double_t xVec[1000];
  Double_t yVec[1000];

  Double_t xTmp, yTmp;
  Double_t mean = 0;
  Double_t error = 0;
  Double_t sumErrors = 0;
  for(int i=0;i<nPoints;++i){
    hGraph->GetPoint(i,xTmp,yTmp);
    error = hGraph->GetErrorY(i);
    xVec[i] = xTmp;
    yVec[i] = yTmp;
    mean+=yTmp*(1.0/error);
    sumErrors+=(1.0/error);
  }
  mean/=sumErrors;
  for(int i=0;i<nPoints;++i){
    yVec[i]/=mean;
  }

  TGraphErrors *gr = (TGraphErrors*) new TGraph(nPoints,xVec,yVec);
  gr->SetMarkerStyle(25);
  gr->SetMarkerColor(2);
  return gr;
}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void plotEffVsEta(TFile *file, TFile *outFile=NULL){
  file->cd();   
  file->ls();

  //----chamber  efficiency
  TCanvas *c11 = getDefaultCanvas("EffVsPtVsEta","L1 RPC efficiency vs Eta vs p_{T}",12,53,1029,600);
  c11->SetLeftMargin(0.08);
  c11->SetRightMargin(0.15);

  TH2D *hEfficMuPtVsEta_N = (TH2D*)(gROOT->FindObject("hEfficMuPtVsEta_N"));
  TH2D *hEfficMuPtVsEta_D = (TH2D*)(gROOT->FindObject("hEfficMuPtVsEta_D"));
  hEfficMuPtVsEta_N->Divide(hEfficMuPtVsEta_D);

  hEfficMuPtVsEta_N->SetXTitle("#eta");
  hEfficMuPtVsEta_N->SetYTitle("p_{T} [GeV/c]");
  hEfficMuPtVsEta_N->SetZTitle("Efficiency");

  int iBinY = hEfficMuPtVsEta_N->GetYaxis()->FindBin(30);
  hEfficMuPtVsEta_N->GetYaxis()->SetRange(0,iBinY);
  hEfficMuPtVsEta_N->Draw("colz");

  c11->Print("png/EffVsPtVsEta.png");
  c11->Print("eps/EffVsPtVsEta.eps");
  c11->Print("C/EffVsPtVsEta.C","cxx");
  if(outFile) {
    outFile->cd();
    c11->Write();
    file->cd();
  }
  
  return;

  //Normalize
  for(int iBin=10;iBin<hEfficMuPtVsEta_N->GetNbinsX();++iBin){
    //Find normalisation factor
    TH1D *h = hEfficMuPtVsEta_N->ProjectionY("_px",iBin,iBin);
    int iBinX = hEfficMuPtVsEta_N->GetXaxis()->FindBin(25);
    float scale = h->Integral(iBinX,hEfficMuPtVsEta_N->GetNbinsX()+1);
    std::cout<<"scale: "<<scale<<std::endl;
    //scale/=(hEfficMuPtVsEta_N->GetNbinsX()-iBin);
    //h->Scale(1.0/scale);
    h->Draw();
    break;
  }

}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void plotEffVsRun(TFile *file, TFile *outFile=NULL){
  file->cd();   
  file->ls();

//-------------------- 
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);


 //----chamber  efficiency
  TCanvas *c11 = getDefaultCanvas("L1RPCEffVsRun","L1 RPC efficiency vs run",12,53,1376,421);
  c11->SetLeftMargin(0.05);
  c11->SetRightMargin(0.05);
  c11->Divide(3,1);

  TLatex *aLatex = new TLatex(0,0,"");
  aLatex->SetNDC(true);
  aLatex->SetTextSize(0.10);
  aLatex->SetTextAlign(21);
  float x=0.5, y;

  TLine *aLine = new TLine(56,0.8,56,1.0);
  aLine->SetLineColor(9);
  aLine->SetLineWidth(2);

  c11->cd(1);
  gPad->SetLeftMargin(0.14);
  gPad->SetRightMargin(0.08);
  gPad->SetFillStyle(4000);  // will be transparent
  TGraphErrors *hGraphRun = (TGraphErrors*)file->Get("hGraphRun");
  TH1D * hAll  = getEffVsRunHisto(hGraphRun);
  hAll->GetXaxis()->LabelsOption("v");
  hAll->GetYaxis()->SetTitleOffset(1.3);
  hAll->Draw();
  getPressVsRunHisto(hAll)->Draw("same P");
  y=0.80; 
  aLatex->DrawLatex(x,y,"|#eta|<1.6");
  //aLine->Draw();

  c11->cd(2);
  gPad->SetLeftMargin(0.08);
  gPad->SetRightMargin(0.14);
  gPad->SetFillStyle(4000);  // will be transparent
  TGraphErrors *hGraphRunBarrel = (TGraphErrors*)file->Get("hGraphRunBarrel");
  TH1D *hBarrel = getEffVsRunHisto(hGraphRunBarrel);
  hBarrel->GetXaxis()->LabelsOption("v");
  hBarrel->Draw();
  hBarrel->GetYaxis()->SetTitle("");
  getPressVsRunHisto(hBarrel)->Draw("same P");
  y=0.20; 
  aLatex->DrawLatex(x,y,"|#eta|<0.8");
  //aLine->Draw();

  c11->cd(3);
  gPad->SetLeftMargin(0.08);
  gPad->SetRightMargin(0.14);
  gPad->SetFillStyle(4000);  // will be transparent
  TGraphErrors *hGraphRunEndcap = (TGraphErrors*)file->Get("hGraphRunEndcap");
  TH1D* hEndcap = getEffVsRunHisto(hGraphRunEndcap);
  hEndcap->GetXaxis()->LabelsOption("v");
  hEndcap->Draw();
  hEndcap->GetYaxis()->SetTitle("");
  hEndcap->Print("all");
  TH1D *hPress = getPressVsRunHisto(hEndcap);
  hPress->Draw("same P");
  TGaxis *axis = new TGaxis(hPress->GetXaxis()->GetXmax(),hPress->GetMinimum(),
			    hPress->GetXaxis()->GetXmax(),hPress->GetMaximum(),
			    hPress->GetBinContent(0),
			    hPress->GetBinContent(hPress->GetNbinsX()+1),
			    510,"+L");
  axis->SetLineColor(kRed);
  axis->SetLabelColor(kRed);
  axis->SetTitleColor(kRed);
  axis->SetTitle("Pressure [mbar]");
  axis->SetTitleOffset(1.3);
  axis->Draw();
  y = 0.80;
  aLatex->DrawLatex(x,y,"0.8<|#eta|<1.6");
  //aLine->Draw();

  c11->Print("png/L1RPCEffVsRun.png");
  c11->Print("eps/L1RPCEffVsRun.eps");
  c11->Print("C/L1RPCEffVsRun.C","cxx");
  if(outFile) {
    outFile->cd();
    c11->Write();
    file->cd();
  }  

  //return;

  TGraphErrors *grAll = getEffChangeVsRun((TGraphErrors*)hGraphRun);
  TGraphErrors *grBarrel = getEffChangeVsRun((TGraphErrors*)hGraphRunBarrel);
  TGraphErrors *grEndcap = getEffChangeVsRun((TGraphErrors*)hGraphRunEndcap);

  c11->SetName("EffChangesVsRun");
  c11->SetTitle("L1RPC efficiency changes vs run");
  c11->cd(1);
  grAll->SetMinimum(0.95);
  grAll->SetMaximum(1.05);
  grAll->Draw("Paint E");
  grAll->GetHistogram()->SetXTitle("Run number");
  grAll->GetHistogram()->SetYTitle("Efficiency/Mean eff.");
  grAll->GetHistogram()->GetXaxis()->SetLabelSize(0.025);
  aLatex->DrawLatex(x,y,"|#eta|<1.6");

  c11->cd(2);
  grBarrel->SetMinimum(0.95);
  grBarrel->SetMaximum(1.05);
  grBarrel->Draw("Paint E");
  grBarrel->GetHistogram()->SetXTitle("Run number");
  grBarrel->GetHistogram()->SetYTitle("Efficiency/Mean eff.");
  grBarrel->GetHistogram()->GetXaxis()->SetLabelSize(0.025);
  aLatex->DrawLatex(x,y,"|#eta|<0.8");

  c11->cd(3);
  grEndcap->SetMinimum(0.95);
  grEndcap->SetMaximum(1.05);
  grEndcap->Draw("Paint E");
  grEndcap->GetHistogram()->SetXTitle("Run number");
  grEndcap->GetHistogram()->SetYTitle("Efficiency/Mean eff.");
  grEndcap->GetHistogram()->GetXaxis()->SetLabelSize(0.025);
  aLatex->DrawLatex(x,y,"0.8<|#eta|<1.6");

  c11->Print("png/EffChangesVsRun.png");
  c11->Print("eps/EffChangesVsRun.eps");
  c11->Print("C/EffChangesVsRun.C","cxx");
  if(outFile) {
    outFile->cd();
    c11->Write();
    file->cd();
  }  

}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void plotEffVsMu(TFile *file, TFile *outFile=NULL){
  file->cd();   
  file->ls();

  TCanvas *c3 = getDefaultCanvas("EffVsMu","L1RPC efficiency wrt global muon p_{T}");

  TGraphAsymmErrors *grEff = new TGraphAsymmErrors();
  TH1D *hEfficTkPt_N = (TH1D*)(gROOT->FindObject("hEfficTkPt_N"));
  TH1D *hEfficTkPt_D = (TH1D*)(gROOT->FindObject("hEfficTkPt_D"));
  grEff->Divide(hEfficTkPt_N,hEfficTkPt_D);
  grEff->SetMarkerStyle(25);
  grEff->SetMarkerColor(4);
  TGraphAsymmErrors *grEff1 = new TGraphAsymmErrors();
  TH1D *hEfficMuPt_N_overlap = (TH1D*)(gROOT->FindObject("hEfficMuPt_N_overlap"));
  TH1D *hEfficMuPt_D_overlap = (TH1D*)(gROOT->FindObject("hEfficMuPt_D_overlap"));
  grEff1->Divide(hEfficMuPt_N_overlap,hEfficMuPt_D_overlap);
  grEff1->SetMarkerStyle(25);
  grEff1->SetMarkerColor(2);

  TH1D *hEfficMuPt_N = (TH1D*)(gROOT->FindObject("hEfficMuPt_N"));
  hEfficMuPt_N->Reset();
  hEfficMuPt_N->SetMinimum(0.);
  hEfficMuPt_N->SetMaximum(1.002);
  hEfficMuPt_N->SetXTitle("p_{T} [GeV/c]");
  hEfficMuPt_N->SetYTitle("Efficiency");

  hEfficMuPt_N->Draw();  // draw frame
  grEff->Draw("EP");
  grEff1->Draw("EP");

  c3->Print("png/EffVsMu.png");
  c3->Print("eps/EffVsMu.eps");
  c3->Print("C/EffVsMu.C","cxx");
  if(outFile) {
    outFile->cd();
    c3->Write();
    file->cd();
  }  

}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void plotDeltaPt(TFile *file){
  file->cd();   
  file->ls();

  ///Choose specific run by setting the file name
  TH2D *hDeltaPtVsEta = (TH2D*)file->Get("hDeltaPtVsEta166841");

  TF1 *func = new TF1("func","gaus(0)",-1,2);
  func->SetParameter(0,1);
  func->SetParameter(1,1);
  func->SetParameter(2,1);

  TH1D *hShifts =  hDeltaPtVsEta->ProjectionX("hShifts");
  hShifts->Reset();

  //for(int iBinX=17;iBinX<18;++iBinX){
  for(int iBinX=0;iBinX<33;++iBinX){
  //for(int iBinX=5;iBinX<33;++iBinX){ //Tower 12
    TH1D *hProjTower = hDeltaPtVsEta->ProjectionY("hProjTower",iBinX,iBinX);
    std::cout<<"Low edge: "<<hDeltaPtVsEta->GetXaxis()->GetBinLowEdge(iBinX)<<std::endl;
    std::cout<<"Low edge: "<<hDeltaPtVsEta->GetXaxis()->GetBinUpEdge(34-iBinX)<<std::endl;

    hProjTower->Draw("same");
    hProjTower->Print();
    hProjTower->Fit(func);
    float meanShift = func->GetParameter(1);
    if(fabs(meanShift-1.0)<1e-5 || fabs(meanShift)>2) meanShift = 0.0;
    hShifts->SetBinContent(iBinX,meanShift);
    hShifts->SetBinError(iBinX,func->GetParError(1));
    func->SetParameter(0,1);
    func->SetParameter(1,1);
    func->SetParameter(2,1);
    //return;
  }

  hShifts->Draw("hist");

}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void plot(TFile *file){
  file->cd();   
  file->ls();

  TH2D* hDeltaPtVsEtaVsRun = (TH2D*)file->Get("hDeltaPtVsEtaVsRun");

  TH1D *hFrame =  hDeltaPtVsEtaVsRun->ProjectionY("hFrame",17,17);
  hFrame->Reset();
  hFrame->SetMinimum(0.0);
  hFrame->SetMaximum(1.0);
  hFrame->Draw();
  
  for(int iBinX=11;iBinX<33;++iBinX){
    TH1D *hProjTower = hDeltaPtVsEtaVsRun->ProjectionY("hProjTower",iBinX,iBinX);
    std::cout<<"Low edge: "<<hDeltaPtVsEtaVsRun->GetXaxis()->GetBinLowEdge(iBinX)<<std::endl;
    std::cout<<"Low edge: "<<hDeltaPtVsEtaVsRun->GetXaxis()->GetBinUpEdge(34-iBinX)<<std::endl;
    hProjTower->Draw("same E1");
    return;
  }


}
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
void makePlots(){

  TFile *file = new TFile("efficiencyHistos.root");
  if (file==NULL) return;
  file->ls();
  gStyle->SetPalette(1);

  // create output plot directories
  gSystem->mkdir("./C");
  gSystem->mkdir("./eps");
  gSystem->mkdir("./png");
  gSystem->mkdir("./root");

  TFile *outFile = new TFile("./root/efficiencyVsRunPlots.root", "RECREATE");
  // make plots  
  plotEffVsRun(file, outFile);
  plotEffVsMu(file, outFile);
  plotEffVsEta(file, outFile);
  //plotDeltaPt(file);
  //plot(file);
  outFile->Close();

}
