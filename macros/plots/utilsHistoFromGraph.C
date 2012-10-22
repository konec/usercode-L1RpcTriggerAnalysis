#include "utilsHistoFromGraph.h"

#include "TROOT.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGaxis.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TLatex.h"
#include "TArrow.h"

#include <map>
#include <cmath>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>

/////////////////////////////////////////////////////////
TH1D runHistoFromGraph(const TGraph* gr)
{
  TH1D result;
  if (!gr) return result;
  unsigned int maxRun = 0;
  unsigned int minRun = 999999999;
  double xTmp, yTmp;
  for (int i=0; i<gr->GetN(); ++i) {
    gr->GetPoint(i,xTmp,yTmp);
    unsigned int aValue = static_cast<unsigned int>( xTmp+1.e-6);
    if (aValue < minRun) minRun = aValue;
    if (aValue > maxRun) maxRun = aValue;
  } 
  minRun = minRun/100*100-250;
  maxRun = maxRun/100*100+250;
  result = TH1D("hResult","hResult",maxRun-minRun+1, minRun-0.5, maxRun+0.5);

  //
  // decorations
  //
  result.SetXTitle("Run number");
  result.GetXaxis()->SetLabelSize(0.045);
  result.GetXaxis()->SetNdivisions(510);
  result.GetXaxis()->SetLabelOffset(0.006);
  result.GetYaxis()->SetTitleOffset(0.9);
  result.GetYaxis()->SetAxisColor(2);
  result.GetYaxis()->SetLabelColor(2);
  result.GetYaxis()->SetTitleColor(2);
  result.SetMarkerStyle(25);
  result.SetMarkerColor(2);

//
// set labels
//
//  int nBins = (maxRun-minRun+1); int nSep = 1; int last = 2;
//  while (nBins/nSep > 20) {
//    if (last==2) last = 5; else last = 2; 
//    nSep *= last;
//  }
//  for (unsigned int i=1; i< maxRun-minRun+1; ++i) {
//    int run = result.GetXaxis()->GetBinCenter(i);
//    std::stringstream s;
//    if (run/nSep*nSep==run) s<<run;
//    result.GetXaxis()->SetBinLabel(i,s.str().c_str());
//  }
//  result.LabelsOption("v");


  return result;
}

/////////////////////////////////////////////////////////
void fillHistoFromGraph(TH1D &histo, TGraph* graph)
{
  if (!graph) return;
  for (int i=0; i < graph->GetN(); ++i) {
    double run, value;
    graph->GetPoint(i,run, value);
    double valueErr=  graph->GetErrorY(i); // TGraph returns negative error values
    if(valueErr < 0.0) valueErr = 0.0;      // TGraphErrors returns true errors
    Int_t bin = histo.FindBin(run);    
    if (histo.GetBinContent(bin) > 0) continue; // skip already filled bins, should not happen
    histo.SetBinContent(bin,value);
    histo.SetBinError(bin,valueErr);
  }
}

//
// a copy of input histo but whith valueas changed to put it in Min,Max scale, Axis provided 
//
std::pair<TH1D,TGaxis* > getDatOnRef(const TH1D &histoDat, const TH1D &histoRef)
{
  float axisMax = histoRef.GetMaximum();
  float axisMin = histoRef.GetMinimum();

  double vMax = histoDat.GetMaximum();
  double vMin = vMax;
  for(Int_t i=1; i<= histoDat.GetNbinsX();++i) if (histoDat.GetBinContent(i) > 0. && histoDat.GetBinContent(i)<vMin) {
     vMin=histoDat.GetBinContent(i);
  }
  double  dx=vMax-vMin;
  double min = vMin-0.10*dx;
  double max = vMax+0.10*dx;

  TH1D hResult(histoDat);
  hResult.Reset("hGetDatOnRef");

  for(Int_t i=1; i<= histoDat.GetNbinsX();++i){
    double val = histoDat.GetBinContent(i);
    double err = histoDat.GetBinError(i); 
    float valNormalised =  (axisMax - axisMin)/(max - min)*(val - min) + axisMin;
    float errNormalised =  (axisMax - axisMin)/(max - min)*err;
    hResult.SetBinContent(i,valNormalised);
    hResult.SetBinError(i,errNormalised);
  }
  hResult.SetLineColor(4);
  hResult.SetMarkerColor(4);
  hResult.SetMarkerSize(0.6);
  hResult.SetMarkerStyle(21);
  
  double atValue = histoRef.GetXaxis()->GetXmax();
  TGaxis* axis=new TGaxis(atValue, axisMin, atValue, axisMax-1.e-5, min, max, 510,"+SLI");
//  TGaxis* axis = new TGaxis(200000, 1.6, 200000, 3.0, 900, 1000, 510, "+SLI");
  axis->SetLineColor(hResult.GetLineColor());
  axis->SetLabelColor(hResult.GetLineColor());
  axis->SetTitleColor(hResult.GetLineColor());
  axis->SetLabelSize(histoRef.GetYaxis()->GetLabelSize());
  axis->SetLabelOffset(histoRef.GetYaxis()->GetLabelOffset());
  axis->SetLabelFont(histoRef.GetYaxis()->GetLabelFont());
  axis->SetTitleSize(histoRef.GetYaxis()->GetTitleSize());
  axis->SetTitleOffset(histoRef.GetYaxis()->GetTitleOffset());
  axis->SetTitleFont(histoRef.GetYaxis()->GetTitleFont());
  axis->SetName("My axis");

  return std::make_pair(hResult,axis);
}



/////////////////////////////////////////////////////////
TH1D *getPressVsRunHisto(TH1D *histo){
  
  // create new histogram with same binning (run numbers)
  TH1D *h = (TH1D*)histo->Clone("h");
  h->Reset();
  ////Load pressure graph, and transfer into map
//  TFile file("PressureGraph.root");
//  TGraphErrors *graph = (TGraphErrors*)file.Get("Graph");
//  if(!file.IsOpen() || !graph) return 0;
  TGraph* graph = (TGraph*)gROOT->FindObject("gr_Pressure"); 
  Double_t xTmp, yTmp, yErr;
  std::map<int,float> aMap;
  std::map<int,float> aMapErr;
  float max = -9999;
  float min = 9999;
  for(int i=0;i<graph->GetN();++i){
    graph->GetPoint(i,xTmp,yTmp);
    if(yTmp<=100) continue; // skip this run
    aMap[xTmp] = yTmp;
    yErr = graph->GetErrorY(i); // TGraph returns negative error values
    if(yErr<0.0) yErr=0.0;      // TGraphErrors returns true errors
    aMapErr[xTmp] = yErr; 
    // for min/max calculation use runs from the reference histogram only
    int ibin=h->GetXaxis()->FindBin(Form("%d",int(xTmp)));
    if(ibin<1 || ibin==h->GetNbinsX()+1) continue; // skip this run
    if(yTmp>max) max = yTmp;
    if(yTmp<min) min = yTmp;
  }
  //file.Close();
  // add 10% and round to the nearest multiple of 5 so axis labels look nicer
  //double dx=max-min;
  //min = (int((min-0.10*dx)/5.0))*5.0;
  //max = (int((max+0.10*dx)/5.0))*5.0;
  // add 10%
  double dx=max-min;
  min = min-0.10*dx;
  max = max+0.10*dx;
  /////////////////////// 
  float axisMin = histo->GetMinimum();
  float axisMax = histo->GetMaximum();
  /////
  for(int i=1;i<=h->GetNbinsX();++i){
    int run = atoi(h->GetXaxis()->GetBinLabel(i));
    float val = 0.0;    
    float err = 0.0;
    if(aMap.find(run)!=aMap.end()) {
      val = aMap[run];
      err = aMapErr[run];
    }
    float valNormalised =  (axisMax - axisMin)/(max - min)*(val - min) + axisMin;
    float errNormalised =  (axisMax - axisMin)/(max - min)*err;
    //if(fabs(val)<1e-5) valNormalised = 0.0;
    h->SetBinContent(i,valNormalised);           
    h->SetBinError(i,errNormalised);           
  }
  h->SetBinContent(0,min);                // store unscaled MIN value
  h->SetBinContent(h->GetNbinsX()+1,max); // store unscaled MAX value
  h->SetLineColor(2);
  h->SetMarkerColor(2);
  h->SetMarkerStyle(21);
  
  return h;
}
/////////////////////////////////////////////////////////

TH1D *getHumidityVsRunHisto(TH1D *histo){
  
  // create new histogram with same binning (run numbers)
  TH1D *h = (TH1D*)histo->Clone("h");
  h->Reset();
  ////Load pressure graph, and transfer into map
  //TFile file("HumidityGraph.root");
  //TGraphErrors *graph = (TGraphErrors*)file.Get("Graph");
  //if(!file.IsOpen() || !graph) return 0;
  TGraphErrors *graph = (TGraphErrors*)gROOT->FindObject("gr_Humidity");
  Double_t xTmp, yTmp, yErr;
  std::map<int,float> aMap;
  std::map<int,float> aMapErr;
  float max = -9999;
  float min = 9999;
  for(int i=0;i<graph->GetN();++i){
    graph->GetPoint(i,xTmp,yTmp);
    if(yTmp>100) continue; // skip this run
    aMap[xTmp] = yTmp;
    yErr = graph->GetErrorY(i); // TGraph returns negative error values
    if(yErr<0.0) yErr=0.0;      // TGraphErrors returns true errors
    aMapErr[xTmp] = yErr; 
    // for min/max calculation use runs from the reference histogram only
    int ibin=h->GetXaxis()->FindBin(Form("%d",int(xTmp)));
    if(ibin<1 || ibin==h->GetNbinsX()+1) continue; // skip this run
    if(yTmp>max) max = yTmp;
    if(yTmp<min) min = yTmp;
  }
  //file.Close();
  // add 10% and round to the nearest multiple of 5 so axis labels look nicer
  //double dx=max-min;
  //min = (int((min-0.10*dx)/5.0))*5.0;
  //max = (int((max+0.10*dx)/5.0))*5.0;
  // add 10%
  double dx=max-min;
  min = min-0.10*dx;
  max = max+0.10*dx;
  /////////////////////// 
  float axisMin = histo->GetMinimum();
  float axisMax = histo->GetMaximum();
  /////
  for(int i=1;i<=h->GetNbinsX();++i){
    int run = atoi(h->GetXaxis()->GetBinLabel(i));
    float val = 0.0;    
    float err = 0.0;
    if(aMap.find(run)!=aMap.end()) {
      val = aMap[run];
      err = aMapErr[run];
    }
    float valNormalised =  (axisMax - axisMin)/(max - min)*(val - min) + axisMin;
    float errNormalised =  (axisMax - axisMin)/(max - min)*err;
    //if(fabs(val)<1e-5) valNormalised = 0.0;
    h->SetBinContent(i,valNormalised);           
    h->SetBinError(i,errNormalised);           
  }
  h->SetBinContent(0,min);                // store unscaled MIN value
  h->SetBinContent(h->GetNbinsX()+1,max); // store unscaled MAX value
  h->SetLineColor(2);
  h->SetMarkerColor(2);
  h->SetMarkerStyle(21);
  
  return h;
}
/////////////////////////////////////////////////////////

TH1D *getClusterSizeVsRunHisto(TH1D *histo, const char *fname){
  
  // create new histogram with same binning (run numbers)
  TH1D *h = (TH1D*)histo->Clone("h");
  h->Reset();
  ////Load cluster size graphs, and transfer into map
  TFile file(fname);
  TGraph *graph = (TGraph*)file.Get("Graph");
  if(!file.IsOpen() || !graph) return 0;
  Double_t xTmp, yTmp;
  std::map<int,float> aMap;
  float max = -9999;
  float min = 9999;
  for(int i=0;i<graph->GetN();++i){
    graph->GetPoint(i,xTmp,yTmp);
    aMap[xTmp] = yTmp;
    // for min/max calculation use runs from the reference histogram only
    int ibin=h->GetXaxis()->FindBin(Form("%d",int(xTmp)));
    if(ibin<1 || ibin==h->GetNbinsX()+1) continue; // skip this run
    if(yTmp>max) max = yTmp;
    if(yTmp<min) min = yTmp;
  }
  file.Close();
  // add 10% and round to the nearest multiple of 5 so axis labels look nicer
  //double dx=max-min;
  //min = (int((min-0.10*dx)/5.0))*5.0;
  //max = (int((max+0.10*dx)/5.0))*5.0;
  // add 10%
  double dx=max-min;
  min = min-0.1*dx;
  max = max+0.1*dx;
  /////////////////////// 
  float axisMin = histo->GetMinimum();
  float axisMax = histo->GetMaximum();
  /////
  for(int i=1;i<=h->GetNbinsX();++i){
    int run = atoi(h->GetXaxis()->GetBinLabel(i));
    float val = 0.0;    
    if(aMap.find(run)!=aMap.end()) val = aMap[run];
    float valNormalised =  (axisMax - axisMin)/(max - min)*(val - min) + axisMin;
    //if(fabs(val)<1e-5) valNormalised = 0.0;
    h->SetBinContent(i,valNormalised);           
  }
  h->SetBinContent(0,min);                // store unscaled MIN value
  h->SetBinContent(h->GetNbinsX()+1,max); // store unscaled MAX value
  h->SetLineColor(2);
  h->SetMarkerColor(2);
  h->SetMarkerStyle(21);
  
  return h;
}
//////////////////////////////////////////////////////

void printRunsOnPlotLinScale(TH1D* histo) {
  TString commentFile="runComments.txt";
  std::map<int, string> runs;
  std::ifstream comm;
  comm.open(commentFile,std::ofstream::in);
  while(comm.good()) {
    string line;
    getline(comm, line);
    // format of each line (>= 8 characters)
    // NNNNNN Any Run Comment
    int run=atoi(line.substr(0,6).c_str());
    if(line.length()<8 || run<100000) continue; // skip bad run numbers
    runs[run]=line;
  };
  comm.close();

  double y,yMin,dy;
  yMin=histo->GetMinimum();
  dy=histo->GetMaximum()-histo->GetMinimum();
  y=yMin+0.1*dy;
  TLatex aLatex;
  aLatex.SetTextColor(kBlack);
  aLatex.SetTextSize(0.03);
  aLatex.SetTextAlign(12);
  aLatex.SetTextAngle(90+0);

  TArrow aArrow;
  aArrow.SetArrowSize(0.01);
  aArrow.SetFillColor(kBlack);
  aArrow.SetFillStyle(1001);
  aArrow.SetLineColor(kBlack);
  aArrow.SetLineWidth(3);
		 
  for(std::map<int,string>::iterator it=runs.begin();it!=runs.end();it++) {
    std::cout <<"DRAWING AT: " << it->first<<"  y="<<y <<" what: "<<it->second.c_str() << std::endl;
    double x = it->first;
    if (x < histo->GetBinCenter(1) ) continue;
    if (x > histo->GetBinCenter( histo->GetNbinsX()) ) continue;
    aLatex.DrawLatex(x,y,it->second.c_str());
    aArrow.DrawArrow(x,y-dy*0.01,x,yMin+0.01*dy);
  }
}

void printRunsOnPlot(TH1D* histo) {
  TString commentFile="runComments.txt";
  std::map<int, string> runs;
  std::ifstream comm;
  comm.open(commentFile,std::ofstream::in);
  while(comm.good()) {
    string line;
    getline(comm, line);
    // format of each line (>= 8 characters)
    // NNNNNN Any Run Comment
    int run=atoi(line.substr(0,6).c_str());
    if(line.length()<8 || run<100000) continue; // skip bad run numbers
    runs[run]=line;
  };
  comm.close();
  // add the first and the last run
  if(runs.find(atoi(histo->GetXaxis()->GetBinLabel(1)))==runs.end()) {
    runs[atoi(histo->GetXaxis()->GetBinLabel(1))]=
      histo->GetXaxis()->GetBinLabel(1);
  }
  if(runs.find(atoi(histo->GetXaxis()->GetBinLabel(histo->GetNbinsX())))==runs.end()) {
    runs[atoi(histo->GetXaxis()->GetBinLabel(histo->GetNbinsX()))]=
      histo->GetXaxis()->GetBinLabel(histo->GetNbinsX());
  }

  double x,y,yMin,dy;
  yMin=histo->GetMinimum();
  dy=histo->GetMaximum()-histo->GetMinimum();
  y=yMin+0.3*dy;
  TLatex *aLatex = new TLatex();
  TArrow *aArrow = new TArrow();
  aArrow->SetArrowSize(0.01);
  aArrow->SetFillColor(kBlue);
  aArrow->SetFillStyle(1001);
  aLatex->SetTextColor(kBlue);
  aLatex->SetTextSize(0.03);
  aArrow->SetLineColor(kBlue);
  aArrow->SetLineWidth(3);
		 
  for(std::map<int,string>::iterator it=runs.begin();it!=runs.end();it++) {
    for(int iBin=1;iBin<=histo->GetNbinsX();++iBin){
      string runNumber = Form("%06d", it->first);
      string label(histo->GetXaxis()->GetBinLabel(iBin));
      string prev_label(histo->GetXaxis()->GetBinLabel(iBin-1));
      x = histo->GetBinCenter(iBin);
      bool modifyText=(iBin!=1) && 
	atoi(runNumber.c_str())<atoi(label.c_str()) && 
	atoi(runNumber.c_str())>atoi(prev_label.c_str());
      if(runNumber==label || modifyText) {
	if((double)(x-histo->GetBinCenter(1))/
	   (double)(histo->GetBinCenter(histo->GetNbinsX())-histo->GetBinCenter(1))<0.94) {
	  // avoid overlaying Y axis labels
	  aLatex->SetTextAlign(13); // default, bottom
	  aLatex->SetTextAngle(90-30);
	} else {
	  aLatex->SetTextAlign(11); // top
	  aLatex->SetTextAngle(90+0);
	}
	string displayText = it->second;
	if(modifyText) displayText=">"+displayText;
	aLatex->DrawLatex(x,y,displayText.c_str());
	aArrow->DrawArrow(x,y-dy*0.01,x,yMin);
      }
    }
  }
}
//void printRunsOnPlot(TH1D* histo, TString commentFile="runComments.txt") {
