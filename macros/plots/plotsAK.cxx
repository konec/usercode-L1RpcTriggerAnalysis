#include "utilsDivideErr.C"
#include "utilsL1RpcStyle.C"

unsigned int nPtBins = 32;

double ptBins[33]={0., 0.1, 
		 1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 6., 7., 8., 
		 10., 12., 14., 16., 18., 20., 25., 30., 35., 40., 45., 
		 50., 60., 70., 80., 90., 100., 120., 140., 
		 160. };

const int color[6] = {kBlack, kRed, kGreen, kBlue, kMagenta, kTeal};
//const int ptCuts[6] = {1, 5, 10, 16, 30, 100};

const int ptCutsGmt[4] = {0.1, 16, 20, 30};
const int ptCutsOtf[4] = {0.1, 14, 18, 25};

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
#include <cmath>
double vxmurateParam(double x){


  TF1 *fIntVxMuRate = new TF1("fIntVxMuRate","TMath::Power(x,[0]*TMath::Log(x))*TMath::Power(x,[1])*TMath::Exp([2])",1,100);
  fIntVxMuRate->SetParameters(-0.235801, -2.82346, 17.162);
  fIntVxMuRate->SetLineColor(3);

  return -fIntVxMuRate->Derivative(x);
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
TH1F* Integrate(TH1F * histoD) {

  TH1F* histoI = (TH1F*)histoD->Clone("hIntegrated");

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
///////////////////////////////////////////////////////
void makeFriendTree(TTree *tree){

  TF1 *fIntVxMuRate = new TF1("fIntVxMuRate","TMath::Power(x,[0]*TMath::Log(x))*TMath::Power(x,[1])*TMath::Exp([2])",1,1000);
  fIntVxMuRate->SetParameters(-0.235801, -2.82346, 17.162);

  Float_t pt, eta;
  TBranch *ptBranch, *etaBranch;

  tree->SetMakeClass(1);

  tree->SetBranchAddress("pt", &pt, &ptBranch);
  tree->SetBranchAddress("eta", &eta, &etaBranch);

  tree->SetBranchStatus("*",0);
  tree->SetBranchStatus("pt",1); 
  tree->SetBranchStatus("eta",1); 

  std::string fileName = "Weights.root";
  std::string treeName = "Weights";

  TFile *file = new TFile(fileName.c_str(),"RECREATE");
  TTree *weightsTree = new TTree(treeName.c_str(),"");  
  Double_t weight;
  TBranch *weightBranch = weightsTree->Branch("weight", &weight, "weight/D");  

  TH1F *hPtOrig = new TH1F("hPtOrig","",400,0,200);
  tree->Draw("pt>>hPtOrig","eta<1.19","goff");

  int iBin, nEvInBin;
  float ptLow, ptHigh;
  Long64_t nentries = tree->GetEntries();
  for (Long64_t ii = 0; ii < nentries; ii++){
    tree->GetEntry(ii);
    iBin = hPtOrig->FindBin(pt);
    ptLow = hPtOrig->GetXaxis()->GetBinLowEdge(iBin);
    ptHigh = hPtOrig->GetXaxis()->GetBinUpEdge(iBin);
    nEvInBin = hPtOrig->GetBinContent(iBin);
    if(nEvInBin<1) nEvInBin = 1;
    weight = (fIntVxMuRate->Eval(ptLow) - fIntVxMuRate->Eval(ptHigh))/nEvInBin;
    weightsTree->Fill();
  }

  file->Write();
  file->Close();
  tree->SetBranchStatus("*",1);
}
///////////////////////////////////////////////////////
// TH2F *h2D,           //X axis of this histogram defines binning of output histogram
// string var="pt",     //efficiency will be plotted vs this variable name (name of the branch)
// string sysType="Otf",//type of system: Otf or Gmt
// string sel = "1"     //additional event selection
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
TH1D *getTurnOncurve(TTree *tree, float ptCut, 
		     TH2F *h2D,
		     string var="pt",     
		     string sysType="Otf",
		     string sel = "1"){

  ///Add events with at least one recontructed L1Mu
  if(sysType=="Gmt" || sysType=="Rpc"){
  tree->Draw(TString::Format("(l1Objects%s[0].pt>=%3.2f):%s>>h2D",
			     sysType.c_str(),ptCut,var.c_str()).Data(),
	     TString::Format("@l1Objects%s.size()>0 && %s",sysType.c_str(),sel.c_str()).Data(),"goff");
  }
  if(sysType=="Otf"){
    tree->Draw(TString::Format("(l1Objects%s[0].pt>=%3.2f && l1ObjectsOtf.q!=103 && l1ObjectsOtf.q!=104 && l1ObjectsOtf.q!=105 && l1ObjectsOtf.q%100>3 && l1ObjectsOtf.q>650):%s>>h2D",
	       //TString::Format("(l1Objects%s[0].pt>=%3.2f && l1ObjectsOtf.q%100>3 && l1ObjectsOtf.q>250):%s>>h2D",
			       //TString::Format("(l1Objects%s[0].pt>=%3.2f && l1ObjectsOtf.q%100>4 &&  l1ObjectsOtf.q/100==8):%s>>h2D",
			       sysType.c_str(),ptCut,var.c_str()).Data(),
	       TString::Format("@l1Objects%s.size()>0 && %s",sysType.c_str(),sel.c_str()).Data(),"goff");
  }
  ///Add events without recontructed L1Mu  
  tree->Draw(TString::Format("0:%s>>+h2D",var.c_str()).Data(),
	     TString::Format("@l1Objects%s.size()==0 && %s",sysType.c_str(), sel.c_str()).Data(),
	     "goff");
  
  TH1D *hNum = h2D->ProjectionX("hNum",2,2);
  TH1D *hDenom = h2D->ProjectionX("hDenom",1,1);
  hDenom->Add(hNum);
  TH1D* hEff =DivideErr(hNum,hDenom,"Pt_Int","B");

  return hEff;
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotEffPanel(TTree *tree,
		  string sysType="Otf"){

  TCanvas* c = new TCanvas(TString::Format("EffVsPt_%s",sysType.c_str()),
			   TString::Format("EffVsPt_%s",sysType.c_str()),			    
			   460,500);
  
  TLegend l(0.6513158,0.1673729,0.8903509,0.470339,NULL,"brNDC");
  l.SetTextSize(0.05);
  l.SetFillStyle(4000);
  l.SetBorderSize(0);
  l.SetFillColor(10);
  c->SetLogx(1);  
  c->SetGrid(0,1);
 
  TH2F *h2D = new TH2F("h2D","",nPtBins,ptBins,2,-0.5,1.5);

  int *ptCuts = ptCutsOtf;
  if(sysType=="Gmt") ptCuts = ptCutsGmt;

  for (int icut=0; icut <=3;++icut){
    string selection(TString::Format("eta<1.19",sysType.c_str()).Data());
    TH1D* hEff = getTurnOncurve(tree,ptCuts[icut],h2D,"pt",sysType,selection);    
    hEff->SetStats(kFALSE);
    hEff->SetMinimum(0.0001); 
    hEff->SetMaximum(1.04);
    hEff->GetXaxis()->SetRange(4,100);
    hEff->SetMarkerStyle(21+icut);
    hEff->SetMarkerColor(color[icut]);
    hEff->SetXTitle("muon p_{T} [GeV/c]");
    hEff->SetYTitle("Efficiency");
    if (icut==0)hEff->DrawCopy();
    else hEff->DrawCopy("same");
    std::string nameCut = TString::Format("%d",ptCuts[icut])+" GeV/c"; 
    if (icut==0) nameCut = "no p_{T} cut";
    l.AddEntry(hEff,nameCut.c_str());
  }
  l.DrawClone();
  c->Print(TString::Format("fig_eps/PanelVsPt_%s.eps",sysType.c_str()).Data());
  c->Print(TString::Format("fig_png/PanelVsPt_%s.png",sysType.c_str()).Data());
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotEffVsPhi(TTree *tree,
		  string sysType="Otf"){

  TCanvas* c = new TCanvas(TString::Format("EffVsPhi_%s",sysType.c_str()),
			   TString::Format("EffVsPhi_%s",sysType.c_str()),			    
			   460,500);
  
  TLegend l(0.6513158,0.1673729,0.8903509,0.470339,NULL,"brNDC");
  l.SetTextSize(0.05);
  l.SetFillStyle(4000);
  l.SetBorderSize(0);
  l.SetFillColor(10);
  c->SetGrid(0,1);

  int *ptCuts = ptCutsOtf;
  if(sysType=="Gmt") ptCuts = ptCutsGmt;

  TH2F *h2D = new TH2F("h2D","",5*32,-TMath::Pi(),TMath::Pi(),2,-0.5,1.5);
  for (int icut=0; icut <=1;++icut){
    string selection(TString::Format("(pt>%d + 20)",ptCuts[icut]).Data());
    TH1D* hEff = getTurnOncurve(tree,ptCuts[icut],h2D,"phi",sysType,selection);
    hEff->SetStats(kFALSE);
    hEff->SetMinimum(0.8); 
    hEff->SetMaximum(1.04);
    hEff->SetMarkerStyle(21+icut);
    hEff->SetMarkerColor(color[icut]);
    hEff->SetXTitle("muon #varphi");
    hEff->SetYTitle("Efficiency"); 
    if (icut==0)hEff->DrawCopy("E0");
    else hEff->DrawCopy("same E0");
    std::string nameCut = TString::Format("%d",ptCuts[icut])+" GeV/c"; 
    if (icut==0) nameCut = "no p_{T} cut";
    l.AddEntry(hEff,nameCut.c_str());   
  }    
  l.DrawClone();
  c->Print(TString::Format("fig_eps/EffVsPhi_%s.eps",sysType.c_str()).Data());
  c->Print(TString::Format("fig_png/EffVsPhi_%s.png",sysType.c_str()).Data());				       
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotEffVsHitPhi(TTree *tree, string sysType="Otf"){

  TCanvas* c = new TCanvas(TString::Format("EffVsHitPhi_%s",sysType.c_str()),
			   TString::Format("EffVsHitPhi_%s",sysType.c_str()),			    
			   460,500);
  
  TLegend l(0.6513158,0.1673729,0.8903509,0.470339,NULL,"brNDC");
  l.SetTextSize(0.05);
  l.SetFillStyle(4000);
  l.SetBorderSize(0);
  l.SetFillColor(10);
  c->SetGrid(0,1);

  //TH2F *h2D = new TH2F("h2D","",1*70,-0.8,0.25,2,-0.5,1.5);
  TH2F *h2D = new TH2F("h2D","",5*64,-TMath::Pi(),TMath::Pi(),2,-0.5,1.5);
  //TH2F *h2D = new TH2F("h2D","",2*40,-0.06,0.04,2,-0.5,1.5);
  

  int *ptCuts = ptCutsOtf;
  if(sysType=="Gmt") ptCuts = ptCutsGmt;

  for (int icut=0; icut <=1;++icut){
    string selection(TString::Format("(pt>%d + 20)",ptCuts[icut]).Data());
    //TH1D* hEff = getTurnOncurve(tree,ptCuts[icut],h2D,"TMath::Nint(phiHit*2*1152./(2*TMath::Pi()))",sysType,selection);
    TH1D* hEff = getTurnOncurve(tree,ptCuts[icut],h2D,"phiHit",sysType,selection);
    hEff->SetStats(kFALSE);
    hEff->SetMinimum(0.7); 
    hEff->SetMaximum(1.04);
    hEff->SetMarkerStyle(21+icut);
    hEff->SetMarkerColor(color[icut]);
    hEff->SetXTitle("hit #varphi");
    hEff->SetYTitle("Efficiency"); 
    if (icut==0) hEff->DrawCopy("E0");
    else hEff->DrawCopy("same");
    std::string nameCut = TString::Format("%d",ptCuts[icut])+" GeV/c"; 
    if (icut==0) nameCut = "no p_{T} cut";
    l.AddEntry(hEff,nameCut.c_str());   
  }    
  l.DrawClone();

  ///Phi range used for generating patterns.
  TLine *aLine = new TLine(0,0,0,0);
  aLine->SetLineWidth(2);
  aLine->SetLineColor(2);
  //aLine->DrawLine(-0.8+2*TMath::Pi(),0,-0.8+2*TMath::Pi(),1.0);
  //aLine->DrawLine(0.25,0,0.25,1.0);

  c->Print(TString::Format("fig_eps/EffVsHitPhi_%s.eps",sysType.c_str()).Data());
  c->Print(TString::Format("fig_png/EffVsHitPhi_%s.png",sysType.c_str()).Data());
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotEffVsEta(TTree *tree, string sysType="Otf"){

  TCanvas* c = new TCanvas(TString::Format("EffVsEta_%s",sysType.c_str()),
			   TString::Format("EffVsEta_%s",sysType.c_str()),			    
			   460,500);
  
  TLegend l(0.6513158,0.1673729,0.8903509,0.470339,NULL,"brNDC");
  l.SetTextSize(0.05);
  l.SetFillStyle(4000);
  l.SetBorderSize(0);
  l.SetFillColor(10);
  c->SetGrid(0,1);

  TH2F *h2D = new TH2F("h2D","",8*26,0.5,1.8,2,-0.5,1.5);

  int *ptCuts = ptCutsOtf;
  if(sysType=="Gmt" || sysType=="Rpc") ptCuts = ptCutsGmt;

  for (int icut=0; icut <=3;++icut){
    string selection(TString::Format("(pt>(%d + 20))",ptCuts[icut]).Data());
    //string selection(TString::Format("(pt<(10))",ptCuts[icut]).Data());
    //string selection("1");
    TH1D* hEff = getTurnOncurve(tree,ptCuts[icut],h2D,"eta",sysType,selection);
    hEff->SetStats(kFALSE);
    hEff->SetMinimum(0.0001); 
    hEff->SetMaximum(1.04);
    hEff->SetMarkerStyle(21+icut);
    hEff->SetMarkerColor(color[icut]);
    hEff->SetXTitle("muon #eta");
    hEff->SetYTitle("Efficiency"); 
    if (icut==0)hEff->DrawCopy();
    else hEff->DrawCopy("same");
    std::string nameCut = TString::Format("%d",ptCuts[icut])+" GeV/c"; 
    if (icut==0) nameCut = "no p_{T} cut";
    l.AddEntry(hEff,nameCut.c_str());   
  }    
  l.DrawClone();

   ///OTF eta range used for generating patterns.
  TLine *aLine = new TLine(0,0,0,0);
  aLine->SetLineWidth(2);
  aLine->SetLineColor(2);
  aLine->DrawLine(0.83,0,0.83,1.0);
  aLine->DrawLine(1.24,0,1.24,1.0);

  c->Print(TString::Format("fig_eps/EffVsEta_%s.eps",sysType.c_str()).Data());
  c->Print(TString::Format("fig_png/EffVsEta_%s.png",sysType.c_str()).Data());
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotOtfVsGmt(TTree *tree, float ptCut){

  TCanvas* c = new TCanvas(TString::Format("OtfVsGmt_%d",(int)ptCut).Data(),
			   TString::Format("OtfVsGmt_%d",(int)ptCut).Data(),
			   460,500);
  
  TLegend l(0.1995614,0.7139831,0.4385965,0.8665254,NULL,"brNDC");
  l.SetTextSize(0.05);
  l.SetFillStyle(4000);
  l.SetBorderSize(0);
  l.SetFillColor(10);
  c->SetLogx(1);  
  c->SetGrid(0,1);
 
  //////Histogram for plotting
  TH2F *h2D = new TH2F("h2D","",nPtBins,ptBins,2,-0.5,1.5);
  string selection(TString::Format("eta<1.19").Data());
  TH1D* hEffGmt = getTurnOncurve(tree,ptCut,h2D,"pt","Gmt",selection);    
  //////Histogram for total eff above ptCut
  TH2F *h2DTmp = new TH2F("h2D","",11,ptCut,9999,2,-0.5,1.5);
  TH1D* hEffGmtTmp = getTurnOncurve(tree,ptCut,h2DTmp,"pt","Gmt",selection);    
  float effGmt = hEffGmtTmp->GetBinContent(1);
  float effOtfMatch = 0.0;
  int match = -1;
  float delta = 999.0;

  for (int icut=13; icut <=31;++icut){
  //for (int icut=18; icut <=18;++icut){
    if(ptBins[icut]>ptCut+20) continue;
    TH1D* hEffOtf = getTurnOncurve(tree,ptBins[icut],h2DTmp,"pt","Otf",selection);    
    float effOtf = hEffOtf->GetBinContent(1);
    std::cout<<ptBins[icut]<<" "<<effOtf<<" "<<fabs(effGmt-effOtf)<<std::endl;
    if(fabs(effGmt-effOtf)<delta){
      match = icut;
      delta = fabs(effGmt-effOtf);      
      effOtfMatch = effOtf;
    }
  }
  std::cout<<"Gmt eff: "<<effGmt<<std::endl;
  std::cout<<"Otf eff: "<<effOtfMatch<<std::endl;

  int icut = 3;
  h2D = new TH2F("h2D","",nPtBins,ptBins,2,-0.5,1.5);
  TH1D* hEffOtf = getTurnOncurve(tree,ptBins[match],h2D,"pt","Otf",selection);
  hEffGmt->SetMarkerStyle(21+icut);
  hEffGmt->SetMarkerColor(color[icut]+10);

  hEffOtf->SetXTitle("muon p_{T} [GeV/c]");
  hEffOtf->SetYTitle("Efficiency"); 
  hEffOtf->SetMinimum(0.0001); 
  hEffOtf->SetMaximum(1.04);
  hEffOtf->GetXaxis()->SetRange(4,100);
  hEffOtf->SetMarkerStyle(8);
  hEffOtf->SetMarkerColor(color[icut]);
  hEffOtf->DrawCopy();

  hEffGmt->DrawCopy("same");
  std::string nameCut(TString::Format("%1.0f GeV/c",ptBins[match]).Data()); 
  if (icut==0) nameCut = "no p_{T} cut";
  l.AddEntry(hEffOtf,("Otf, "+nameCut).c_str());
  string tmp = "Gmt, %1.0f GeV/c";
  if( int(ptBins[match]*10)%10==5)  tmp = "Gmt, %1.1f GeV/c";
  l.AddEntry(hEffGmt,TString::Format(tmp.c_str(),ptCut).Data());
  l.DrawClone();

  TLine aLine(0,0,0,0);
  aLine.SetLineColor(2);
  aLine.SetLineWidth(3);
  aLine.DrawLine(ptCut,0,ptCut,1.04);

  c->Print(TString::Format("fig_eps/OtfVsGmt_%d.eps",(int)ptCut).Data());
  c->Print(TString::Format("fig_png/OtfVsGmt_%d.png",(int)ptCut).Data());
  
  c->SetLogy();
  c->Print(TString::Format("fig_eps/OtfVsGmt_%d_log.eps",(int)ptCut).Data());
  c->Print(TString::Format("fig_png/OtfVsGmt_%d_log.png",(int)ptCut).Data());

}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
TH1F* getRateHisto(TTree *tree, 
		  std::string type = "Vx"){

  std::string friendFileName = "Weights.root";
  std::string friendTreeName = "Weights";
  std::string friendVarName = friendTreeName+".weight";

  if(tree->GetFriend(friendTreeName.c_str())==0) tree->AddFriend(friendTreeName.c_str(),friendFileName.c_str());  

  std::string var = "pt>>hRateVx";
  if(type!="Vx") var = std::string(TString::Format("l1Objects%s[0].pt>>hRate%s",type.c_str(),type.c_str()));

  TH1F *hRate = new TH1F(TString::Format("hRate%s",type.c_str()),"",400,0,200);
  hRate->SetXTitle("p_{T}^{cut} [GeV/c]");
  hRate->SetYTitle("Arbitrary units");

  hRate->SetLineWidth(3);
  std::string selection = friendVarName+"*(eta>1.15)";
  if(type=="Otf") selection+= "*(l1ObjectsOtf.q!=103 && l1ObjectsOtf.q!=104 && l1ObjectsOtf.q!=105 && l1ObjectsOtf.q%100>3 && l1ObjectsOtf.q>650)";

  tree->Draw(var.c_str(),selection.c_str(),"goff");

  return Integrate(hRate);  
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotRate(TTree *tree){


  TH1F *hRateVx = getRateHisto(tree,"Vx");
  TH1F *hRateGmt = getRateHisto(tree,"Gmt");
  TH1F *hRateOtf = getRateHisto(tree,"Otf");

  TF1 *fIntVxMuRate = new TF1("fIntVxMuRate","TMath::Power(x,[0]*TMath::Log(x))*TMath::Power(x,[1])*TMath::Exp([2])",1,1000);
  fIntVxMuRate->SetParameters(-0.235801, -2.82346, 17.162);
  fIntVxMuRate->SetLineColor(6);
 
  hRateVx->SetLineWidth(3);
  hRateGmt->SetLineWidth(3);
  hRateOtf->SetLineWidth(3);

  hRateGmt->SetLineColor(2);
  hRateOtf->SetLineColor(4);

  hRateGmt->SetLineStyle(2);
  
  TCanvas* c = new TCanvas("cRate","Rate",460,500);
  c->SetLogy(1);  
  c->SetGrid(1,1);
  
  TLegend *leg = new TLegend(0.6074561,0.6800847,0.8464912,0.8728814,NULL,"brNDC");
  leg->SetTextSize(0.05);
  leg->SetFillStyle(4000);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);


  int iBinMin = hRateVx->FindBin(1); 
  int iBinMax = hRateVx->FindBin(90); 
  hRateVx->GetXaxis()->SetRange(iBinMin,iBinMax);
  hRateVx->SetMinimum(8E6);

  hRateVx->Draw();
  hRateGmt->Draw("same");
  hRateOtf->Draw("same");

  leg->AddEntry(hRateVx,"#mu rate@Vx");
  leg->AddEntry(hRateGmt,"GMT");
  leg->AddEntry(hRateOtf,"OTF");
  leg->Draw();

  c->Print("fig_eps/Rate.eps");
  c->Print("fig_png/Rate.png");

}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotEffVsRate(TTree *tree,
		   float aThreshold = 20){

  TCanvas* c = new TCanvas("cEffVsRate","EffVsRate",460,500);
  c->SetGrid(1,1);
  
  TLegend *leg = new TLegend(0.2149123,0.6716102,0.4539474,0.8644068,NULL,"brNDC");
  leg->SetTextSize(0.05);
  leg->SetFillStyle(4000);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);

  TH1F *hRateGmt = getRateHisto(tree,"Gmt");
  TH1F *hRateOtf = getRateHisto(tree,"Otf");

  string selection(TString::Format("eta>1.15").Data());
  
  TH2F *h2D = new TH2F("h2D","",1,aThreshold,9999,2,-0.5,1.5);
  
  Double_t eff, rate;  
  int iPtCut;
  for(int i=0;i<32;++i){
    if(fabs(ptBins[i]-aThreshold)<1E-5){
      iPtCut = i;
      break;
    }
  }
  
  
  TGraph *grGmt = new TGraph();
  TGraph *grOtf = new TGraph();

  grGmt->SetMarkerColor(2);
  grGmt->SetMarkerStyle(20);
  grGmt->SetMarkerSize(1.3);

  grOtf->SetMarkerColor(4);
  grOtf->SetMarkerStyle(22);
  grOtf->SetMarkerSize(1.3);

  float effNom, rateNom;
  for(int i=0;i<5;++i){
    float ptCut = ptBins[iPtCut-2+i];
    int iBin = hRateGmt->FindBin(ptCut);
    TH1D* hEffGmt = getTurnOncurve(tree,ptCut,h2D,"pt","Gmt",selection);
    TH1D* hEffOtf = getTurnOncurve(tree,ptCut,h2D,"pt","Otf",selection);
    eff = hEffGmt->GetBinContent(1);
    rate = hRateGmt->GetBinContent(iBin);
    grGmt->SetPoint(i,eff,rate);
    if(i==2){
      effNom = eff;
      rateNom = rate;
    }
    ///
    eff = hEffOtf->GetBinContent(1);
    rate = hRateOtf->GetBinContent(iBin);
    grOtf->SetPoint(i,eff,rate);
    std::cout<<ptCut<<" eff: "<<eff<<" rate: "<<rate<<std::endl;
  }
  
  float maxY, minY;
 if(aThreshold<50){
    maxY = 3E4;
    minY = 500;
  }
  if(aThreshold<20){
    maxY = 7E4;
    minY = 2E3;
  }

  TH1F *hFrame = new TH1F("hFrame",";Efficiency;Rate",2,0.80,1.0);
  hFrame->SetMinimum(minY);
  hFrame->SetMaximum(maxY);
  hFrame->GetYaxis()->SetTitleOffset(1.7);
  hFrame->GetYaxis()->SetLabelSize(0.04);
  hFrame->GetXaxis()->SetLabelSize(0.04);
  hFrame->SetXTitle(TString::Format("Efficiency for p_{T}^{gen}>%d GeV/c",(int)aThreshold));
  hFrame->Draw();

  grGmt->Draw("P");
  grOtf->Draw("P");

  TMarker *aMarker = new TMarker(effNom,rateNom,4);
  aMarker->SetMarkerSize(2.1);
  aMarker->Draw();

  leg->AddEntry(aMarker,"Nominal GMT cut","p");
  leg->AddEntry(grGmt,"GMT","p");
  leg->AddEntry(grOtf,"OTF","p");
  leg->Draw();

  c->Print(TString::Format("fig_eps/RateVsEff_%d.eps",(int)aThreshold).Data());
  c->Print(TString::Format("fig_png/RateVsEff_%d.png",(int)aThreshold).Data());
  c->Print(TString::Format("fig_png/RateVsEff_%d.C",(int)aThreshold).Data());

}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotsAK(){

  utilsL1RpcStyle()->cd();
 
  string path = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Dev3/job_4_ana/";
  path = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Dev3/job_4_ana/";

  //path = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Dev3/job_4_ana/DistanceFromMedian/";

  //path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/Baseline_19_02_2014/SingleMu_8_p/";
  //path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/SingleEtaFullStat/";


  //path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/05RPC/";
  //path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/10_02_2014/";
  //path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/SingleEta/SingleMu_30_p/";
  //path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/noRPC/";
  //path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/noDT_CSC/";



  //path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/05RPC/";
  //path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/01RPC/";
  //path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/1RPC/";

  //path = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Dev3/job_4_ana/TrainSample/SingleMu_30_m/";
  //path = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Dev3/job_4_ana/TestSample/SingleMu_30_m/";

  path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/SingleEta_01RPC/";
  //path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/SingleEta_01RPC_noRPC_ref/";
  //path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev3/job_4_ana/SingleEta_01RPC_noDT_CSC_ref/";
  


  TFile *file = new TFile((path+"EfficiencyTree.root").c_str());
  if (file->IsZombie()) return;

  TTree *tree = (TTree*)file->Get("efficiencyTree");
  makeFriendTree(tree);
  delete tree;
  tree = (TTree*)file->Get("efficiencyTree");

  plotEffVsEta(tree,"Otf");
  plotEffVsRate(tree,20);
  //plotEffPanel(tree,"Otf");
  //plotEffVsHitPhi(tree,"Otf");
  return;

  //plotEffVsEta(tree,"Otf");
  //plotEffVsEta(tree,"Gmt");  
  //plotEffPanel(tree,"Otf");
  //plotEffPanel(tree,"Gmt");
  //plotEffVsHitPhi(tree,"Otf");

  //plotEffVsRate(tree,20);
  return;

  ///Plot panel with a few turn on curves.
  plotEffPanel(tree,"Otf");
  plotEffPanel(tree,"Gmt");
  ///Plot efficiency as a function of generated phi
  plotEffVsPhi(tree,"Gmt");
  plotEffVsPhi(tree,"Otf");
  ///Plot efficiency as a function of phi of hit in reference station
  plotEffVsHitPhi(tree,"Otf");
  plotEffVsHitPhi(tree,"Gmt");
  ///Plot efficiency as a function of generated eta
  plotEffVsEta(tree,"Otf");
  plotEffVsEta(tree,"Gmt");  
  ///Plot Otf vs Gmt efficiency for given pt cut
  plotOtfVsGmt(tree,16);
  plotOtfVsGmt(tree,20);
  plotOtfVsGmt(tree,30);
  ///Plot rate Otf, Gmt and generated
  plotRate(tree);
  ///Plot rate vs efficienncy. Should use some benchmark
  ///physics scenario, eg. H-> tau tau -> mu + X
  plotEffVsRate(tree,16);
  plotEffVsRate(tree,20);
  plotEffVsRate(tree,30);
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
