#include<TROOT.h>
#include<TFile.h>
#include<TGraph.h>
#include<TGraphErrors.h>
#include<TCanvas.h>
#include<TPad.h>
#include<TStyle.h>
#include<TH1.h>
#include<TLatex.h>
#include<TArrow.h>
#include<TAxis.h>
#include<TGaxis.h>
#include<TList.h>
#include<TSystemDirectory.h>
#include<TSystem.h>
#include<iostream>
#include<vector>
#include<map>
#include<algorithm>
#include<string>
/*
more RPCPressure.dat | cut -f 1,6 | sed s/"noise"/""/ | sed s/"HV SCAN RUN"/""/ | sed s/"Thr. scan"/""/ | cut -f -3 -d" "

 */
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

TH1I * Integrate(TH1I * histoD) {

   TH1I * histoI = new TH1I(*histoD); 
   Int_t *  cont = new Int_t [histoD->GetNbinsX()+2];  //with under+overflow
   Int_t *  errs = new Int_t [histoD->GetNbinsX()+2];  //with under+overflow
   histoI->Reset();
   
// bin=0 underf
// bin 1-GetNbinsX() -conten
// bin GetNbinsX()+1 overflow

   Int_t i;
   for (i = 0; i <= histoD->GetNbinsX()+1; i++) { 
      cont[i] = histoD->GetBinContent(i);   
      errs[i] = histoD->GetBinError(i);
   }
   Double_t sum=0.;
   Double_t sume2=0.;
   ////////////////////
   for (i = histoD->GetNbinsX()+1; i >= 0; i--) {
        sum+=cont[i];
        sume2+=errs[i]*errs[i];
	///////////////////////////
        histoI->SetBinContent(i,(Int_t)sum);
        histoI->SetBinError(i,sqrt(sume2));
   }   
   //for (i = 0; i<=histoI->GetNbinsX()+1;i++){
   //  cout <<"bin: "<<i<<" cont: "<<histoI->GetBinContent(i);
   //  cout <<            " error: "<<histoI->GetBinError(i)<<endl;
   //   }

   return histoI;
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
std::vector<int> getRunListFromGraph(TGraph *graph) {

  std::vector<int> runList;
  if(graph) {
    int nPoints = graph->GetN();
    for(int i=0;i<nPoints;++i) {
      Double_t xTmp, yTmp;
      graph->GetPoint(i,xTmp,yTmp);
      // avoid duplications
      if(std::find(runList.begin(),runList.end(),(int)xTmp)==runList.end()) runList.push_back((int)xTmp);
    }
  }
  // DEBUG
  for(int i=0; i<runList.size(); ++i) std::cout<<"getRunListFromGraph runList["<<i<<"]="<<runList[i]<<std::endl;
  // DEBUG
  return runList;
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
TH1F* getNormalisedRateHistoForRun(TFile *file, int nMu, string region="B"){

  if(nMu>4 || (region!="B" && region!="E")){
    std::cout<<"Wrong muon index: "<<nMu
	     <<" or wrong region: "<<region
	     <<std::endl;
  }

  char padName[100];
  int index=nMu;
  if(region=="E") index=nMu+4;
  sprintf(padName,"SC_fsHisits_%d",index);
  string runNumber = string(file->GetName()).substr(string(file->GetName()).find("run")+4,6);

  char hName[100];
  sprintf(hName,"FS_HIST_CODE_%s%d_ptCode",region.c_str(),nMu-1);
  file->cd();
  TCanvas *aCanvas = (TCanvas *)file->Get("SC_fsHisits");
  if(!aCanvas){
    std::cout<<"No canvas in file: "<<file->GetName()<<std::endl;
    return 0;
  }
  TPad *aPad = (TPad*)(aCanvas->GetPrimitive(padName)); 
  TH1I *aHist = (TH1I*)(aPad->GetPrimitive(hName));
  TH1I *hInt =  Integrate(aHist);
  sprintf(hName,"FS_HIST_CODE_%s%d_ptCode_Int_%s",region.c_str(),nMu-1,runNumber.c_str());
  hInt->SetName(hName);
  char hIntName[100];
  sprintf(hIntName,"hIntFloat_%s_%s",region.c_str(),runNumber.c_str());
  TH1F *hIntFloat = new TH1F(hIntName, // "hIntFloat",
			     hInt->GetTitle(),
			     hInt->GetNbinsX(),
			     hInt->GetXaxis()->GetXmin(),hInt->GetXaxis()->GetXmax());
  hIntFloat->SetXTitle(hInt->GetXaxis()->GetTitle());
  hIntFloat->SetYTitle("Normalised rate");
  sprintf(hName,"FS_HIST_CODE_%s%d_ptCode_IntFloat_%s",region.c_str(),nMu-1,runNumber.c_str());
  hIntFloat->SetName(hName);
  ///Rewrite renormalised content of TH1I 
  ///Put total rate into underflow bin
  hIntFloat->SetBinContent(0,hInt->GetBinContent(1));
  ///Put run number into overflow bin
  hIntFloat->SetBinContent(hInt->GetNbinsX()+1,atoi(runNumber.c_str()));
  ///Put normalised rate
  // DEBUG
  //std::cout<<"### getNormalisedRateHistoForRun: file="<<file->GetName()<<" nMu="<<nMu<<" region="<<region<<std::endl;
  // DEBUG
  if(hIntFloat->GetBinContent(0)>0) {
    for(int iBin=1;iBin<=hInt->GetNbinsX();++iBin){
      hIntFloat->SetBinContent(iBin,(float)hInt->GetBinContent(iBin)/(float)hInt->GetBinContent(1));
      hIntFloat->SetBinError(iBin,(float)hInt->GetBinError(iBin)/(float)hInt->GetBinContent(1));
      // DEBUG
      //std::cout<<"### getNormalisedRateHistoForRun: iBin="<<iBin<<" val="<<hIntFloat->GetBinContent(iBin)
      //	       <<" err="<<hIntFloat->GetBinError(iBin)<<std::endl;
      // DEBUG
    }
  }
  /////////////////
  delete aCanvas;
  gROOT->Reset();
  //delete aPad;
  //delete aHist;
  //delete hInt;
  /////////////////
  return hIntFloat;
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
TList* getListOfFiles(string dir){

  TSystemDirectory aDir("Directory",dir.c_str());
  TList *files = aDir.GetListOfFiles();

  return files;
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
std::vector<TH1F*> getNormalisedRateHistoVecForRuns(int nMu, string region){

  string oldpath=gSystem->pwd();
  string path = oldpath+"/MonitorHistos/"; // "/home/akalinow/scratch/CMS/PAC/MonitorHistos/";
  vector<TH1F*> histos;
  TList *files = getListOfFiles(path); // this changes current PWD!
  TIter next(files);
  TObject *obj = 0;
  int counter = 0;
  while ( obj=next() ){
    string fileName(obj->GetName());
    if(fileName.find(".root")==std::string::npos) continue;
    string runNumber = fileName.substr(fileName.find("run")+4,6);
    if(runNumber.find("_")!=std::string::npos) continue;
    //if(runNumber.find("167163")==std::string::npos) continue;
    //std::cout<<fileName<<" "<<runNumber<<std::endl;
    TFile *file = new TFile((path+fileName).c_str());
    TH1F *h = getNormalisedRateHistoForRun(file,nMu,region);
    if(h==0) continue;
    h->SetDirectory(0);
    histos.push_back(h);
    delete file;
    counter++;
    // DEBUG
    //if(counter>500) break;
    // DEBUG
  }
  delete files;

  //TSystemDirectory aDir1("Directory1","/home/akalinow/scratch/CMS/PAC/soft/");
  TSystemDirectory aDir1("Directory1", oldpath.c_str());
  aDir1.GetListOfFiles();

  return histos;
}
//////////////////////////////////////////////////////
// 
// Superimpose integrated rate histograms for all runs
// for a given set of parameters {# muons, region}.
//
//////////////////////////////////////////////////////
void plotNormalisedRateForRuns(int nMu, string region){

  vector<TH1F*> histos =  getNormalisedRateHistoVecForRuns(nMu,region);

  TCanvas *c1 = new TCanvas("c1","Rates for runs");
  c1->Draw();
  bool isFrame=false;
  int minRun = 999999;
  int maxRun = -999999;
  for(int i=0;i<histos.size();++i){
    if(histos[i]->GetBinContent(0)<1e5) continue;
    if(histos[i]->GetBinContent(histos[i]->GetNbinsX()+1)>maxRun){
      maxRun = histos[i]->GetBinContent(histos[i]->GetNbinsX()+1);
    }
    if(histos[i]->GetBinContent(histos[i]->GetNbinsX()+1)<minRun){
      minRun = histos[i]->GetBinContent(histos[i]->GetNbinsX()+1);
    }
    histos[i]->SetStats(kFALSE);
    histos[i]->SetTitle("");
    if(!isFrame){
      histos[i]->DrawCopy("");
      isFrame=true;
    }
    else histos[i]->DrawCopy("same");
  }
  
  char name[100];
  TLatex *aLatex = new TLatex(0,0,"");
  if(region=="B") aLatex->DrawLatex(18,0.9,"Barrel");
  if(region=="E") aLatex->DrawLatex(18,0.9,"Endcap");
  sprintf(name,"Runs: %d - %d",minRun,maxRun);
  aLatex->DrawLatex(18,0.85,name);
  c1->Modified();
  c1->Update();
  sprintf(name,"eps/RateVsRun_%d_%s_Collisions.eps",nMu,region.c_str());
  c1->Print(name);
  sprintf(name,"png/RateVsRun_%d_%s_Collisions.png",nMu,region.c_str());
  c1->Print(name);
  sprintf(name,"C/RateVsRun_%d_%s_Collisions.C",nMu,region.c_str());
  c1->Print(name);
}
//////////////////////////////////////////////////////
//
// Gets TGraph with normalised rate for a given 
// set of parameters {# muons, pT code #, region}
// vs run number. 
//
//////////////////////////////////////////////////////
TGraphErrors* getNormalisedRateVsRunGraph(int nMu, string region,int ptCode=13){

  vector<TH1F*> histos =  getNormalisedRateHistoVecForRuns(nMu,region);
  std::map<int,float> valMap;
  std::map<int,float> valMapErr;
  ////Load pressure graph, and transfer into map
  //TFile *file = new TFile("PressureGraph.root");
  //TGraph *graph = (TGraph*)file->Get("Graph");
  //Double_t xTmp, yTmp;
  //std::map<int,float> aMap;
  //float max = -9999;
  //float min = 9999;
  //for(int i=0;i<graph->GetN();++i){
  //  graph->GetPoint(i,xTmp,yTmp);     // x=run, y=pressure
  //  aMap[xTmp] = yTmp;
  //  if(yTmp>max) max = yTmp;
  //  if(yTmp<min && yTmp>100) min = yTmp;
  //}
  ////////////////////////////////////////////////

  for(int i=0;i<histos.size();++i){
    float val = histos[i]->GetBinContent(histos[i]->FindBin(ptCode));
    float err = histos[i]->GetBinError(histos[i]->FindBin(ptCode));
    /////if(region=="B" && (val<0.05 || val>0.2)) continue;
    /////if(region=="E" && val<0.05) continue;
    float run = histos[i]->GetBinContent(histos[i]->GetNbinsX()+1);
    //if(aMap.find(run)==aMap.end()) continue;
    //if(histos[i]->GetBinContent(0)<1e5) continue;  // skip histograms with less than 100k triggers
    if(run<100000) continue;                       // skip invalid run numbers
    /////std::cout<<"run: "<<run<<std::endl;
    if(valMap.find(run)!=valMap.end() || valMapErr.find(run)!=valMapErr.end()){
      std::cout<<"Run: "<<run<<" already analysed!"<<std::endl;
    }
    valMap[run] = val;
    valMapErr[run] = err;
  }

  ///Put values into TGraph
  Double_t x[10000], y[10000], e[10000];
  std::map<int,float>::iterator ci = valMap.begin();
  std::map<int,float>::iterator cei = valMapErr.begin();
  for(int i=0;ci!=valMap.end();++ci,++cei) if(ci->first==cei->first) {
    x[i] = ci->first;
    y[i] = ci->second;
    e[i] = cei->second;
    ++i;
  }

  TGraphErrors *gr = new TGraphErrors(valMap.size(),x,y,0,e);
  gr->SetName("gNormRateVsRun");
  gr->SetTitle("Normalised L1 RPC rate vs Run;Run;Normalised rate");
  // DEBUG
  //std::cout<<"### getNormalisedRateVsRunGraph: nMu="<<nMu<<" region="<<region<<" ptCode="<<ptCode<<std::endl;
  //for(int i=1;i<gr->GetN();++i) {
  //    Double_t xTmp, yTmp, eTmp;
  //    gr->GetPoint(i,xTmp,yTmp);
  //    eTmp = gr->GetErrorY(i);
  //   std::cout<<"### getNormalisedRateVsRunGraph: run="<<xTmp<<" rate="<<yTmp<<" +- "<<eTmp<<std::endl;
  //}
  // DEBUG
  return gr;
}
//////////////////////////////////////////////////////
//
// Creates histogram (text labels) with normalized 
// rate vs run number from TGraph.
// Optionally only run numbers listed in runList will be
// taken into account.
//
//////////////////////////////////////////////////////
TH1F* getHistoFromNormalisedRateVsRunGraph(TGraphErrors *graph,
					   std::vector<int> runList=std::vector<int>()) {
  if(!graph) return NULL;
  // transfer graph data to a map
  std::map<int,float> aMap;
  std::map<int,float> aMapErr;
  for(int i=0;i<graph->GetN();++i) {
    Double_t xTmp, yTmp, eTmp;
    graph->GetPoint(i,xTmp,yTmp);
    eTmp=graph->GetErrorY(i);
    // DEBUG
    //std::cout<<"x="<<xTmp<<" y="<<yTmp<<" err="<<eTmp<<std::endl;
    // DEBUG
    aMap[xTmp]=yTmp;
    aMapErr[xTmp]=eTmp;
  }
  int nPoints;
  if(runList.size()) nPoints = runList.size();
  else               nPoints = aMap.size();
  TH1F* histo = new TH1F("h_NormRateVsRun","",nPoints,-0.5,nPoints-0.5);
  histo->SetMaximum(1.);
  histo->SetMinimum(0.0); 
  histo->SetYTitle("L1 RPC normalised rate");
  histo->SetLineWidth(2);
  // set text bin labels
  if(runList.size()) {
    for(int i=0;i<nPoints;++i) {
      histo->GetXaxis()->SetBinLabel(i+1,Form("%d",(int)(runList[i]))); 
      // DEBUG
      //std::cout<<"### getHistoFromNormalisedRateVsRunGraph GetBinLabel("<<i+1<<")="
      //	       <<histo->GetXaxis()->GetBinLabel(i+1)<<" runList["<<i<<"]="<<runList[i]<<std::endl;
      // DEBUG
    }
  } else {
    std::map<int,float>::iterator it=aMap.begin();
    std::map<int,float>::iterator ite=aMapErr.begin();
    for(int i=0;i<nPoints;++i,++it, ++ite) {
      histo->GetXaxis()->SetBinLabel(i+1,Form("%d",(int)(it->first))); 
      // DEBUG
      //std::cout<<"### getHistoFromNormalisedRateVsRunGraph GetBinLabel("<<i+1<<")="
      //	       <<histo->GetXaxis()->GetBinLabel(i+1)<<" aMap["<<it->first<<"]="<<it->second<<" +- "<<ite->second<<std::endl;
      // DEBUG
    }
  }
  // fill histogram
  std::map<int,float>::iterator it,ite;
  for(int i=0; i<nPoints ; ++i) {
    int run = atoi(histo->GetXaxis()->GetBinLabel(i+1));
    if( (it=aMap.find(run))!=aMap.end() && (ite=aMapErr.find(run))!=aMapErr.end()) {    
      histo->SetBinContent(i+1,it->second);
      histo->SetBinError(i+1,ite->second);
    }
  }
  histo->LabelsOption("a","X"); // sort alphabetically
  return histo;
}

//////////////////////////////////////////////////////
//
// Creates histogram with pressure vs run number (text labels).
// Optionally only run numbers listed in runList will be
// taken into account.
//
//////////////////////////////////////////////////////
TGraphErrors* getPressureVsRunGraph(const char* pressureGraphFile="PressureGraph.root") {

  //Load pressure graph, and transfer into map
  string name = gSystem->pwd();
  name+="/";
  name+=pressureGraphFile;
  TFile *file = new TFile(name.c_str()); // "./PressureGraph.root");
  if(!file) {
    //    std::cout<<"ERROR: Can't open file: "<<pressureGraphFile<<std::endl;
    std::cout<<"ERROR: Can't open file: "<<name<<std::endl;
    return NULL;
  }
  file->cd();
  file->ls();
  TGraph *graph = (TGraph*)file->Get("Graph");
  if(!graph) {
    std::cout<<"ERROR: Can't find TGraph in file: "<<pressureGraphFile<<std::endl;
    return NULL;
  }

  //TGraph *gr = graph->Clone("gPressVsRun");
  std::map<int,float> aMap;  
  float pressMax = -9999;
  float pressMin = 9999;
  for(int i=0;i<graph->GetN();++i){
    Double_t xTmp, yTmp;
    graph->GetPoint(i,xTmp,yTmp);
    if(yTmp<=100) continue;
    if(yTmp>pressMax) pressMax = yTmp;
    if(yTmp<pressMin && yTmp>100) pressMin = yTmp;
    aMap[xTmp] = yTmp;
  }
  //Put values into TGraph
  Double_t x[10000],y[10000];
  std::map<int,float>::iterator ci = aMap.begin();
  for(int i=0;ci!=aMap.end();++ci){
    x[i] = ci->first;
    y[i] = ci->second;
    ++i;
  }
  if(!aMap.size()) return NULL;
  TGraphErrors *gr = new TGraphErrors(aMap.size(),x,y,0,0);
  gr->SetName("gPressVsRun");
  gr->SetTitle("Pressure vs Run;run;pressure [mbar]");
  // DEBUG
  //std::cout<<"### getPressureVsRunGraph: file="<<pressureGraphFile<<std::endl;
  //for(int i=1;i<gr->GetN();++i) {
  //  Double_t xTmp, yTmp;
  //  gr->GetPoint(i,xTmp,yTmp);
  //  std::cout<<"### getPressureVsRunGraph: run="<<xTmp<<" p="<<yTmp<<std::endl;
  //}
  // DEBUG
  return gr;
}
//////////////////////////////////////////////////////
//
// Creates histogram with pressure vs run number (text labels).
// Optionally only run numbers listed in runList will be
// taken into account.
//
//////////////////////////////////////////////////////
TH1F *getHistoFromPressureVsRunGraph(TGraph *graph, 
				     std::vector<int> runList=std::vector<int>()) {

  if(!graph) return NULL;
  const float pressure_err=0.1; // accuracy 0.1 mbar
  std::map<int,float> aMap;
  float pressMax = -9999;
  float pressMin = 9999;
  for(int i=0;i<graph->GetN();++i){
    Double_t xTmp, yTmp;
    graph->GetPoint(i,xTmp,yTmp);
    aMap[xTmp] = yTmp;
    if(yTmp>pressMax) pressMax = yTmp;
    if(yTmp<pressMin && yTmp>100) pressMin = yTmp;
  }
  // create histogram
  TH1F *h;
  int nPoints;
  if(runList.size()) {
    nPoints = runList.size();
    h = (TH1F*) new TH1F("hPressVsRun", "Pressure vs run;run;pressure [mbar]", nPoints, -0.5, nPoints-0.5);
    std::vector<int>::iterator it=runList.begin();
    for(int i=1;it!=runList.end();++it,++i) h->GetXaxis()->SetBinLabel(i,Form("%d",*it));
  } else {
    nPoints = aMap.size();
    h = (TH1F*) new TH1F("hPressVsRun", "Pressure vs run;run;pressure [mbar]", nPoints, -0.5, nPoints-0.5);
    std::map<int,float>::iterator it=aMap.begin();
    for(int i=1;it!=aMap.end();++it,++i) h->GetXaxis()->SetBinLabel(i,Form("%d",it->first));
  }
  // fill histogram
  for(int i=1;i<=nPoints;++i){
    int run = atoi(h->GetXaxis()->GetBinLabel(i));
    float val = 0.0;
    if(aMap.find(run)!=aMap.end()) val = aMap[run];
    h->SetBinContent(i,val);
    h->SetBinError(i,pressure_err);
  }
  h->SetBinContent(0,pressMin);       
  h->SetBinContent(h->GetNbinsX()+1,pressMax);  
  h->SetLineColor(2);
  h->SetMarkerColor(2);
  h->SetMarkerStyle(21);
  h->LabelsOption("a","X"); // sort alphabetically
  return h;
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void printRunsOnPlot(TH1F* h) {

  string runs[6] = {"160406 key LHC6_1", 
		    "165537 key LHC7",
		    "165970 key LHC7_1EX",
		    "169985 Jun TS end", 
		    "175832 Aug TS end",
		    "XXXXX"};
  const int nRuns=sizeof(runs)/sizeof(string);
  // XXXXX will be replaced by the last run number from PressureGraph.root

  if (nRuns<1) return;
  runs[nRuns-1] = h->GetXaxis()->GetBinLabel(h->GetNbinsX());

  float x,y,yMin,dy;
  yMin=h->GetMinimum();
  dy=h->GetMaximum()-h->GetMinimum();
  y=yMin+0.5*dy;
  TLatex *aLatex = new TLatex();
  TArrow *aArrow = new TArrow();
  aArrow->SetArrowSize(0.01);
  aArrow->SetFillColor(kBlue);
  aArrow->SetFillStyle(1001);
  aLatex->SetTextColor(kBlue);
  aArrow->SetLineColor(kBlue);
  aArrow->SetLineWidth(3);

  for(int i=0;i<nRuns;++i){
    for(int iBin=1;iBin<=h->GetNbinsX();++iBin){
      string runNumber = (runs[i]).substr(0,6);
      string label(h->GetXaxis()->GetBinLabel(iBin));
      x = h->GetBinCenter(iBin);
      if(runNumber==label){ // if(runs[i]==label){
	if((float)(x-h->GetBinCenter(1))/
	   (float)(h->GetBinCenter(h->GetNbinsX())-h->GetBinCenter(1))<0.94) {
	  // avoid overlaying Y axis labels
	  aLatex->SetTextAlign(13); // default, bottom
	  aLatex->SetTextAngle(90-30);
	} else {
	  aLatex->SetTextAlign(11); // top
	  aLatex->SetTextAngle(90+0);
	}
	aLatex->DrawLatex(x,y,(runs[i]).c_str());
	aArrow->DrawArrow(x,y-dy*0.01,x,yMin);
      }
    }
  }
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
bool getMeanRmsFromHisto(TH1* h, 
			 Double_t &mean, Double_t &rms,
			 bool onlyPositiveValues=false) {
  if(h==NULL) { 
    std::cout<<"ERROR: Histogram is NULL!!!"<<std::endl;
    return false;
  }
  if(h->GetNbinsX()<1) { 
    std::cout<<"ERROR: Hisgoram has 0 bins !!!"<<std::endl;
    return false;
  }
  Double_t sum=0.0;
  Double_t sumw=0.0;
  for(int i=1;i<=h->GetNbinsX();++i) {
    Double_t val = h->GetBinContent(i);
    Double_t err = h->GetBinError(i);
    if(onlyPositiveValues && val<=0.0) continue; // skip empty points
    if(err<=0.0) continue; // skip empty points
    sum  += val/err/err;
    sumw += 1.0/err/err;
  }
  mean=sum/sumw;
  Double_t sum2=0.0;
  Double_t sumw2=0.0;
  for(int i=1;i<=h->GetNbinsX();++i) {
    Double_t val = h->GetBinContent(i);
    Double_t err = h->GetBinError(i);
    if(onlyPositiveValues && val<=0.0) continue; // skip empty points
    if(err<=0.0) continue; // skip empty points
    sum2  += (val-mean)*(val-mean)/err/err;
    sumw2 += 1.0/err/err/err/err;
  }
  rms=sqrt( sumw/(sumw*sumw-sumw2)*sum2 ); // standard deviation for weighted sample
  return true;
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
bool compareXlabels(TH1* h1, TH1 *h2) {
  if(h1==NULL) { 
    std::cout<<"ERROR: 1st histogram is NULL!!!"<<std::endl;
    return false;
  }
  if(h2==NULL) { 
    std::cout<<"ERROR: 2nd histogram is NULL!!!"<<std::endl;
    return false;
  }
  if(h1->GetNbinsX()!=h2->GetNbinsX()) { 
    std::cout<<"ERROR: Different number of X bins: "
	     <<h1->GetNbinsX()<<" "<<h2->GetNbinsX()<<std::endl;
    return false;
  }
  if(h1->GetXaxis()->GetXmin()!=h2->GetXaxis()->GetXmin()) {
    std::cout<<"ERROR: Different X MIN values: "
	     <<h1->GetXaxis()->GetXmin()<<" "<<h2->GetXaxis()->GetXmin()<<std::endl;
    return false;
  }
  if(h1->GetXaxis()->GetXmax()!=h2->GetXaxis()->GetXmax()) {
    std::cout<<"ERROR: Different X MAX values: "
	     <<h1->GetXaxis()->GetXmax()<<" "<<h2->GetXaxis()->GetXmax()<<std::endl;
    return false;
  }
  for(int i=1; i<=h1->GetNbinsX(); i++) {
    if((string)h1->GetXaxis()->GetBinLabel(i)!=(string)h2->GetXaxis()->GetBinLabel(i)) {
      std::cout<<"ERROR: Different X LABELS at bin "<<i
	       <<": "<<h1->GetXaxis()->GetBinLabel(i)
	       <<" "<<h2->GetXaxis()->GetBinLabel(i)<<std::endl;
      return false;
    }
  }
  std::cout<<"compareXlabels: X labels are identical for: "
	   <<h1->GetName()<<" "<<h2->GetName()<<std::endl;
  return true; // comparison OK
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void plotSCRateVsRunCollisionsByRegion(int ptCode=13, 
				       string region="B",       // "B" / "E"
				       TFile *outFile=NULL,
				       float rateMin=9999,      // 0.1 / 0.15
				       float rateMax=-9999,     // 0.15 / 0.4
				       string pressureGraphFile="PressureGraph.root",
				       float pressMin=9999,
				       float pressMax=-9999){

  int nMu=1;  // first muon
  string title;
  string regionName="";
  if(region=="B") regionName="Barrel";
  if(region=="E") regionName="Endcap";
  if(region!="B" && region!="E") {
    std::cout<<"ERROR: Wrong region: "<<region<<std::endl;
    return;
  }

  char text[500];
  sprintf(text,"Rate for p_{T} code: %d, region: %s",ptCode,regionName.c_str());
  title=text;

  TGraphErrors *grPress = getPressureVsRunGraph(pressureGraphFile.c_str());
  std::vector<int> runList = getRunListFromGraph(grPress);
  TH1F   *hPress = getHistoFromPressureVsRunGraph(grPress, runList);
  if (hPress==NULL || runList.size()==0) {
    std::cout<<"WARNING: Missing Pressure vs Run data!!!"<<std::endl;
  } else {
    if(pressMin<pressMax && pressMin>0) {
      hPress->SetMinimum(pressMin);
      hPress->SetMaximum(pressMax);
    } else {
      // adjust range automatically
      Double_t mean, rms;
      if(getMeanRmsFromHisto(hPress, mean, rms, true)) {
	std::cout<<">>>>> hPress meanY="<<mean<<" rmsY="<<rms<<std::endl;
	hPress->SetMinimum(mean-3.0*rms); // default: 3*sigma
	hPress->SetMaximum(mean+3.0*rms); // default: 3*sigma
      }
    }
  }

  TPad *pad1, *pad2;
  TCanvas *c2 = new TCanvas(Form("Rate_%dmu_PtCode%d_%s_Collisions",nMu,ptCode,regionName.c_str()),
			    Form("Normalized SC rate vs run : %d mu, p_{T} code %d, %s",
				 nMu, ptCode, regionName.c_str()),12,33,1576,500);
  c2->SetLeftMargin(0.06);
  c2->SetRightMargin(0.06);
  c2->Draw();
  c2->cd();
  pad1 = new TPad("pad1","",0,0,1,1); // rate histogram
  pad2 = new TPad("pad2","",0,0,1,1); // pressure histogram
  pad2->SetFillStyle(4000);           // will be transparent
  pad1->Draw();
  pad1->cd();
  gStyle->SetOptStat(kFALSE);

  TGraphErrors *grRate = getNormalisedRateVsRunGraph(nMu,region.c_str(),ptCode);
  TH1F *hRate = (runList.size() ? 
		     getHistoFromNormalisedRateVsRunGraph(grRate,runList) :
		     getHistoFromNormalisedRateVsRunGraph(grRate));
  // just for x-check
  if(hPress && !compareXlabels(hPress,hRate)) {
    std::cout<<"ERROR: Something wrong with the X bin labels!!!"<<std::endl;
    return;
  }
  hRate->SetTitle(title.c_str());
  if(rateMin<rateMax && rateMin>=0.0) {
    hRate->SetMinimum(rateMin);
    hRate->SetMaximum(rateMax);
  } else {
    //adjust range automatically
    Double_t mean, rms;
    if(getMeanRmsFromHisto(hRate, mean, rms, true)) {
      std::cout<<">>>>> hRate meanY="<<mean<<" rmsY="<<rms<<std::endl;
      hRate->SetMinimum(mean-2.0*rms); // default: 2.5*sigma
      hRate->SetMaximum(mean+2.0*rms); // default: 2.5*sigma
    }
  }
  hRate->SetName(Form("histo%s",regionName.c_str()));
  hRate->SetStats(kFALSE);  
  hRate->GetYaxis()->SetTitleOffset(0.85);

  //////////////////////////////////////////// DEBUG
  //for(int i=1;i<=hRate->GetNbinsX();++i) {
  //  std::cout<<">>>>> hRate bin="<<i
  //	     <<" label="<<hRate->GetXaxis()->GetBinLabel(i)
  //	     <<" val="<<hRate->GetBinContent(i)
  //	     <<" err="<<hRate->GetBinError(i)<<std::endl;
  //}
  //////////////////////////////////////////// DEBUG

  hRate->Draw("E1,P"); 
  printRunsOnPlot(hRate); //,region);
  c2->cd();

  ////////Pressure
  if(hPress) {
    //compute the pad range with suitable margins
    Double_t ymin = hPress->GetMinimum();
    Double_t ymax = hPress->GetMaximum();
    Double_t dy = (ymax-ymin)/(1.0-c2->GetTopMargin()-c2->GetBottomMargin());
    Double_t xmin = hPress->GetXaxis()->GetXmin();
    Double_t xmax = hPress->GetXaxis()->GetXmax();
    Double_t dx = (xmax-xmin)/(1.0-c2->GetLeftMargin()-c2->GetRightMargin());
    std::cout<<"TPAD RANGE xmin="<<xmin
	     <<" xmax="<<xmax
	     <<" ymin="<<ymin
	     <<" ymax="<<ymax
	     <<" dx="<<dx<<" dy="<<dy<<std::endl;
    std::cout<<"(xmax-xmin)="<<(xmax-xmin)<<" L="<<pad1->GetLeftMargin()
    	     <<" R="<<pad1->GetRightMargin()<<std::endl;
    pad2->Range(xmin-c2->GetLeftMargin()*dx,
    		ymin-c2->GetBottomMargin()*dy,
    		xmax+c2->GetRightMargin()*dx,
    		ymax+c2->GetTopMargin()*dy);
    pad2->Draw();
    pad2->cd();
    hPress->SetLineColor(kRed);
    hPress->DrawCopy("][sames,P");
   
    // draw axis on the right side of the pad
    TGaxis *axis = new TGaxis(xmax,ymin,xmax,ymax,ymin,ymax,510,"+L");
    axis->SetLineColor(kRed);
    axis->SetLabelColor(kRed);
    axis->SetTitleColor(kRed);
    axis->SetTitle("Pressure [mbar]");
    axis->SetTitleOffset(0.8);
    axis->Draw();
    
    printRunsOnPlot(hPress); //,region);
    pad2->Update();
  }
  c2->Modified();
  c2->Update();
  c2->Print(Form("eps/Rate_PtCode%d_%s_Collisions.eps",ptCode,regionName.c_str()));
  c2->Print(Form("png/Rate_PtCode%d_%s_Collisions.png",ptCode,regionName.c_str()));
  c2->Print(Form("C/Rate_PtCode%d_%s_Collisions.C",ptCode,regionName.c_str()));
  if(outFile) {
    outFile->cd();
    c2->Write();
  }
  //c2->Print(Form("root/Rate_PtCode%d_%s_Collisions.root",ptCode,regionName.c_str()));
  delete pad1;
  delete pad2;
  delete c2;
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

void plotSCRateVsRunCollisions() {

  // create output plot directories
  gSystem->mkdir("./C");
  gSystem->mkdir("./eps");
  gSystem->mkdir("./png");
  gSystem->mkdir("./root");

  std::vector<int> ptCode;
  ptCode.push_back(5);  
  ptCode.push_back(10);  
  ptCode.push_back(13);
  ptCode.push_back(15);
  ptCode.push_back(20);
  ptCode.push_back(25);

  TFile *outFile = new TFile("./root/SCrateVsRunPlots.root","RECREATE");
  for(int icode=0; icode<ptCode.size(); icode++) {
    std::cout<<"Doing plots for PT code: "<<ptCode[icode]<<std::endl;
    // BARREL
    plotSCRateVsRunCollisionsByRegion(ptCode[icode], "B", outFile); // , 0.1, 0.15,
    //				    "PressureGraph.root", 945.0, 985.0 );
    // ENDCAP 
    plotSCRateVsRunCollisionsByRegion(ptCode[icode], "E", outFile); //, 0.15, 0.4,
    //				    "PressureGraph.root", 945.0, 985.0 );
  }
  outFile->Close();
}
