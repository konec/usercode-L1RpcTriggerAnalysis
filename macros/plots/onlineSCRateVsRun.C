#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TLatex.h"
#include "TArrow.h"
#include "TAxis.h"
#include "TGaxis.h"
#include "TList.h"
#include "TSystemDirectory.h"
#include "TString.h"
#include <iomanip>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <string>

#include "utilsL1RpcStyle.C"
#include "utilsPlotsSaver.C"
#include "utilsHistoFromGraph.C"

// To produce plots run this commsnd:
//
// root -l -q plotSCrateVsRun.C++
//

/*
more RPCPressure.dat | cut -f 1,6 | sed s/"noise"/""/ | sed s/"HV SCAN RUN"/""/ | sed s/"Thr. scan"/""/ | cut -f -3 -d" "

 */

/////////////////////////////////////////////////////////

TH1D* Integrate(TH1I * histoD) {

  TH1D* histoI = new TH1D("hIntegrated",
			  "Integrated p_{T} code distribution;p_{T} code thr.;Counts/bin",
			  histoD->GetNbinsX(),
			  histoD->GetXaxis()->GetXmin(),
			  histoD->GetXaxis()->GetXmax());
  Double_t * cont = new Double_t [histoD->GetNbinsX()+2];  //with under+overflow
  Double_t * errs = new Double_t [histoD->GetNbinsX()+2];  //with under+overflow
  histoI->Reset();
  
  // bin=0 underf
  // bin 1-GetNbinsX() -conten
  // bin GetNbinsX()+1 overflow
  
  for (Int_t i = 0; i <= histoD->GetNbinsX()+1; i++) {
    cont[i] = (Double_t)(histoD->GetBinContent(i));   
    errs[i] = (Double_t)(histoD->GetBinError(i));
  }
  Double_t sum=0.;
  Double_t sume2=0.;
  for (Int_t i = histoD->GetNbinsX()+1; i >= 0; i--) {
    sum+=cont[i];
    sume2+=errs[i]*errs[i];
    histoI->SetBinContent(i,sum);
    histoI->SetBinError(i,sqrt(sume2));
  }
  delete [] cont;
  delete [] errs;
  return histoI;
}
//////////////////////////////////////////////////////

TH1D* getNormalisedRateHisto(TFile *file, int nMu, TString region="Barrel"){
  if(!file) {
    std::cout << "Wrong TFile pointer" << std::endl;
    return 0;
  }
  if(nMu<1 || nMu>4 || (region!="Barrel" && region!="Endcap")){
    std::cout << "Wrong muon index=" << nMu
	      << " or region=" << region << std::endl;
    return 0;
  }
  int index=nMu;
  if(region=="Endcap") index=nMu+4;
  string padName(Form("SC_fsHisits_%d",index));
  string runNumber = string(file->GetName()).substr(string(file->GetName()).find("run")+4,6);
  string hName(Form("FS_HIST_CODE_%.1s%d_ptCode",region.Data(),nMu-1));
  file->cd();
  TCanvas *aCanvas = (TCanvas *)file->Get("SC_fsHisits");
  if(!aCanvas){
    std::cout << "No TCanvas in file="<<file->GetName() << std::endl;
    return 0;
  }
  TPad *aPad = (TPad*)(aCanvas->GetPrimitive(padName.c_str()));
  if(!aPad){
    std::cout << "No TPad named " << padName << " in file="<<file->GetName() << std::endl;
    return 0;
  }
  TH1I *aHist = (TH1I*)(aPad->GetPrimitive(hName.c_str()));
  if(!aPad){
    std::cout << "No THI named " << hName << " in file=" << file->GetName() << std::endl;
    return 0;
  }
  //
  // get SC rate vs time graph and extract mean its value
  //
  // TODO
  //
  TH1D *hIntDouble = Integrate(aHist);
  hIntDouble->SetName(Form("FS_HIST_CODE_%.1s%d_ptCode_IntNorm_%s",region.Data(),nMu-1,runNumber.c_str()));
  hIntDouble->SetTitle(Form("%s normalised %d mu L1 RPC rate vs p_{T} code thr.;p_{T} code thr.;%s normalised %d mu L1 RPC rate",
			    region.Data(), nMu, region.Data(), nMu));
  ///Rewrite renormalised content of hIntDouble
  ///Put total rate into underflow bin
  hIntDouble->SetBinContent(0,(double)(hIntDouble->GetBinContent(1)));
  ///Put run number into overflow bin
  hIntDouble->SetBinContent(hIntDouble->GetNbinsX()+1,atoi(runNumber.c_str()));
  ///Put normalised rate
  if( hIntDouble->GetBinContent(0)>0 ) {
    for(int iBin=1;iBin<=hIntDouble->GetNbinsX();++iBin){
      Double_t val = (double)hIntDouble->GetBinContent(iBin)/(double)hIntDouble->GetBinContent(0);
      Double_t err = (double)hIntDouble->GetBinError(iBin)/(double)hIntDouble->GetBinContent(0);
      hIntDouble->SetBinContent(iBin, val);
      hIntDouble->SetBinError(iBin, err);
    }
  }
  // DEBUG
  //  if(runNumber=="199608") {
  //    std::cout << "# run=" << runNumber << std::endl
  //	      << "# hIntDouble["<<0<<"]=" 
  //	      << hIntDouble->GetBinContent(0) << std::endl
  //	      << "# hIntDouble["<<hIntDouble->GetNbinsX()+1<<"]="
  //	      << hIntDouble->GetBinContent(hIntDouble->GetNbinsX()+1) << std::endl;
  //    for(int iBin=1;iBin<=hIntDouble->GetNbinsX();++iBin){
  //      std::cout << "# hIntDouble["<<iBin<<"]="<<hIntDouble->GetBinContent(iBin)<<std::endl;
  //    }
  //  }

  return hIntDouble;
}
//////////////////////////////////////////////////////

TList* getListOfFiles(string dir){

  TSystemDirectory aDir("Directory",dir.c_str());
  TList *files = aDir.GetListOfFiles();

  return files;
}
//////////////////////////////////////////////////////

std::vector<TH1D*> getArrayOfNormalisedRateHisto(int nMu=1, TString region="Barrel"){
  std::vector<TH1D*> histos;
  if( region!="Barrel" && region!="Endcap" ) return histos;
  string oldpath=gSystem->pwd();
  string path = oldpath+"/MonitorHistos/"; // eg. "/home/akalinow/scratch/CMS/PAC/MonitorHistos/";
  TList *files = getListOfFiles(path); // this changes current PWD!
  if(!files) {
    std::cout << "No files in path=" << path << std::endl;
    return histos;
  }
  TIter next(files);
  TObject *obj = 0;
  int counter = 0;
  while ( (obj=next()) ){
    string fileName(obj->GetName());
    if(fileName.find(".root")==std::string::npos) continue;
    string runNumber = fileName.substr(fileName.find("run")+4,6);
    if(runNumber.find("_")!=std::string::npos) continue;

    std::cout << "Processing file=" << fileName << " run=" << runNumber << std::endl;

    TFile *file = new TFile((path+fileName).c_str());
    TH1D *h = getNormalisedRateHisto(file, nMu, region);

    if(h==0) {
      std::cout << "Result of getNormalisedRateHisto in NULL for file=" << fileName << std::endl;
      continue;
    }
    h->SetDirectory(0);
    histos.push_back(h);
    file->Close();
    delete file;
    counter++;
  }
  delete files;
  // restore old path
  TSystemDirectory aDir1("Directory1", oldpath.c_str());
  aDir1.GetListOfFiles();

  std::cout << "getArrayOfNormalisedRateHisto: size=" << histos.size() << " nMu=" << nMu << " region=" << region << std::endl;

  return histos;
}

//////////////////////////////////////////////////////
//
// Gets TGraphErrors with normalised rate for a given 
// set of parameters {# muons, pT code #, region}
// vs run number. 
//
//////////////////////////////////////////////////////
TGraphErrors* getNormalisedRateVsRunGraph(int nMu, int ptCode, TString region, 
					  std::vector<TH1D*> &histos) {
  if(nMu<1 || nMu>4 || (region!="Barrel" && region!="Endcap")) return 0;
  std::map<int,double> valMap;
  std::map<int,double> valMapErr;
  for(int i=0;i<histos.size();++i){
    if(!histos[i]) { 
      std::cout << "Null pointer at index=" << i << std::endl;
      continue;
    }
    Double_t val = histos[i]->GetBinContent(histos[i]->FindBin(ptCode));
    Double_t err = histos[i]->GetBinError(histos[i]->FindBin(ptCode));
    Double_t run = histos[i]->GetBinContent(histos[i]->GetNbinsX()+1);
    if( (double)(histos[i]->GetBinContent(0))<1e5) continue;  // skip runs with less than 100k triggers
    if(run<100000) continue;            // skip invalid run numbers
    if(val<1E-5 && err==0.0) continue;  // skip bins with low statistics

    if(valMap.find(run)!=valMap.end() || valMapErr.find(run)!=valMapErr.end()){
      std::cout<<"Run: "<<run<<" already exists!"<<std::endl;
    } else {
      std::cout<<"Run: "<<run<<std::endl;
    }
    valMap[run] = val;
    valMapErr[run] = err;
  }

  ///Put values into TGraphErrors
  const int nPoints = valMap.size();
  double x[nPoints], y[nPoints], e[nPoints];
  std::map<int,double>::iterator ci = valMap.begin();
  std::map<int,double>::iterator cei = valMapErr.begin();
  for(int i=0;ci!=valMap.end();++ci,++cei) if(ci->first==cei->first) {
    x[i] = ci->first;
    y[i] = ci->second;
    e[i] = cei->second;
    // DEBUG
    std::cout << "tgraph: i=" << i << "  " << x[i] << "  " << y[i] << "  " << e[i] << std::endl;
    // DEBUG
    ++i;
  }
  TGraphErrors *gr = new TGraphErrors(valMap.size(),x,y,0,e);
  if(!gr) return 0;
  gr->SetName(Form("Graph_%.1s_%d",region.Data(),ptCode));
  gr->SetTitle(Form("%s normalised L1 RPC rate for p_{T} code #gt %d vs run;Run;%s normalised rate",
		    region.Data(),ptCode,region.Data()));
  return gr;
}
//////////////////////////////////////////////////////
//
// Creates histogram (text labels) with normalised 
// rate vs run number from TGraph.
// Optionally only run numbers listed in runList will be
// taken into account.
//
//////////////////////////////////////////////////////
TH1D * getNormalisedRateVsRunHisto(TGraphErrors *hGraph){
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

  TH1D *histo = new TH1D(Form("hNormRateVsRun"),
			 "Normalised L1 RPC rate vs run;Run;Normalised L1 RPC rate",
			 nGoodRuns,-0.5,nGoodRuns-0.5);
  //histo->SetMaximum(1.0);
  //histo->SetMinimum(0.0); 
  histo->SetLineWidth(2);
  nGoodRuns=0;
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
//////////////////////////////////////////////////////

bool getMeanRmsFromHisto(TH1* histo, 
			 Double_t &mean, Double_t &rms,
			 bool onlyPositiveValues=false) {
  if(histo==NULL) { 
    std::cout<<"ERROR: Histogram is NULL!!!"<<std::endl;
    return false;
  }
  if(histo->GetNbinsX()<1) { 
    std::cout<<"ERROR: Hisgoram has 0 bins !!!"<<std::endl;
    return false;
  }
  Double_t sum=0.0;
  Double_t sumw=0.0;
  for(int i=1;i<=histo->GetNbinsX();++i) {
    Double_t val = histo->GetBinContent(i);
    Double_t err = histo->GetBinError(i);
    if(onlyPositiveValues && val<=0.0) continue; // skip empty points
    if(err<=0.0) continue; // skip empty points
    sum  += val/err/err;
    sumw += 1.0/err/err;
  }
  mean=sum/sumw;
  Double_t sum2=0.0;
  Double_t sumw2=0.0;
  for(int i=1;i<=histo->GetNbinsX();++i) {
    Double_t val = histo->GetBinContent(i);
    Double_t err = histo->GetBinError(i);
    if(onlyPositiveValues && val<=0.0) continue; // skip empty points
    if(err<=0.0) continue; // skip empty points
    sum2  += (val-mean)*(val-mean)/err/err;
    sumw2 += 1.0/err/err/err/err;
  }
  rms=sqrt( sumw/(sumw*sumw-sumw2)*sum2 ); // standard deviation for weighted sample
  return true;
}
//////////////////////////////////////////////////////

TCanvas* cSCRatePressureVsRun(int nMu,
			      int ptCode,
			      TString region,
			      std::vector<TH1D*> &histos) {
  if( region!="Barrel" && region!="Endcap" ) return 0;
  TCanvas* c = new TCanvas(Form("cSCRatePressVsRun_%dmu_code%d_%s",nMu, ptCode, region.Data()),
			   Form("%s L1 RPC normalised rate (%d mu, p_{T} code #geq %d) and UXC pressure vs run;Run;%s normalised rate",
				region.Data(), nMu, ptCode,region.Data()),
			   1200,600);
  // DEBUG
  std::cout << "cSCRatePressureVsRun: nMu=" << nMu << " ptCode=" << ptCode
	    << " region=" << region << " histos.size=" << histos.size() << std::endl;
  // DEBUG

  // plot rate
  TGraphErrors* gr = getNormalisedRateVsRunGraph(nMu, ptCode, region, histos);
  // DEBUG
  std::cout << "cSCRatePressureVsRun: gr=" << gr << std::endl;
  // DEBUG

  TH1D* hAll = getNormalisedRateVsRunHisto(gr);
  // DEBUG
  std::cout << "cSCRatePressureVsRun: hAll=" << hAll << std::endl;
  // DEBUG

  //hAll->SetMaximum(1.0);
  //hAll->SetMinimum(0.0);

  hAll->SetTitle(c->GetTitle());
  hAll->SetXTitle("");
  hAll->GetXaxis()->LabelsOption("v");  // vertical labels
  hAll->GetYaxis()->SetTitleOffset(1.3);
  hAll->SetYTitle(Form("%s L1 RPC normalised rate",region.Data()));
  hAll->Draw("E0"); // draw error bars, turn off symbol clipping
  TH1D * hpress = getPressVsRunHisto(hAll);
  hpress->Draw("same P E0");  // draw error bars, turn off symbol clipping
  
  //user comments
  printRunsOnPlot(hAll);

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

  return c;
}
//////////////////////////////////////////////////////

TCanvas* cSCRateClSizeVsRun(int nMu,
			    int ptCode,
			    TString region,
			    std::vector<TH1D*> &histos) {
  if( region!="Barrel" && region!="Endcap" ) return 0;
  TCanvas* c = new TCanvas(Form("cSCRateClSizeVsRun_%dmu_code%d_%s",nMu, ptCode, region.Data()),
			   Form("%s L1 RPC normalised rate (%d mu, p_{T} code #geq %d) and cl.size vs run;Run;%s normalised rate",
				region.Data(), nMu, ptCode,region.Data()),
			   1200,600);
  // DEBUG
  std::cout << "cSCRateClSizeVsRun: nMu=" << nMu << " ptCode=" << ptCode
	    << " region=" << region << " histos.size=" << histos.size() << std::endl;
  // DEBUG

  // plot rate
  TGraphErrors* gr = getNormalisedRateVsRunGraph(nMu, ptCode, region, histos);
  // DEBUG
  std::cout << "cSCRateClSizeVsRun: gr=" << gr << std::endl;
  // DEBUG

  TH1D* hAll = getNormalisedRateVsRunHisto(gr);
  // DEBUG
  std::cout << "cSCRateClSizeVsRun: hAll=" << hAll << std::endl;
  // DEBUG

  //hAll->SetMaximum(1.0);
  //hAll->SetMinimum(0.0);

  hAll->SetTitle(c->GetTitle());
  hAll->SetXTitle("");
  hAll->GetXaxis()->LabelsOption("v");  // vertical labels
  hAll->GetYaxis()->SetTitleOffset(1.3);
  hAll->SetYTitle(Form("%s L1 RPC normalised rate",region.Data()));
  hAll->Draw("E0"); // draw error bars, turn off symbol clipping
  TH1D * hcluster = 
    getClusterSizeVsRunHisto(hAll,
			     Form("ClusterSizeGraph_%s.root",region.Data()));
  hcluster->Draw("same P E0");  // draw error bars, turn off symbol clipping

  //user comments
  printRunsOnPlot(hAll);

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

  return c;
}
//////////////////////////////////////////////////////

void plotSCRateVsRun() {
  //
  // little cleanup
  //
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();

  if (std::string(gStyle->GetName()) != "L1RpcStyle" ) {
    if (0==gROOT->GetListOfStyles()->FindObject("L1RpcStyle")) utilsL1RpcStyle();
    gROOT->SetStyle("L1RpcStyle");
    gROOT->ForceStyle();
    gStyle->ls();
  }

  // 
  // vector of pT code thresholds 
  //
  std::vector<int> ptCode;
  ptCode.push_back(5);  
  ptCode.push_back(10);  
  ptCode.push_back(14);
  ptCode.push_back(16);
  ptCode.push_back(20);
  ptCode.push_back(25);

  //
  // vector of integrated, normalised rate histograms
  // for all pt codes
  //
  const int nMu=1;
  std::vector<TH1D*> histos_B = getArrayOfNormalisedRateHisto(nMu, "Barrel"); 
  std::vector<TH1D*> histos_E = getArrayOfNormalisedRateHisto(nMu, "Endcap"); 

  //
  // make plots
  //
  for(int icode=0; icode<ptCode.size(); icode++) {
    std::cout<<"Doing plots for PT code: "<<ptCode[icode]<<std::endl;
    utilsPlotsSaver
      ( cSCRatePressureVsRun(nMu, ptCode[icode], "Barrel", histos_B ) );
    utilsPlotsSaver
     ( cSCRatePressureVsRun(nMu, ptCode[icode], "Endcap", histos_E ) );
    utilsPlotsSaver
     ( cSCRateClSizeVsRun(nMu, ptCode[icode], "Barrel", histos_B ) );
    utilsPlotsSaver
     ( cSCRateClSizeVsRun(nMu, ptCode[icode], "Endcap", histos_E ) );
  }
}
