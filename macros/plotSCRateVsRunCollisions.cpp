#include <vector>
#include <map>
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
   Int_t sum=0.;
   Int_t sume2=0.;
   ////////////////////
   for (i = histoD->GetNbinsX()+1; i >= 0; i--) {
        sum+=cont[i];
        sume2+=errs[i]*errs[i];
	///////////////////////////
        histoI->SetBinContent(i,sum);
        histoI->SetBinError(i,sqrt(sume2));
   }   
// for (i = 0; i<=histoI->GetNbinsX()+1;i++){
//      cout <<"bin: "<<i<<" cont: "<<histoI->GetBinContent(i);
//      cout <<            " error: "<<histoI->GetBinError(i)<<endl;
// }

   return histoI;
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
TH1F* getHistoForRun(TFile *file, int nMu, string region="B"){

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
  TCanvas *aCanvas = (TCanvas *)file->Get("SC_fsHisits");
  if(!aCanvas){
    std::cout<<"No canvas in file: "<<file->GetName()<<std::endl;
    return 0;
  }
  TPad *aPad = (TPad*)(SC_fsHisits->GetPrimitive(padName)); 
  TH1I *aHist = (TH1I*)(aPad->GetPrimitive(hName));
  TH1I *hInt =  Integrate(aHist);
  sprintf(hName,"FS_HIST_CODE_%s%d_ptCode_Int_%s",region.c_str(),nMu-1,runNumber.c_str());
  hInt->SetName(hName);
  TH1F *hIntFloat = new TH1F("hIntFloat",hInt->GetTitle(),
			     hInt->GetNbinsX(),
			     hInt->GetXaxis()->GetXmin(),hInt->GetXaxis()->GetXmax());
  hIntFloat->SetXTitle(hInt->GetXaxis()->GetTitle());
  hIntFloat->SetYTitle("Normalised rate");
  sprintf(hName,"FS_HIST_CODE_%s%d_ptCode_IntFloat_%s",region.c_str(),nMu-1,runNumber.c_str());
  hIntFloat->SetName(hName);
  ///Rewrite remormalised content of TH1I 
  ///Put total rate into underflow bin
  hIntFloat->SetBinContent(0,hInt->GetBinContent(1));
  ///Put run number into overflow bin
  hIntFloat->SetBinContent(hInt->GetNbinsX()+1,atoi(runNumber.c_str()));
  ///Put normalised rate
  for(int iBin=1;iBin<=hInt->GetNbinsX();++iBin){
    hIntFloat->SetBinContent(iBin,hInt->GetBinContent(iBin)/hInt->GetBinContent(1));
    hIntFloat->SetBinError(iBin,hInt->GetBinError(iBin)/hInt->GetBinContent(1));
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
std::vector<TH1F*> getHistoVecForRuns(int nMu, string region){

  string path = "./" ; // "/home/akalinow/scratch/CMS/PAC/MonitorHistos/";
  vector<TH1F*> histos;
  TList *files = getListOfFiles(path);
  TIter next(files);
  TObject *obj = 0;
  int counter = 0;
  while (obj = next()){
    string fileName(obj->GetName());
    if(fileName.find(".root")==std::string::npos) continue;
    string runNumber = fileName.substr(fileName.find("run")+4,6);
    if(runNumber.find("_")!=std::string::npos) continue;
    //if(runNumber.find("167163")==std::string::npos) continue;
    //std::cout<<fileName<<" "<<runNumber<<std::endl;
    TFile *file = new TFile((path+fileName).c_str());
    TH1F *h = getHistoForRun(file,nMu,region);
    if(h==0) continue;
    h->SetDirectory(0);
    histos.push_back(h);
    delete file;
    counter++;
    //if(counter>400) break;
  }
  delete files;

  //TSystemDirectory aDir1("Directory1","/home/akalinow/scratch/CMS/PAC/soft/");
  TSystemDirectory aDir1("Directory1","./");
  aDir1.GetListOfFiles();

  return histos;
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void plotVsRun(int nMu, string region){

  vector<TH1F*> histos =  getHistoVecForRuns(nMu,region);

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
      histos[i]->Draw("");
      isFrame=true;
    }
    else histos[i]->Draw("same");
  }
  
  char name[100];
  TLatex *aLatex = new TLatex(0,0,"");
  if(region=="B") aLatex->DrawLatex(18,0.9,"Barrel");
  if(region=="E") aLatex->DrawLatex(18,0.9,"Endcap");
  sprintf(name,"Runs: %d - %d",minRun,maxRun);
  aLatex->DrawLatex(18,0.85,name);
  sprintf(name,"eps/RateVsRun_%d_%sCollisions.eps",nMu,region.c_str());
  c1->Print(name);
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
TGraph* plotAtPointVsRun(int nMu, string region,int ptCode=13){

  vector<TH1F*> histos =  getHistoVecForRuns(nMu,region);
  std::map<int,float> valMap;
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
  ////////////////////////////////////////////////

  for(int i=0;i<histos.size();++i){
    float val = histos[i]->GetBinContent(histos[i]->FindBin(ptCode));
    //if(region=="B" && (val<0.05 || val>0.2)) continue;
    //if(region=="E" && val<0.05) continue;
    float run = histos[i]->GetBinContent(histos[i]->GetNbinsX()+1);
    if(aMap.find(run)==aMap.end()) continue;
    if(histos[i]->GetBinContent(0)<1e5) continue;
    if(run<100000) continue;
    //std::cout<<"run: "<<run<<std::endl;
    if(valMap.find(run)!=valMap.end()){
      std::cout<<"Run: "<<run<<" already analysed!"<<std::endl;
    }
    valMap[run] = val;
  }

  ///Put values into TGraph
  Double_t x[10000], y[10000];
  std::map<int,float>::const_iterator ci = valMap.begin();
  int i = 0;
  for(;ci!=valMap.end();++ci){
    x[i] = ci->first;
    y[i] = ci->second;
    ++i;
  }

  TGraph *gr = new TGraph(valMap.size(),x,y);
  return gr;
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
TH1F * getEffVsRunHisto(TGraph *hGraph){

  int nPoints = hGraph->GetN();
  TH1F *histo = new TH1F("histo","",nPoints,-0.5,nPoints-0.5);
  histo->SetMaximum(1.);
  histo->SetMinimum(0.0); 
  histo->SetYTitle("L1 RPC normalised rate");
  histo->SetLineWidth(2);


  Double_t xTmp, yTmp;
  Double_t error = 0;
  for(int i=0;i<nPoints;++i){
    hGraph->GetPoint(i,xTmp,yTmp);
    error = hGraph->GetErrorY(i);
    histo->SetBinContent(i+1,yTmp);
    histo->SetBinError(i+1,error);
    histo->GetXaxis()->SetBinLabel(i+1,Form("%d",int(xTmp)));
  }

  return histo;
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
TH1F *getHistoFromGraph(TH1F *histo){

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
  //max*=1.05;
  //min*=0.95;
  /////
  std::cout<<max<<" "<<min<<" "<<axisMin<<" "<<axisMax<<std::endl;
  TH1F *h = (TH1F*)histo->Clone("h");
  h->Reset();
  for(int i=1;i<=h->GetNbinsX();++i){
    int run = atoi(h->GetXaxis()->GetBinLabel(i));
    float val = 0.0;
    if(aMap.find(run)!=aMap.end()) val = aMap[run];
    float valNormalised =  (axisMax - axisMin)/(max - min)*(val - min) + axisMin;
    /*
    valNormalised = -valNormalised +
      (axisMax - axisMin)/(max - min)*(max - min) + axisMin +
      (axisMax - axisMin)/(max - min)*(min - min) + axisMin;
    */
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
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void printRunsOnPlot(TH1F* h, string region="B"){

  string runs[7] = {"160406", "163296","163340","163758","166565", "169985", "XXXXX"};
  const int nRuns=sizeof(runs)/sizeof(string);
  // XXXXX will be replaced by the last run number from PressureGraph.root

  if (nRuns<1) return;
  //runs[5] = h->GetXaxis()->GetBinLabel(h->GetNbinsX());
  runs[nRuns-1] = h->GetXaxis()->GetBinLabel(h->GetNbinsX());

  float x = 0;
  float y = 0.135;
  float yMin = 0.1;
  if(region=="E"){
    y = 0.35;
    yMin = 0.15;
  }
  TLatex *aLatex = new TLatex(x,y,"");
  TArrow *aArrow = new TArrow(0,0,1,1);
  aArrow->SetArrowSize(0.01);
  aLatex->SetTextColor(kBlue);
  aArrow->SetLineColor(kBlue);

  for(int i=0;i<nRuns;++i){
    for(int iBin=1;iBin<=h->GetNbinsX();++iBin){
      string label(h->GetXaxis()->GetBinLabel(iBin));
      x = h->GetBinCenter(iBin);
      if(runs[i]==label){
	if((float)(x-h->GetBinCenter(1))/
	   (float)(h->GetBinCenter(h->GetNbinsX())-h->GetBinCenter(1))<0.06) {
	  // avoid overlaying Y axis labels
	  aLatex->SetTextAlign(13); // default, bottom
	  aLatex->SetTextAngle(90-10);
	} else {
	  aLatex->SetTextAlign(11); // top
	  aLatex->SetTextAngle(90+10);
	}
	aLatex->DrawLatex(x,y,label.c_str());
	//aLatex->DrawLatex(x+0.06,y,label.c_str());
	aArrow->DrawArrow(x,y-0.001,x,yMin);
      }
    }
  }
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void plotSCRateVsRunCollisions(){

  int ptCode = 13;

  TCanvas *c2 = new TCanvas("c2","Rates for runs",12,33,1576,500);
  c2->SetLeftMargin(0.06);
  c2->SetRightMargin(0.04);
  c2->Draw();

  TLatex *aLatex = new TLatex(0,0,"");
  char text[500];
  sprintf(text,"Rate at pt code %d",ptCode);
  ////Barrel, first muon
  int nMu=1;
  string region = "B";  
  TGraph *grBarr = plotAtPointVsRun(nMu,region,ptCode);
  TH1F *histoBarr = getEffVsRunHisto(grBarr);
  histoBarr->SetMinimum(0.1);
  histoBarr->SetMaximum(0.15);
  histoBarr->SetName("histoBarr");
  histoBarr->SetStats(kFALSE);  
  histoBarr->GetYaxis()->SetTitleOffset(0.6);
  string title;
  title.append(text);
  title+=" Region: barrel";
  histoBarr->SetTitle(title.c_str());
  c2->cd();
  histoBarr->Draw("hist"); 
  printRunsOnPlot(histoBarr,"B");
  /////////Pressure
  TH1F *hPress = getHistoFromGraph(histoBarr);
  hPress->SetLineColor(2);
  hPress->SetLineStyle(2);
  hPress->Draw("same P"); 
  TGaxis *axis = new TGaxis(hPress->GetXaxis()->GetXmax(),histoBarr->GetMinimum(),
			    hPress->GetXaxis()->GetXmax(),histoBarr->GetMaximum(),
			    hPress->GetBinContent(0),    			    
			    hPress->GetBinContent(hPress->GetNbinsX()+1),
  			    510,"+L");
  axis->SetLineColor(kRed);
  axis->SetLabelColor(kRed);
  axis->SetTitleColor(kRed);
  axis->SetTitle("Pressure [mb]");
  axis->SetTitleOffset(0.55);
  axis->Draw();
  ////////////////////////
  c2->Print("eps/RateAtPointBarrelCollisions.eps");
  c2->Print("png/RateAtPointBarrelCollisions.png");
  /////////////
  /////////////
  ////Endcap, first muon  
  region = "E";
  TGraph *grEndcap = plotAtPointVsRun(nMu,region,ptCode);
  TH1F *histoEndcap = getEffVsRunHisto(grEndcap);
  histoEndcap->SetMinimum(0.15);
  histoEndcap->SetMaximum(0.4);
  histoEndcap->SetName("histoEndcap");
  histoEndcap->SetStats(kFALSE);
  histoEndcap->GetYaxis()->SetTitleOffset(0.6);
  string title;
  title.append(text);
  title+=" Region: endcap";
  histoEndcap->SetTitle(title.c_str());
  c2->cd();
  histoEndcap->Draw("hist");
  printRunsOnPlot(histoEndcap,"E");
 /////////Pressure
  TH1F *hPress = getHistoFromGraph(histoEndcap);
  hPress->SetLineColor(2);
  hPress->SetLineStyle(2);
  hPress->Draw("same P"); 
  TGaxis *axis = new TGaxis(hPress->GetXaxis()->GetXmax(),hPress->GetMinimum(),
			    hPress->GetXaxis()->GetXmax(),hPress->GetMaximum(),
			    hPress->GetBinContent(0),
			    hPress->GetBinContent(hPress->GetNbinsX()+1),
			    510,"+L");
  axis->SetLineColor(kRed);
  axis->SetLabelColor(kRed);
  axis->SetTitleColor(kRed);
  axis->SetTitle("Pressure [mb]");
  axis->SetTitleOffset(0.55);
  axis->Draw();
  ////////////////////////
  c2->Print("eps/RateAtPointEndcapCollisions.eps");
  c2->Print("png/RateAtPointEndcapCollisions.png");
  ////  
  //plotVsRun(nMu,"E");
  ///
  //plotVsRun(nMu,"B");

}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
