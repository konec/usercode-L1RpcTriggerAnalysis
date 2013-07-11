#include "utilsDivideErr.C"
#include "utilsL1RpcStyle.C"

unsigned int nPtBins = 32;

double ptBins[33]={0., 0.1, 
		 1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 6., 7., 8., 
		 10., 12., 14., 16., 18., 20., 25., 30., 35., 40., 45., 
		 50., 60., 70., 80., 90., 100., 120., 140., 
		 160. };

const int color[6] = {kBlack, kRed, kGreen, kBlue, kMagenta, kTeal};
const int ptCuts[6] = {1, 5, 10, 16, 30, 100};
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
// TH2F *h2D,           //X axis of this histogram defines binning of output histogram
// string var="pt",     //efficiency will be plotted vs this variable name (name of the branch)
// string sysType="Otf",//type of system: Otf or Gmt
// string sel = "1"     //additional event selection
///////////////////////////////////////////////////////
TH1D *getTurnOncurve(TTree *tree, float ptCut, 
		     TH2F *h2D,
		     string var="pt",     
		     string sysType="Otf",
		     string sel = "1"){
  ///Add events with at least one recontructed L1Mu
  tree->Draw(TString::Format("(l1Objects%s.pt>%3.2f):%s>>h2D",
			     sysType.c_str(),ptCut,var.c_str()).Data(),
	     TString::Format("@l1Objects%s.size()>0 && %s",sysType.c_str(),sel.c_str()).Data(),"goff");
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

  TCanvas* c = new TCanvas("EffVsPt","EffVsPt",460,500);
  
  TLegend l(0.6513158,0.1673729,0.8903509,0.470339,NULL,"brNDC");
  l.SetTextSize(0.05);
  l.SetFillStyle(4000);
  l.SetBorderSize(0);
  l.SetFillColor(10);
  c->SetLogx(1);  
  c->SetGrid(0,1);
 
  TH2F *h2D = new TH2F("h2D","",nPtBins,ptBins,2,-0.5,1.5);

  for (int icut=0; icut <=5;++icut){
    TH1D* hEff = getTurnOncurve(tree,ptCuts[icut],h2D,"pt",sysType);    
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
void plotEffVsPhi(TTree *tree){

  TCanvas* c = new TCanvas("EffVsPhi","EffVsPhi",460,500);
  
  TLegend l(0.6513158,0.1673729,0.8903509,0.470339,NULL,"brNDC");
  l.SetTextSize(0.05);
  l.SetFillStyle(4000);
  l.SetBorderSize(0);
  l.SetFillColor(10);
  c->SetGrid(0,1);

  TH2F *h2D = new TH2F("h2D","",2*32,-3.2,3.2,2,-0.5,1.5);
  for (int icut=0; icut <=5;++icut){
    string selection(TString::Format("pt>%d",ptCuts[icut]).Data());
    TH1D* hEff = getTurnOncurve(tree,ptCuts[icut],h2D,"phi","Otf",selection);
    hEff->SetStats(kFALSE);
    hEff->SetMinimum(0.0001); 
    hEff->SetMaximum(1.04);
    hEff->SetMarkerStyle(21+icut);
    hEff->SetMarkerColor(color[icut]);
    hEff->SetXTitle("muon #varphi");
    hEff->SetYTitle("Efficiency"); 
    if (icut==0)hEff->DrawCopy("E0");
    else hEff->DrawCopy("same");
    std::string nameCut = TString::Format("%d",ptCuts[icut])+" GeV/c"; 
    if (icut==0) nameCut = "no p_{T} cut";
    l.AddEntry(hEff,nameCut.c_str());   
  }    
  l.DrawClone();
  c->Print("fig_eps/EffVsPhi.eps");				       
  c->Print("fig_png/EffVsPhi.png");				       
}



///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotEffVsHitPhi(TTree *tree){

  TCanvas* c = new TCanvas("EffVsHitPhi","EffVsHitPhi",460,500);
  
  TLegend l(0.6513158,0.1673729,0.8903509,0.470339,NULL,"brNDC");
  l.SetTextSize(0.05);
  l.SetFillStyle(4000);
  l.SetBorderSize(0);
  l.SetFillColor(10);
  c->SetGrid(0,1);

  TH2F *h2D = new TH2F("h2D","",4*24,-1.3,1.1,2,-0.5,1.5);
  for (int icut=0; icut <=5;++icut){
    string selection(TString::Format("pt>%d",ptCuts[icut]).Data());
    TH1D* hEff = getTurnOncurve(tree,ptCuts[icut],h2D,"phiHit","Otf",selection);
    hEff->SetStats(kFALSE);
    hEff->SetMinimum(-0.0001); 
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
  c->Print("fig_eps/EffVsHitPhi.eps");				       
  c->Print("fig_png/EffVsHitPhi.png");				       
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotEffVsEta(TTree *tree){

  TCanvas* c = new TCanvas("EffVsEta","EffVsEta",460,500);
  
  TLegend l(0.6513158,0.1673729,0.8903509,0.470339,NULL,"brNDC");
  l.SetTextSize(0.05);
  l.SetFillStyle(4000);
  l.SetBorderSize(0);
  l.SetFillColor(10);
  c->SetGrid(0,1);

  TH2F *h2D = new TH2F("h2D","",2*26,0.5,1.8,2,-0.5,1.5);
  for (int icut=0; icut <=5;++icut){
    string selection(TString::Format("pt>%d",ptCuts[icut]).Data());
    TH1D* hEff = getTurnOncurve(tree,ptCuts[icut],h2D,"eta","Otf",selection);
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
  c->Print("fig_eps/EffVsEta.eps");				       
  c->Print("fig_png/EffVsEta.png");				       
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotOtfVsGmt(TTree *tree, float ptCut){

  TCanvas* c = new TCanvas(TString::Format("OtfVsGmt_%d",(int)ptCut).Data(),
			   TString::Format("OtfVsGmt_%d",(int)ptCut).Data(),
			   460,500);
  
  TLegend l(0.5592105,0.5783898,0.7982456,0.7309322,NULL,"brNDC");
  l.SetTextSize(0.05);
  l.SetFillStyle(4000);
  l.SetBorderSize(0);
  l.SetFillColor(10);
  c->SetLogx(1);  
  c->SetGrid(0,1);
 
  TH2F *h2D = new TH2F("h2D","",nPtBins,ptBins,2,-0.5,1.5);

  TH1D* hEffOtf = getTurnOncurve(tree,ptCut,h2D,"pt","Otf");    
  int iBin = hEffOtf->FindBin(ptCut);
  float effOtf = hEffOtf->GetBinContent(iBin);
  int match = -1;
  float delta = 999.0;
  for (int icut=0; icut <=31;++icut){
    TH1D* hEffGmt = getTurnOncurve(tree,ptBins[icut],h2D,"pt","Gmt");    
    float effGmt = hEffGmt->GetBinContent(iBin);
    if(fabs(effGmt-effOtf)<delta){
      match = icut;
      delta = fabs(effGmt-effOtf);
    }
  }
  std::cout<<"Otf eff: "<<effOtf<<std::endl;
  std::cout<<"Gmt eff: "<<effGmt<<std::endl;

  int icut = 3;
  TH1D* hEffGmt = getTurnOncurve(tree,ptBins[match],h2D,"pt","Gmt");
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
  std::string nameCut(TString::Format("%1.0f GeV/c",ptCut).Data()); 
  if (icut==0) nameCut = "no p_{T} cut";
  l.AddEntry(hEffOtf,("Otf, "+nameCut).c_str());
  string tmp = "Gmt, %1.0f GeV/c";
  if( int(ptBins[match]*10)%10==5)  tmp = "Gmt, %1.1f GeV/c";
  l.AddEntry(hEffGmt,TString::Format(tmp.c_str(),ptBins[match]).Data());
  l.DrawClone();

  TLine aLine(0,0,0,0);
  aLine.SetLineColor(2);
  aLine.SetLineWidth(3);
  aLine.DrawLine(ptBins[match],0,ptBins[match],1.04);

  c->Print(TString::Format("fig_eps/OtfVsGmt_%d.eps",(int)ptCut).Data());
  c->Print(TString::Format("fig_png/OtfVsGmt_%d.png",(int)ptCut).Data());
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
void plotsAK(){

  utilsL1RpcStyle()->cd();
 
  string path = "/home/akalinow/scratch/CMS/OverlapTrackFinder/job_4_ana/test/";
  //path = "/home/akalinow/scratch0/OverlapTrackFinder/job_4_ana/";
  TFile *file = new TFile((path+"EfficiencyTree.root").c_str());
  if (file->IsZombie()) return;

  TTree *tree = (TTree*)file->Get("efficiencyTree");
  
  ///Plot panel with a few turn on curves.
  plotEffPanel(tree,"Otf");
  //plotEffPanel(tree,"Gmt");
  ///Plot efficiency as a function of generated phi
  plotEffVsPhi(tree);
  ///Plot efficiency as a function of phi of hit in reference station
  plotEffVsHitPhi(tree);
  ///Plot efficiency as a function of generated eta
  plotEffVsEta(tree);
  ///Plot Otf vs Gmt efficiency for given pt cut
  //plotOtfVsGmt(tree,10);
  //plotOtfVsGmt(tree,16);
  //plotOtfVsGmt(tree,30);
}
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
