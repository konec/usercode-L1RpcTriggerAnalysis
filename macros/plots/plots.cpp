
unsigned int nPtBins = 32;

double ptBins[33]={0., 0.1, 
		 1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 6., 7., 8., 
		 10., 12., 14., 16., 18., 20., 25., 30., 35., 40., 45., 
		 50., 60., 70., 80., 90., 100., 120., 140., 
		 160. };


string path = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Dev3/job_3_pat/SingleMu_18_p/";


//path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev5/job_3_pat/FullEta/5bins/SingleMu_7_p/";
//path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev5/job_4_ana/";


//path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev5/job_3_pat/FullEta/SingleMu_7_p/";
//path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev4/job_3_pat/FullEta/SingleMu_16_p/";


//path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev4/job_3_pat/Dynamic/SingleMu_10_p/";
//path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev4/job_3_pat/05RPC/SingleMu_16_p/";
//path = "/home/akalinow/scratch0/CMS/OverlapTrackFinder/Dev4/job_3_pat/FullEta05RPC/SingleMu_10_p/";

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotRecoPt(string sysType="Otf",
		int ipt=22){

  string filePath = path;// + string(TString::Format("SingleMu_%d_p/",ipt));
  
  TFile *file = new TFile((filePath+"EfficiencyTree.root").c_str());
  if (file->IsZombie()) return;
  
  TTree *tree = (TTree*)file->Get("efficiencyTree");
  
  TH1F *h1D = new TH1F("h1D",";p_{T}^{L1} [GeV/c];",nPtBins,ptBins);
  //h1D->SetStats(kFALSE);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(11);
  gStyle->SetStatW(0.3);
  gStyle->SetStatY(0.9);
  gStyle->SetStatX(0.9);
  
  TH1F *h1 = (TH1F*)h1D->Clone(TString::Format("h1%s",sysType.c_str()));
  std::string selection = "eta<1.6 && eta>1.25 && pt<25 && pt>20";
  
  //std::string selection = "eta<0.93 && eta<1.25 && pt<25 && pt>20";
  //std::string selection = "eta>0.93 && eta<1.25 && pt<15 && pt>10";
  
  tree->Draw(TString::Format("l1Objects%s[0].pt>>h1%s",sysType.c_str(),sysType.c_str())
	     ,selection.c_str(),"goff");

  TH1F *h2 = (TH1F*)h1D->Clone(TString::Format("h2%s",sysType.c_str()));
  selection = "eta<1.6 && eta<1.25 && pt<15 && pt>10";

  tree->Draw(TString::Format("l1Objects%s[0].pt>>h2%s",sysType.c_str(),sysType.c_str())
  	     ,selection.c_str(),"goff");

  TCanvas* c = new TCanvas(TString::Format("RecoPt_%s",sysType.c_str()),"RecoPt",460,500);

  h1->Scale(1.0/h1->Integral());
  h2->Scale(1.0/h2->Integral());

  h1->Fit("gaus","S","",ptBins[ipt]-30,ptBins[ipt]+30);
  h2->Fit("gaus","S","",ptBins[ipt]-30,ptBins[ipt]+30);			  
  
  h2->SetLineColor(4);
  h2->SetLineWidth(3);

  h2->Draw();
  h1->Draw("same");

  TLatex aLatex(100,4000,
		TString::Format("%d<p_{T}^{gen}<%d",(int)ptBins[ipt],(int)ptBins[ipt+1]).Data());
  aLatex.Draw();
  
  float x = (ptBins[ipt] + ptBins[ipt+1])/2.0;
  TLine aLine(x,0,x,4000);
  aLine.SetLineWidth(3);
  aLine.Draw();

  c->Print(TString::Format("fig_eps/RecoPt%s_%d.eps",sysType.c_str(),ipt).Data());
  c->Print(TString::Format("fig_png/RecoPt%s_%d.png",sysType.c_str(),ipt).Data());

}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotRecoPhi(int ipt=0){

  string filePath = path;
  if(ipt>0) filePath = path+ string(TString::Format("SingleMu_%d_p/",ipt));
  
  TFile *file = new TFile((filePath+"EfficiencyTree.root").c_str());
  if (file->IsZombie()) return;
  
  TTree *tree = (TTree*)file->Get("efficiencyTree");
  
  TH1F *h1D = new TH1F("h1D",";#varphi^{L1}-#varphi^{SimHit@ref st.};",100,-0.2,0.2);
  h1D->SetLineWidth(3);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(11);
  gStyle->SetStatW(0.3);
  gStyle->SetStatY(0.9);
  gStyle->SetStatX(0.9);
  
  TH1F *hOtf = (TH1F*)h1D->Clone("hOtf");
  TH1F *hGmt = (TH1F*)h1D->Clone("hGmt");
 
  hGmt->SetLineColor(2);
  hGmt->SetLineStyle(2);
  hOtf->SetLineColor(4);
 
  std::string sysType = "Otf";
  std::string selection = "eta>1.1";
  tree->Draw(TString::Format("l1Objects%s[0].phi - phiHit>>h%s",sysType.c_str(),sysType.c_str())
	     ,selection.c_str(),"goff");
  tree->Draw(TString::Format("l1Objects%s[0].phi - phiHit - 2*TMath::Pi()>>h%s",sysType.c_str(),sysType.c_str())
  	     ,selection.c_str(),"goff");

  std::string sysType = "Gmt";
  selection = "eta>1.1";
  tree->Draw(TString::Format("l1Objects%s[0].phi - phiHit>>h%s",sysType.c_str(),sysType.c_str())
	     ,selection.c_str(),"goff");
  tree->Draw(TString::Format("l1Objects%s[0].phi - phiHit- 2*TMath::Pi()>>h%s",sysType.c_str(),sysType.c_str())
	     ,selection.c_str(),"goff");

  TCanvas* c = new TCanvas("RecoPhiRes","RecoPhiRes",460,500);
  TLegend *leg = new TLegend(0.6074561,0.6800847,0.8464912,0.8728814,NULL,"brNDC");
  leg->SetTextSize(0.05);
  leg->SetFillStyle(4000);
  leg->SetBorderSize(0);
  leg->SetFillColor(10);			  

  hOtf->Draw();
  hGmt->Draw("same");
  leg->AddEntry(hOtf,"OTF");
  leg->AddEntry(hGmt,"GMT");
  leg->Draw();

  if(ipt>0){
    TLatex aLatex(0.02,1500,
		  TString::Format("%d<p_{T}^{gen}<%d",(int)ptBins[ipt],(int)ptBins[ipt+1]).Data());
    aLatex.Draw();
  }
  

  c->Print(TString::Format("fig_eps/RecoPhiRes_%d.eps",ipt).Data());
  c->Print(TString::Format("fig_png/RecoPhiRes_%d.png",ipt).Data());

}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotLLH(int ipt=22){

  string filePath path + string(TString::Format("SingleMu_%d_m/",ipt));

  TFile *file = new TFile((filePath+"EfficiencyTree.root").c_str());
  if (file->IsZombie()) return;
  
  TTree *tree = (TTree*)file->Get("efficiencyTree");

  TH1F *hLLH = new TH1F("hLLH","",50,-100,0);
  hLLH->SetStats(kFALSE);

  TH1F *hLLHgood = hLLH->Clone("hLLHgood");
  TH1F *hLLHbad = hLLH->Clone("hLLHbad");

  tree->Draw("l1ObjectsOtf.eta>>hLLHgood",
	     TString::Format("l1ObjectsOtf.pt>%3.2f && l1ObjectsOtf.pt<%3.2f",ptBins[ipt-1],ptBins[ipt+1]).Data(),
	     "goff");

  tree->Draw("l1ObjectsOtf.eta>>hLLHbad",
	     TString::Format("l1ObjectsOtf.pt<%3.2f",ptBins[ipt]).Data(),
	     "goff");


  hLLHgood->Scale(1.0/hLLHgood->Integral());
  hLLHbad->Scale(1.0/hLLHbad->Integral());

  hLLHbad->SetLineColor(2);

  hLLHgood->Draw();
  hLLHbad->Draw("same");

}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotGoldenPattern(int iPt=9, int iTower=9, int iRef=44, int iCharge=-1){

  TFile *file = new TFile((path+"GoldenPatterns.root").c_str());

  TObjArray grRPC, grCSC, grDT;

  iCharge*=-1;
  iCharge+=1;
  TString dirName = TString::Format("fig_png/iPt_%d_iTower_%d_iRef_%d_iCh_%d",iPt,iTower,iRef,iCharge);
  gSystem->mkdir(dirName);

  TString locName = TString::Format("tower%d_ref%d_pt%d_sign%d",iTower,iRef,iPt,iCharge);
  std::cout<<locName.Data()<<std::endl;
  ////
  //TString cName = "Dev"+locName;
  TString cName = "RPC"+locName;
  TCanvas *cRPC = (TCanvas*)file->Get(cName.Data());
  cName = "DT"+locName;
  TCanvas *cDT = (TCanvas*)file->Get(cName.Data());
  cName = "CSC"+locName;
  TCanvas *cCSC = (TCanvas*)file->Get(cName.Data());
  ////
  TList *aPrimList = cRPC->GetListOfPrimitives();
  TIter padItr(aPrimList);
  TObject *aPad = padItr();
  while (aPad){
    TIter objItr(((TPad*)aPad)->GetListOfPrimitives());
    TObject *obj = objItr();
    while (obj){
      string aName(obj->GetName());
      if(aName.find("layer")!=std::string::npos){
	TGraph *aGr = (TGraph*)obj; 
	Double_t x,y, sum=0;	
	for(int i=0;i<aGr->GetN();++i){aGr->GetPoint(i,x,y);sum+=y;}
	std::cout<<aGr->GetName()<<" sum: "<<sum<<std::endl;
	if(sum>1E-2) grRPC.Add(aGr);			
      }
      obj = objItr();
    }
    aPad = padItr();
  }
  /////////
  aPrimList = cCSC->GetListOfPrimitives();
  TIter padItr(aPrimList);
  aPad = padItr();
  while (aPad){
    TIter objItr(((TPad*)aPad)->GetListOfPrimitives());
    TObject *obj = objItr();
    while (obj){
      string aName(obj->GetName());
      if(aName.find("layer")!=std::string::npos){
	TGraph *aGr = (TGraph*)obj;
	Double_t x,y, sum=0;
	for(int i=0;i<aGr->GetN();++i){aGr->GetPoint(i,x,y);sum+=y;}
	aGr->Print("all");
	std::cout<<aGr->GetName()<<" sum: "<<sum<<std::endl;
	if(sum>1E-2) grCSC.Add(aGr);
      }
      obj = objItr();
    }
    aPad = padItr();
  }
  ////
  aPrimList = cDT->GetListOfPrimitives();
  TIter padItr(aPrimList);
  aPad = padItr();
  while (aPad){
    TIter objItr(((TPad*)aPad)->GetListOfPrimitives());
    TObject *obj = objItr();
    while (obj){
      string aName(obj->GetName());
      if(aName.find("layer")!=std::string::npos){
	TGraph *aGr = (TGraph*)obj; 
	Double_t x,y, sum=0;
	for(int i=0;i<aGr->GetN();++i){aGr->GetPoint(i,x,y);sum+=y;}
	std::cout<<aGr->GetName()<<" sum: "<<sum<<std::endl;
	if(sum>1E-2) grDT.Add(aGr);
      }
      obj = objItr();
    }
    aPad = padItr();
  }
  ////
  std::cout<<grRPC.GetEntries()<<std::endl;
  std::cout<<grCSC.GetEntries()<<std::endl;
  std::cout<<grDT.GetEntries()<<std::endl;

  TCanvas* c = new TCanvas("GoldenPattern","GoldenPattern",460,500);	
  c->SetRightMargin(0.02);
  c->SetLeftMargin(0.15);

  for(int i=0;i<grRPC.GetEntries();++i){
    TGraph *gr = (TGraph*)grRPC.At(i);
    gr->GetXaxis()->SetLabelSize(0.06);
    gr->GetXaxis()->SetLabelOffset(0.03);
    gr->GetYaxis()->SetLabelSize(0.06);
    gr->SetFillColor(kBlue-10);
    gr->Draw("A b");
    c->Print(TString::Format("%s/pdf_%s.png",dirName.Data(),gr->GetName()));
  }
  /////
  for(int i=0;i<grDT.GetEntries();++i){
    TGraph *gr = (TGraph*)grDT.At(i);
    gr->GetXaxis()->SetLabelSize(0.06);
    gr->GetXaxis()->SetLabelOffset(0.03);
    gr->GetYaxis()->SetLabelSize(0.06);
    gr->SetFillColor(kBlue-10);
    gr->Draw("A b");
    c->Print(TString::Format("%s/pdf_%s.png",dirName.Data(),gr->GetName()));
  }
  //////
  for(int i=0;i<grCSC.GetEntries();++i){
    TGraph *gr = (TGraph*)grCSC.At(i);
    gr->GetXaxis()->SetLabelSize(0.06);
    gr->GetXaxis()->SetLabelOffset(0.03);
    gr->GetYaxis()->SetLabelSize(0.06);
    gr->SetFillColor(kBlue-10);
    gr->Draw("A b");
    c->Print(TString::Format("%s/pdf_%s.png",dirName.Data(),gr->GetName()));
  }
  //////
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plots(){


  //plotRecoPt("Otf",16);
  //return;

  //plotRecoPhi(10);
  //return;

  plotGoldenPattern(18,1,3211,1);  
  plotGoldenPattern(18,1,3221,1); 
  plotGoldenPattern(18,1,3231,1); 
  plotGoldenPattern(18,1,3222,1); 
  plotGoldenPattern(18,1,3232,1); 
  return;

  plotGoldenPattern(17,0,1202,1); 
  plotGoldenPattern(17,0,2202,1); 
  plotGoldenPattern(17,0,3202,1); 
  plotGoldenPattern(17,0,4202,1); 
  //plotGoldenPattern(7,0,203,1); 
  //plotGoldenPattern(16,0,304,1); 
  return;


  ///
  plotRecoPhi(0);
  plotRecoPhi(16);
  plotRecoPhi(22);
  ///
  plotRecoPt("Otf",16);
  plotRecoPt("Gmt",16);
  ///
  plotRecoPt("Otf",22);
  plotRecoPt("Gmt",22);
  ///
  plotGoldenPattern(16,6,302,-1);  
  plotGoldenPattern(16,7,302,-1);  
  plotGoldenPattern(16,8,312,-1);  
  plotGoldenPattern(16,9,312,-1);  
  
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
