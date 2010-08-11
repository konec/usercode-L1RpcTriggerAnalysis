#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <TROOT.h>
#include <TFile.h>
#include <TText.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TProfile.h>
#include <TLine.h>
#include <TLatex.h>
#include <TGraphErrors.h>
#include <TPaveStats.h>
#include <TLegend.h>
using namespace std;
#include <math.h>
#include "Style.C"    // "/afs/cern.ch/cms/L1/rpc/plots/macros/Style.C"
#include "FigUtils.C" // "/afs/cern.ch/cms/L1/rpc/plots/macros/FigUtils.C"

void eff(TString datafile,
	 TString outfile="",    
	 TString runrange="",
	 TString datasettype="") {

  const Float_t legendFontSize = 0.030;
  const Float_t legendOffset   = 0.030;

  setTDRStyle();
  TStyle *tdrStyle=(TStyle*)(gROOT->FindObject("tdrStyle"));
  tdrStyle->SetOptStat(0);
  tdrStyle->SetOptLogy(0);
  tdrStyle->SetOptTitle(0);//off title
  tdrStyle->SetPadGridY(0);
  tdrStyle->SetPadRightMargin(0.02);
  tdrStyle->SetPadLeftMargin(0.12);
  tdrStyle->SetPalette(1);
  tdrStyle->SetOptTitle(1);
  tdrStyle->SetTitleXSize(0.04);
  tdrStyle->SetTitleYSize(0.04);
  tdrStyle->SetTitleXOffset(1.0);
  tdrStyle->SetTitleYOffset(1.5);
  gROOT->ForceStyle();

  cout << "DrawEff:"<<endl 
       << " Input root file      = " << datafile << endl
       << " Output file prefix   = " << outfile
       << endl;

  if(outfile=="") outfile="drawEff";
  TFile *f = new TFile( (outfile+".root").Data(), "RECREATE");
  f->mkdir("Eff");
  f->ls();

  //tdrStyle->Write();
  //gStyle->Write();

  TFile *file = new TFile( datafile.Data(), "OLD" );
  file->ls();

  tdrStyle->cd();
  gStyle->SetOptStat(1110);

  //----muon distribution
  TCanvas *c;
  TH1D *h,*h1,*h2,*h3,*h4,*h5;
  TH2D *hh;
  TGraphErrors *g;
  float x1,x2,y1,y2;
  TPaveStats *st;
  Int_t integral;
  TLegend *leg;

  c = new TCanvas("c0Mu","Global muons",1500,500);
  c->Divide(3,1);
  c->cd(1);
  h=(TH1D*)(gROOT->FindObject("hMuonPt"));  
  h->DrawCopy();
  c->cd(2);
  h=(TH1D*)(gROOT->FindObject("hMuonEta"));  
  h->SetMinimum(0);
  h->DrawCopy();
  c->cd(3);
  h=(TH1D*)(gROOT->FindObject("hMuonPhi"));  
  h->SetMinimum(0);
  h->DrawCopy();
  c->Print( outfile + "_" + c->GetName() + ".eps" );
  c->Print( outfile + "_" + c->GetName() + ".gif" );
  f->cd("Eff"); c->Write(); f->cd(); file->cd();

  //----track distribution
  c = new TCanvas("c0Tk","Tracks matching global muons",1500,500);
  c->Divide(3,1);
  c->cd(1);
  h=(TH1D*)(gROOT->FindObject("hTrackPt"));  
  h->DrawCopy();
  c->cd(2);
  h=(TH1D*)(gROOT->FindObject("hTrackEta"));  
  h->DrawCopy();
  c->cd(3);
  h=(TH1D*)(gROOT->FindObject("hTrackPhi"));  
  h->DrawCopy();
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //----hits population
  c = new TCanvas("c1H","Hits population",1000,500);
  c->Divide(2,1);
  c->cd(1);
  h=(TH1D*)(gROOT->FindObject("hHitsB"));  
  h->SetMinimum(0.);
  h->DrawCopy(); 
  c->cd(2);
  h=(TH1D*)(gROOT->FindObject("hHitsE"));  
  h->SetMinimum(0.);
  h->DrawCopy();
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //----hits population
  c = new TCanvas("c1D","Dets population",1000,500);
  c->Divide(2,1);
  c->cd(1);
  h=(TH1D*)(gROOT->FindObject("hDetsB_100"));  
  h->SetMinimum(0.);
  h->DrawCopy(); 
  c->cd(2);
  h=(TH1D*)(gROOT->FindObject("hDetsE_100"));  
  h->SetMinimum(0.);
  h->DrawCopy();
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //----hits efficiency
  c = new TCanvas("c2","Hits efficiency N/D",500,500);
  h1=(TH1D*)(gROOT->FindObject("hEfficHits_N"));  
  h2=(TH1D*)(gROOT->FindObject("hEfficHits_D"));  
  h = DivideErr(h1,h2,"hEfficHits","B");
  h->SetMinimum(0.);
  h->SetMaximum(1.002);
  h->SetXTitle("#eta");
  h->SetYTitle("efficiency");
  h->SetMarkerStyle(25);
  h->SetMarkerColor(4);
  h->DrawCopy("E");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //----hits efficiency
  c = new TCanvas("c2R","Hits efficiency H/N",500,500);
  h1=(TH1D*)(gROOT->FindObject("hEfficHits_H"));  
  h2=(TH1D*)(gROOT->FindObject("hEfficHits_N"));  
  h = DivideErr(h1,h2,"hEfficHitsR","B");
  h->SetMinimum(0.);
  h->SetMaximum(1.002);
  h->SetXTitle("#eta");
  h->SetYTitle("efficiency");
  h->SetMarkerStyle(25);
  h->SetMarkerColor(4);
  h->DrawCopy("E");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //--- efficiency for trigger wrt Mu 
  c = new TCanvas("c3","Trigger eff wrt Mu",500,500);
  h1=(TH1D*)(gROOT->FindObject("hEfficMu_N"));  
  h2=(TH1D*)(gROOT->FindObject("hEfficMu_D"));  
  h = DivideErr(h1,h2,"hEfficMu","B");
  h->SetMinimum(0.);
  h->SetMaximum(1.002);
  h->SetXTitle("#eta");
  h->SetYTitle("efficiency");
  h->SetMarkerStyle(25);
  h->SetMarkerColor(4);
  h->DrawCopy("E");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- efficiency for trigger wrt Tk 
  c = new TCanvas("c4","Trigger eff wrt Tk",500,500);
  h1=(TH1D*)(gROOT->FindObject("hEfficTk_N"));  
  h2=(TH1D*)(gROOT->FindObject("hEfficTk_D"));  
  h = DivideErr(h1,h2,"hEfficTk","B");
  h->SetMinimum(0.);
  h->SetMaximum(1.002);
  h->SetXTitle("#eta");
  h->SetYTitle("efficiency");
  h->SetMarkerStyle(25);
  h->SetMarkerColor(4);
  h->DrawCopy("E");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- trigger turn-on for pTmin and 7 GeV cuts 
  c = new TCanvas("c5","Trg.eff. vs glb.muon p_{T}, L1 RPC p_{T}>p_{T}^{min}, 7 GeV",500,500);
  h1=(TH1D*)(gROOT->FindObject("hEfficMuPt_N"));  
  h2=(TH1D*)(gROOT->FindObject("hEfficMuPt_D"));  
  h3=(TH1D*)(gROOT->FindObject("hEfficMuPt7_N"));
  h4 = DivideErr(h1,h2,"hEfficMuPt","B");
  h5 = DivideErr(h3,h2,"hEfficMuPt7","B");
  h4->SetMinimum(0.);
  h4->SetMaximum(1.002);
  h4->SetXTitle("Glb.muon p_{T} [GeV]");
  h4->SetYTitle("Efficiency");
  h4->SetTitle("");
  h4->SetMarkerStyle(25);
  h4->SetMarkerColor(4);
  h4->SetFillStyle(0);
  h4->SetStats(0);
  h4->DrawCopy("E");
  h5->SetMarkerStyle(23);
  h5->SetMarkerColor(2);
  h4->SetFillStyle(0);
  h5->DrawCopy("E SAME");
  leg = new TLegend(0.6,0.25,0.95,0.5,"Trigger turn-on:","rNDC");
  leg->AddEntry(h4,"No cut on L1 RPC p_{T}","lp");
  leg->AddEntry(h5,"L1 RPC p_{T}>7 GeV","lp");
  leg->SetFillStyle(0);
  leg->Draw();
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- trigger turn-on for pTmin and 7 GeV cuts 
  c = new TCanvas("c6","Trg.eff. vs track p_{T}, L1 RPC p_{T}>p_{T}^{min}, 7 GeV",500,500);
  h1=(TH1D*)(gROOT->FindObject("hEfficTkPt_N"));  
  h2=(TH1D*)(gROOT->FindObject("hEfficTkPt_D"));  
  h3=(TH1D*)(gROOT->FindObject("hEfficTkPt7_N"));
  h4 = DivideErr(h1,h2,"hEfficTkPt","B");
  h5 = DivideErr(h3,h2,"hEfficTkPt7","B");
  h4->SetMinimum(0.);
  h4->SetMaximum(1.002);
  h4->SetXTitle("Track p_{T} [GeV]");
  h4->SetYTitle("Efficiency");
  h4->SetTitle("");
  h4->SetMarkerStyle(25);
  h4->SetMarkerColor(4);
  h4->SetFillStyle(0);
  h4->SetStats(0);
  h4->DrawCopy("E");
  h5->SetMarkerStyle(23);
  h5->SetMarkerColor(2);
  h5->SetFillStyle(0);
  h5->DrawCopy("E SAME");
  leg = new TLegend(0.6,0.25,0.95,0.5,"Trigger turn-on:","rNDC");
  leg->AddEntry(h4,"No cut on L1 RPC p_{T}","lp");
  leg->AddEntry(h5,"L1 RPC p_{T}>7 GeV","lp");
  leg->SetFillStyle(0);
  leg->Draw();
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- (ETA,PHI) for L1RPC and L1DT/L1CSC
  c = new TCanvas("c7","L1 muon triggers (#eta,#phi)",1000,500);
  c->Divide(2,1);
  c->cd(1);
  hh=(TH2D*)(gROOT->FindObject("hDistL1Rpc"));
  hh->DrawCopy("BOX");
  c->cd(2);
  hh=(TH2D*)(gROOT->FindObject("hDistL1Other"));
  hh->DrawCopy("BOX");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- (ETA,PHI) for L1RPC and L1DT/L1CSC
  c = new TCanvas("c7a","L1 RPC (#eta,#phi), quality 0",1000,500);
  c->Divide(2,1);
  c->cd(1);
  hh=(TH2D*)(gROOT->FindObject("h2L1RpcBX"));
  hh->DrawCopy("BOX");
  c->cd(2);
  hh=(TH2D*)(gROOT->FindObject("h2L1RpcBX1"));
  hh->DrawCopy("BOX");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- (ETA,PHI) for L1RPC and L1DT/L1CSC
  c = new TCanvas("c7b","L1 RPC (#eta,#phi), quality 0",1000,500);
  c->Divide(2,1);
  c->cd(1);
  hh=(TH2D*)(gROOT->FindObject("h2L1RpcBX_q0"));
  hh->DrawCopy("BOX");
  c->cd(2);
  hh=(TH2D*)(gROOT->FindObject("h2L1RpcBX1_q0"));
  hh->DrawCopy("BOX");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- (ETA,PHI) for L1RPC and L1DT/L1CSC
  c = new TCanvas("c7c","L1 RPC (#eta,#phi), quality 1",1000,500);
  c->Divide(2,1);
  c->cd(1);
  hh=(TH2D*)(gROOT->FindObject("h2L1RpcBX_q1"));
  hh->DrawCopy("BOX");
  c->cd(2);
  hh=(TH2D*)(gROOT->FindObject("h2L1RpcBX1_q1"));
  hh->DrawCopy("BOX");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- (ETA,PHI) for L1RPC and L1DT/L1CSC
  c = new TCanvas("c7d","L1 RPC (#eta,#phi), quality 2",1000,500);
  c->Divide(2,1);
  c->cd(1);
  hh=(TH2D*)(gROOT->FindObject("h2L1RpcBX_q2"));
  hh->DrawCopy("BOX");
  c->cd(2);
  hh=(TH2D*)(gROOT->FindObject("h2L1RpcBX1_q2"));
  hh->DrawCopy("BOX");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- (ETA,PHI) for L1RPC and L1DT/L1CSC
  c = new TCanvas("c7e","L1 RPC (#eta,#phi), quality 3",1000,500);
  c->Divide(2,1);
  c->cd(1);
  hh=(TH2D*)(gROOT->FindObject("h2L1RpcBX_q3"));
  hh->DrawCopy("BOX");
  c->cd(2);
  hh=(TH2D*)(gROOT->FindObject("h2L1RpcBX1_q3"));
  hh->DrawCopy("BOX");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- First L1 RPC candidate BX wrt L1
  c = new TCanvas("c8","First L1 RPC candidate BX wrt L1",500,500);
  h=(TH1D*)(gROOT->FindObject("hL1RpcBX"));
  h->Sumw2();
  integral = h->Integral(); 
  h->Scale(1./integral);
  h->SetYTitle("Fraction of triggers");
  gPad->SetLogy();
  h->SetNdivisions(605,"X");
  h->DrawCopy("HIST TEXT0");
  h->DrawCopy("E0 SAME");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- All L1 RPC candidates BX wrt L1
  c = new TCanvas("c8a","L1 RPC candidates BX wrt L1",500,500);
  h=(TH1D*)(gROOT->FindObject("hL1RpcBX_all"));
  h->Sumw2();
  integral = h->Integral(); 
  h->Scale(1./integral);
  h->SetYTitle("Fraction of candidates");
  gPad->SetLogy();
  h->SetNdivisions(605,"X");
  h->DrawCopy("HIST TEXT0");
  h->DrawCopy("E0 SAME");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- First L1 RPC candidate BX
  c = new TCanvas("c8b","First L1 RPC candidate BX",500,500);
  h=(TH1D*)(gROOT->FindObject("hL1RpcGBX"));
  gPad->SetLogy();
  h->SetMinimum(0.1);
  //h->DrawCopy("HIST TEXT0");
  //h->DrawCopy("E0 SAME");
  h->DrawCopy();
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- Global muon BX
  c = new TCanvas("c8c","Glb.muon BX",500,500);
  h=(TH1D*)(gROOT->FindObject("hMuGBX"));
  gPad->SetLogy();
  h->SetMinimum(0.1);
  //h->DrawCopy("HIST TEXT0");
  //h->DrawCopy("E0 SAME");
  h->DrawCopy();
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- Chamber efficiency
  c =  new TCanvas("c9","Chamber efficiency",1000,500);
  c->Divide(2,1);
  c->cd(1);
  h1=(TH1D*)(gROOT->FindObject("hEfficChambBar_N")); // nominator   (barrel)  
  h2=(TH1D*)(gROOT->FindObject("hEfficChambBar_D")); // denominator (barrel)  
  h = DivideErr(h1,h2,"hEfficChambBar","B");
  h->SetMinimum(0.);
  h->SetMaximum(1.002);
  h->SetXTitle("Layer");
  h->SetYTitle("Efficiency");
  h->SetMarkerStyle(0);
  h->SetStats(1);
  h=(TH1D*)(h->DrawCopy("E1 HIST"));
  gPad->Update(); // to make sure stats box is drawn
  st = (TPaveStats*)h->FindObject("stats");
  if(st) {
    st->SetOptStat(10);
    st->SetTextColor(h->GetLineColor());
    gPad->Update();
  }
  c->cd(2);
  h3=(TH1D*)(gROOT->FindObject("hEfficChambEnd_N")); // nominator   (endcap)   
  h4=(TH1D*)(gROOT->FindObject("hEfficChambEnd_D")); // denominator (endcap)   
  h = DivideErr(h3,h4,"hEfficChambEnd","B");
  h->SetMinimum(0.);
  h->SetMaximum(1.002);
  h->SetXTitle("Layer");
  h->SetYTitle("Efficiency");
  h->SetMarkerStyle(0);
  h=(TH1D*)(h->DrawCopy("E1 HIST"));
  h->SetStats(1);
  gPad->Update(); // to make sure stats box is drawn
  st = (TPaveStats*)h->FindObject("stats");
  if(st) {
    st->SetOptStat(10);
    st->SetTextColor(h->GetLineColor());
    gPad->Update();
  }
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- Chamber efficiency
  c =  new TCanvas("c9b","Chamber efficiency",1000,500);
  c->Divide(2,1);
  c->cd(1);
  h2->SetLineColor(2);
  h2->SetMarkerColor(2);
  h2->SetFillColor(2);
  h2->SetStats(1); // activate statistics box
  h2->SetMinimum(0);
  h2->SetMarkerStyle(0);
  h=(TH1D*)(h2->DrawCopy("E1 HIST"));
  gPad->Update(); // to make sure stats box is drawn
  st = (TPaveStats*)h->FindObject("stats");
  if(st) {
    st->SetOptStat(11);
    st->SetTextColor(h->GetLineColor());
    x1=st->GetX1NDC();
    x2=st->GetX2NDC();
    y1=st->GetY1NDC();
    y2=st->GetY2NDC();
    st->SetY1NDC(0.2); //new y start position
    st->SetY2NDC(0.2+(y2-y1)); //new y end position
    gPad->Update();
  }
  h1->SetStats(1); // activate statistics box
  h1->SetMarkerStyle(0);
  h=(TH1D*)(h1->DrawCopy("E1 HIST SAMES"));	
  gPad->Update(); // to make sure stats box is drawn
  st = (TPaveStats*)h->FindObject("stats");
  if(st) {
    st->SetOptStat(11);
    st->SetTextColor(h->GetLineColor());
    st->SetY1NDC(0.2); //new y start position
    st->SetY2NDC(0.2+(y2-y1)); //new y end position
    st->SetX1NDC(x1-0.3); //new x start position
    st->SetX2NDC(x2-0.3); //new x end position
    gPad->Update();
  }
  c->cd(2);
  h4->SetLineColor(2);
  h4->SetMarkerColor(2);
  h4->SetFillColor(2);
  h4->SetStats(1); // activate statistics box
  h4->SetMinimum(0);
  h4->SetMarkerStyle(0);
  h=(TH1D*)(h4->DrawCopy("E1 HIST"));
  gPad->Update(); // to make sure stats box is drawn
  st = (TPaveStats*)h->FindObject("stats");
  if(st) {
    st->SetOptStat(11);
    st->SetTextColor(h->GetLineColor());
    y1=st->GetY1NDC();
    y2=st->GetY2NDC();
    st->SetY1NDC(0.2); //new y start position
    st->SetY2NDC(0.2+(y2-y1)); //new y end position
    gPad->Update();
  }
  h3->SetStats(1); // activate statistics box
  h3->SetMarkerStyle(0);
  h=(TH1D*)(h3->DrawCopy("E1 HIST SAMES"));	
  gPad->Update(); // to make sure stats box is drawn
  st = (TPaveStats*)h->FindObject("stats");
  if(st) {
    st->SetOptStat(11);
    st->SetTextColor(h->GetLineColor());
    st->SetY1NDC(0.2); //new y start position
    st->SetY2NDC(0.2+(y2-y1)); //new y end position
    st->SetX1NDC(x1-0.3); //new x start position
    st->SetX2NDC(x2-0.3); //new x end position
    gPad->Update();
  }
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- Chamber efficiency per RUN
  c = new TCanvas("c10","Chamber efficiency per run",1000,500);
  c->Divide(2,1);
  c->cd(1);
  h=(TH1D*)(gROOT->FindObject("hEffRun"));  
  h->DrawCopy();
  c->cd(2);
  g=(TGraphErrors*)(gROOT->FindObject("hGraphRun"));  
  g->SetTitle("Chamber eff vs run;run;efficiency");
  g->GetXaxis()->SetNoExponent(true);
  g->GetXaxis()->SetNdivisions(505);
  g->SetMarkerStyle(25);
  g->SetMarkerColor(2);
  g->SetMinimum(0.);
  g->Draw("Paint E");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");

  //---- Chamber efficiency per LS
  c = new TCanvas("c11","Chamber efficiency per LS",1000,500);
  c->Divide(2,1);
  c->cd(1);
  h=(TH1D*)(gROOT->FindObject("hEffLumi"));  
  h->DrawCopy();
  c->cd(2);
  g=(TGraphErrors*)(gROOT->FindObject("hGraphLumi"));  
  g->SetTitle("Chamber eff vs LS;run+LS/1000;efficiency");
  g->GetXaxis()->SetNoExponent(true);
  g->GetXaxis()->SetNdivisions(505);
  g->SetMarkerStyle(25);
  g->SetMarkerColor(2);
  g->SetMinimum(0.);
  g->Draw("Paint E");
  f->cd("Eff"); c->Write(); f->cd(); file->cd();
  c->Print( outfile + "_" + c->GetName() + ".eps");
  c->Print( outfile + "_" + c->GetName() + ".gif");


  file->Close();
  f->Close();

  return;
}


//========== MAIN ==============

void drawEff(const TString dir="./", 
	     const TString runrange="",
	     const TString dataset="",
	     const TString prefix="eff") {

  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();

  cout << dir << endl;
  cout << runrange << endl;
  cout << dataset << endl;
  cout << prefix << endl;

  eff(dir + 
      "/efficiencyHistos.root",
      prefix,
      runrange, // "139096-140180, no #mu",// "Data corresponds"
      dataset   // "ExpressPhysics"        // "  to XX nb^{-1}"
     );
  return;
}


