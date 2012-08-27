#include "TROOT.h"
#include "TFile.h"
#include "TGraph.h"
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

/////////////////////////////////////////////////////////

TH1D *getPressVsRunHisto(TH1D *histo){
  
  // create new histogram with same binning (run numbers)
  TH1D *h = (TH1D*)histo->Clone("h");
  h->Reset();
  ////Load pressure graph, and transfer into map
  TFile file("PressureGraph.root");
  TGraphErrors *graph = (TGraphErrors*)file.Get("Graph");
  if(!file.IsOpen() || !graph) return 0;
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
  file.Close();
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
  TFile file("HumidityGraph.root");
  TGraphErrors *graph = (TGraphErrors*)file.Get("Graph");
  if(!file.IsOpen() || !graph) return 0;
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
  file.Close();
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

void printRunsOnPlot(TH1D* histo, TString commentFile="runComments.txt") {
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
