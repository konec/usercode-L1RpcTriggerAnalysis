#include "TROOT.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TStyle.h"
#include "TText.h"
#include "TLatex.h"
#include "TString.h"
#include "TGaxis.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TVirtualPad.h"
#include "TLegend.h"
#include "TObjArray.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <map>
#include <vector>
#include "utilsPlotsSaver.h"

TCanvas* pTimingL1RMS()
{
  TCanvas* c = new TCanvas("cTimingL1_RMS","cTimingL1_RMS",600,600);
  c->SetLogx();
  //TProfile* h = (TProfile*)gROOT->FindObject("hTimingL1_Prof");
  TProfile* h = (TProfile*)gROOT->FindObject("hTimingL1_RMS");
  for (int ibin=1; ibin<=h->GetNbinsX(); ibin++) {
    h->SetBinError(ibin, 1.e-5);
  }
  h->SetLineWidth(2);
  h->SetLineColor(4);
  h->SetMarkerStyle(23);
  h->SetMarkerSize(0.5);
  h->Print("all");
  h->GetXaxis()->SetTitle("Muon pT");
  h->GetYaxis()->SetTitle("RMS");
  h->DrawCopy("E0");

  
  return c;
}

TCanvas* pTimingL1Gmt()
{
  std::string timing = "TimingL1_";
  TCanvas* c = new TCanvas("cTimingL1_Gmt","cTimingL1_Gmt",1000,500);
  c->Divide(2,1);
  
  for (int i=1;i<=2;++i) { 
  TVirtualPad *p = c->cd(i); p->SetLogy(1); p->SetGrid(0,1);
  TH1D* h = (i==1) ? (TH1D*)gROOT->FindObject("hTimingL1_GmtAll") : (TH1D*)gROOT->FindObject("hTimingL1_Gmt");
  double atPeak = h->GetBinContent(3);
  h->Scale(1./atPeak);
  h->SetMaximum(1.95);
  h->SetMinimum(1.e-7);
  h->SetLineWidth(2);
  h->SetMarkerStyle(1);
  h->SetLineColor(4);
  h->SetFillColor(4);
  h->SetFillStyle(3003);
  h->GetXaxis()->SetNdivisions(5);
  h->GetXaxis()->SetTitle("Gmt timing [BX]");
  h->GetYaxis()->SetTitle("Fraction of triggers");
  h->DrawCopy("hist text");

  TLatex t;
  t.SetNDC(true);t.SetTextSize(0.05); t.SetTextColor(2); 
  string txt = (i==1) ? "ALL" : "SELECTED";
  t.SetText(0.2, 0.8, txt.c_str()); t.DrawClone();
  }

  return c;
}

TCanvas* pTimingL1(const std::string& what)
{
  std::string timing = "TimingL1_";
  TCanvas* c = new TCanvas( ("c"+timing+what).c_str(),(timing+what).c_str() ,600,600);
  c->SetLogy();
  c->SetGrid(0,1);

  TH1D* h = (TH1D*)gROOT->FindObject( ("h"+timing+what).c_str() );
  double atPeak = h->GetBinContent(3);
  h->Scale(1./atPeak);
  h->SetMaximum(1.95);
  h->SetMinimum(1.e-5);
  h->SetLineWidth(2);
  h->SetMarkerStyle(1);
  h->SetLineColor(4);
  h->SetFillColor(4);
  h->SetFillStyle(3003);
  h->GetXaxis()->SetNdivisions(5);
  h->GetXaxis()->SetTitle("First candidate arrival [BX]");
  h->GetYaxis()->SetTitle("Fraction of triggers");

  h->DrawCopy("hist text");

  TLatex t;
  t.SetNDC(true);t.SetTextSize( 0.05); t.SetTextColor(2); 
  t.SetText(0.2, 0.8, what.c_str()); t.DrawClone();

  return c;
}

TCanvas* pTimingL1BXDist_L1PerformancePaper()
{
  std::string timing  = "TimingL1_";
  const unsigned int ntags = 6; 
  std::string tags[ntags] =      {"GmtEmu", "Gmt", "Rpc", "RpcEmu", "Dt","Csc"};
  std::string tagnames[ntags] =  {"GMT","GMT HSCP","RPC HSCP", "RPC 1BX", "DT","CSC"};
  TCanvas* c = new TCanvas( "cTimingL1BXDist_L1PerformancePaper","cTimingL1BXDist_L1PerformancePaper",600,600);
  c->SetLogy();
  c->SetGrid(0,1);
  TLegend l(0.65, 0.63, 0.93,.89);
  TH1D* hReference =  (TH1D*)gROOT->FindObject("hTimingL1_Gmt");
  double atPeakReference = hReference->GetBinContent(3);
  bool first = true;
  for (unsigned int itag=0;  itag<ntags; ++itag) {
    string name = "h"+timing+tags[itag]; 
    TH1D* h = 0;
    TH1D* h1 =  (TH1D*)gROOT->FindObject( name.c_str() );
    if (!h1) { std::cout <<"no histo!"<< std::endl; continue; }
    h = (TH1D*) h1->Clone( (name+"tmp").c_str() );
    h->Scale(1./atPeakReference);
    int color = 1; int style = 1; double size = 1.;
    if (tags[itag]=="Gmt")     {color = kBlack;   style = 23; size = 1.3;}
    if (tags[itag]=="GmtEmu") { color = kGray+1;  style =  1; size = 1.;}
    if (tags[itag]=="Rpc")     {color = kCyan+1;  style = 22; size = 1.3;}
    if (tags[itag]=="RpcEmu") { color = kBlue;    style = 26; size = 2.;}
    if (tags[itag]=="Dt")     { color = kRed;     style = 25; size = 2.;}
    if (tags[itag]=="Csc")    { color = kGreen+1; style = 24; size = 2.;}
    h->SetLineColor(color);
    if (first) {
      first = false;
      h->SetMaximum(2.95);
// h->SetMaximum(1.e5);
      h->SetMinimum(5.e-7);
// h->SetMinimum(0.5);
      h->GetXaxis()->SetNdivisions(5);
      h->GetXaxis()->SetTitle("L1 candidate arrival [BX]");
      h->GetYaxis()->SetTitle("Fraction of triggers");
      h->SetLineWidth(2);
      h->SetMarkerStyle(1);
      h->SetMarkerSize(0.1);
      h->SetFillColor(color);
      h->SetFillStyle(3003);
      h->DrawCopy("h");
    } else {
      h->SetMarkerSize(size);
      h->SetMarkerStyle(style);
      h->SetMarkerColor(color);
      h->DrawCopy("same");
    }
    l.AddEntry(h, tagnames[itag].c_str());
  }
  l.DrawClone();
  return c;
}

TCanvas* pTimingL1DistFor(const std::string & opt,const std::string & bxopt ="")
{
  std::string timing  = "TimingL1_";
  const unsigned int ntags = 6; 
  std::string tags[ntags] =      {"GmtEmu", "Gmt",     "Dt","Csc","RpcEmu", "Rpc"};
  std::string tagnames[ntags] =  {"GMT","GMT HSCP","DT","CSC","RPC 1BX","RPC HSCP"};
/*
  std::string tags[ntags] =      {"GmtEmu","Rpc","Dt","Csc","Gmt","RpcEmu"};
  std::string tagnames[ntags] =  {"Gmt 1BX","Rpc HSCP","Dt","Csc","Gmt HSCP","Rpc 1BX"};
*/
  TCanvas* c = new TCanvas( ("c"+timing+"DistFor_"+opt+bxopt).c_str(),(timing+"DistFor_"+opt+bxopt).c_str() ,600,600);
  c->SetLogy();
  c->SetGrid(0,1);
  TLegend l(0.65, 0.63, 0.93,.89);
  bool first = true;
  for (unsigned int itag=0;  itag<ntags; ++itag) {
    string name = "h"+timing+tags[itag]; 
    TH1D* h = 0;
    if (opt=="merged") {
      TH1D* h1 = (TH1D*)gROOT->FindObject( (name+"_L1PT16_MuPT0"+bxopt).c_str() );
      TH1D* h2 = (TH1D*)gROOT->FindObject( (name+"_L1PT20_MuPT0"+bxopt).c_str() );
      TH1D* h3 = (TH1D*)gROOT->FindObject( (name+"_L1PT25_MuPT0"+bxopt).c_str() );
      h = (TH1D*) h1->Clone((name+"merged"+bxopt).c_str());
      h->Add(h2,1.);
      h->Add(h3,1.);
    } else if (opt=="combined") {
      TH1D* h01 = (TH1D*)gROOT->FindObject( (name+"_L1PT16_MuPT0_noBXm2").c_str() );
      TH1D* h02 = (TH1D*)gROOT->FindObject( (name+"_L1PT20_MuPT0_noBXm2").c_str() );
      TH1D* h03 = (TH1D*)gROOT->FindObject( (name+"_L1PT25_MuPT0_noBXm2").c_str() );
      TH1D* hm2 = (TH1D*) h01->Clone( (name+"combined_m2").c_str());
      hm2->Add(h02,1.);
      hm2->Add(h03,1.);
      hm2->Scale(1./hm2->GetBinContent(3));
      TH1D* h21 = (TH1D*)gROOT->FindObject( (name+"_L1PT16_MuPT0_noBXp2").c_str() );
      TH1D* h22 = (TH1D*)gROOT->FindObject( (name+"_L1PT20_MuPT0_noBXp2").c_str() );
      TH1D* h23 = (TH1D*)gROOT->FindObject( (name+"_L1PT25_MuPT0_noBXp2").c_str() );
      TH1D* hp2 = (TH1D*) h21->Clone( (name+"combined_p2").c_str());
      hp2->Add(h22,1.);
      hp2->Add(h23,1.);
      hp2->Scale(1./hp2->GetBinContent(3));
      TH1D* h11 = (TH1D*)gROOT->FindObject( (name+"_L1PT16_MuPT0").c_str() );
      TH1D* h12 = (TH1D*)gROOT->FindObject( (name+"_L1PT20_MuPT0").c_str() );
      TH1D* h13 = (TH1D*)gROOT->FindObject( (name+"_L1PT25_MuPT0").c_str() );
      h = (TH1D*) h11->Clone( (name+"combined").c_str());
      h->Add(h12,1.);
      h->Add(h13,1.);
      h->Scale(1./h->GetBinContent(3));
      h->SetBinContent(1, hm2->GetBinContent(1));
      h->SetBinError(1, hm2->GetBinError(1));
      h->SetBinContent(5, hp2->GetBinContent(5));
      h->SetBinError(5, hp2->GetBinError(5));
    } else {
      name = name+"_"+opt+bxopt;
      TH1D* h1 =  (TH1D*)gROOT->FindObject( name.c_str() );
      h = (TH1D*) h1->Clone( (name+"tmp").c_str() );
    }
    if (!h) { std::cout <<"no histo!"<< std::endl; continue; }
    double atPeak = h->GetBinContent(3);
    h->Scale(1./atPeak);
    int color = 1;
    if (tags[itag]=="Rpc") color = kBlue+1;
    if (tags[itag]=="RpcEmu") color = kCyan;
    if (tags[itag]=="GmtEmu") color = kGray+1;
    if (tags[itag]=="Dt") color = kRed;
    if (tags[itag]=="Csc") color = kGreen+1;
    h->SetLineColor(color);
    if (first) {
      first = false;
      h->SetMaximum(2.95);
// h->SetMaximum(1.e5);
      h->SetMinimum(1.e-6);
// h->SetMinimum(0.5);
      h->GetXaxis()->SetNdivisions(5);
      h->GetXaxis()->SetTitle("L1 candidate arrival [BX]");
      h->GetYaxis()->SetTitle("Fraction of triggers");
      h->SetLineWidth(2);
      h->SetMarkerStyle(1);
      h->SetFillColor(color);
      h->SetFillStyle(3003);
      h->DrawCopy("h");
    } else {
      h->SetMarkerStyle(20+itag);
      h->SetMarkerColor(color);
      h->DrawCopy("same");
    }
    l.AddEntry(h, tagnames[itag].c_str());
  }
  l.DrawClone();
  return c;
}


TCanvas* pTimingL1FireBX(int bx)
{

  std::string timing  = "TimingL1_";
//  std::string tags[ntags] = {"GmtEmu","Rpc","Dt","Csc","Gmt","RpcEmu"};
  std::string tags[6] = {"GmtEmu","Dt","Csc","RpcEmu","Gmt","Rpc"};
  std::string tagnames[6] = {"GMT","DT","CSC","RPC 1BX","GMT HSCP","RPC HSCP"};
  const unsigned int ntags = 4;
//  TH1D * harray[ntags] = {0};
  std::stringstream sCanvas; sCanvas <<"c"<<timing<<"FireBX"<<bx;
  TCanvas* c = new TCanvas( sCanvas.str().c_str(), sCanvas.str().c_str(),600,600);
  c->SetLogy();
  c->SetLogx();
  c->SetGrid(0,1);
  TLegend l(0.65, 0.69, 0.93,.89);
  TH1D hDummy("hDummy","hDummy",100,2.,100.);
  hDummy.SetMinimum(1.e-6);
  hDummy.SetMaximum(1.);
  hDummy.SetXTitle("L1 Candidate p_{T} [GeV/c]");
  hDummy.SetYTitle("Fraction of events");
  hDummy.DrawCopy();
  TObjArray harr;
  for (unsigned int itag=0; itag < ntags; itag++) {
//    TH1D *h = harray[itag];
    std::stringstream myname; myname << "hPrefire_BX"<<bx<<"_"<<tags[itag];
    TH1D* h = (TH1D*)gROOT->FindObject("hPrefire")->Clone(myname.str().c_str()); 
//    h->SetDirectory(0);
    for (int ibin=1; ibin <=  h->GetNbinsX(); ibin++) {
      double pt = h->GetBinLowEdge(ibin) ;
      std::stringstream str;
      str <<"h"<<timing <<tags[itag] <<"_L1PT"<<pt<<"_MuPT0"; //<<"_noBXp2";
      TH1D* hPT = (TH1D*)gROOT->FindObject( str.str().c_str() );
      double valBX = hPT->GetBinContent( hPT->FindBin(bx) );
      double valBXe = hPT->GetBinError( hPT->FindBin(bx) );
      double valBX0  = std::max(1., hPT->GetBinContent( hPT->FindBin(0.) ) );
      h->SetBinContent(ibin, valBX/valBX0);
      h->SetBinError(ibin,  valBXe/valBX0);
//      std::cout <<str.str() << "pref: "<<valBX<<" BX0: "<<valBX0<<  std::endl;
    }
    int color = 1;
    if (tags[itag]=="Rpc") color = kBlue+1;
    if (tags[itag]=="RpcEmu") color = kBlue;
    if (tags[itag]=="GmtEmu") color = kGray+1;
    if (tags[itag]=="Dt") color = kRed;
    if (tags[itag]=="Csc") color = kGreen+1;
    h->SetLineColor(color);
    h->SetMarkerStyle(20+itag);
    h->SetMarkerColor(color);
    h->DrawCopy("same");
    l.AddEntry(h,tagnames[itag].c_str());
    harr.Add(h);
  }
  l.DrawClone();
  harr.Delete();
  return c;

}

TCanvas* pTimingL1FirePrePost_L1PerformancePaper()
{
  TCanvas* c = new TCanvas("cTimingL1FirePrePost_L1PerformancePaper",
                           "cTimingL1FirePrePost_L1PerformancePaper",600,600);


  c->SetLogy();
  c->SetLogx();
  c->SetGrid(0,1);
  TLegend l(0.7, 0.69, 0.93,.89);
  TH1D hDummy("hDummy","hDummy",100,1.5,160.);
  hDummy.SetMinimum(1.e-6); hDummy.SetMaximum(1.);
//  hDummy.SetMinimum(0.6); hDummy.SetMaximum(1.e7);
  hDummy.SetXTitle("L1 Candidate p_{T} [GeV/c]");
  hDummy.SetYTitle("Fraction of triggers");
  hDummy.DrawCopy();
  TObjArray harr;
  TDirectory *dBefore = gDirectory;
  TH1D* hPre, *hPost;

  //
  // pre
  //
  {
    TFile fPre("../l1RpcAnalysis_all.root");
    TH1D* hD = (TH1D*) fPre.Get("hTimingL1_preFire_GmtEmu_D");
    TH1D* hN = (TH1D*) fPre.Get("hTimingL1_preFire_GmtEmu");
     hD->SetLineColor(kRed);
     hN->SetLineColor(kRed);
//    hD->DrawCopy("same");
//    hN->DrawCopy("same");
//    TH1D* hD =  (TH1D*)gROOT->FindObject("hTimingL1_preFire_GmtEmu_D");
//    TH1D* hN =  (TH1D*)gROOT->FindObject("hTimingL1_preFire_GmtEmu");
    hD->Sumw2();
    hN->Sumw2();
    hPre = (TH1D*)hD->Clone("hPre"); 
    hPre->SetDirectory(0); 
    hPre->Reset();
    hPre->Divide(hN,hD,1.,1.,"B");
    hPre->SetLineColor(kRed);
    hPre->SetLineStyle(1);
    hPre->SetLineWidth(2);
    hPre->SetMarkerStyle(24);
    hPre->SetMarkerSize(1.6);
    hPre->SetMarkerColor(kRed);
    l.AddEntry(hPre, "pre-Fire");
    harr.Add(hPre);
    hPre->DrawCopy("same e ");
  }
  dBefore->cd();


  //
  // post
  //
  {
    TFile fPost("../l1RpcAnalysis_sel.root");
    TH1D* hD = (TH1D*) fPost.Get("hTimingL1_postFire_GmtEmu_D");
    TH1D* hN = (TH1D*) fPost.Get("hTimingL1_postFire_GmtEmu");
    hD->Sumw2();
    hN->Sumw2();
    hPost = (TH1D*)hD->Clone("hPost"); hPost->SetDirectory(0); hPost->Reset();
    hPost->Divide(hN,hD,1.,1.,"B");
    hPost->SetLineColor(kBlue);
    hPost->SetLineStyle(1);
    hPost->SetLineWidth(2);
    hPost->SetMarkerStyle(25);
    hPost->SetMarkerSize(1.6);
    hPost->SetMarkerColor(kBlue);
    l.AddEntry(hPost, "post-Fire");
    harr.Add(hPost);
    hPost->DrawCopy("same e ");
  }
  dBefore->cd();

  l.SetHeader("GMT:");
  l.DrawClone();
  harr.Delete();
  return c;
} 


TCanvas* pTimingL1FirePrePost(const std::string & tag, const std::vector<int> & bxes)
{

  std::string timing  = "TimingL1_";
  std::stringstream sCanvas; sCanvas <<"c"<<timing<<"FirePrePost"<<tag;
  TCanvas* c = new TCanvas( sCanvas.str().c_str(), sCanvas.str().c_str(),600,600);
  c->SetLogy();
  c->SetLogx();
  c->SetGrid(0,1);
  TLegend l(0.7, 0.69, 0.93,.89);
  TH1D hDummy("hDummy","hDummy",100,1.,200.);
  hDummy.SetMinimum(1.e-6);
  hDummy.SetMaximum(1.);
  hDummy.SetXTitle("L1 Candidate p_{T} [GeV/c]");
  hDummy.SetYTitle("fraction of events");
  hDummy.DrawCopy();
  TObjArray harr;
  for (unsigned int idx=0; idx < bxes.size(); idx++) {
    int bx = bxes[idx];
    TH1D* h = (TH1D*)gROOT->FindObject("hPrefire")->Clone( ("hPrefire"+tag).c_str() ); 
    for (int ibin=1; ibin <=  h->GetNbinsX(); ibin++) {
      double pt = h->GetBinLowEdge(ibin) ;
      std::stringstream str;
      str <<"h"<<timing <<tag <<"_L1PT"<<pt<<"_MuPT0";
      TH1D* hPT = (TH1D*)gROOT->FindObject( str.str().c_str() );
      double valBX = hPT->GetBinContent( hPT->FindBin(bx) );
      double valBXe = hPT->GetBinError( hPT->FindBin(bx) );
      double valBX0  = std::max(1., hPT->GetBinContent( hPT->FindBin(0.) ) );
      h->SetBinContent(ibin, valBX/valBX0);
      h->SetBinError(ibin,  valBXe/valBX0);
//      std::cout <<str.str() << "pref: "<<valBX<<" BX0: "<<valBX0<<  std::endl;
    }
    int color = 1;
    if (tag=="Rpc") color = kBlue+1;
    if (tag=="RpcEmu") color = kBlue;
    if (tag=="GmtEmu") color = kGray+1;
    if (tag=="Dt") color = kRed;
    if (tag=="Csc") color = kGreen+1;
    h->SetLineColor(color);
    int style = 1;
    if (bx == 1 ) style = 2;
    if (bx == 2 ) style = 3;
    h->SetLineColor(color);
    h->SetLineStyle(style);
    h->SetLineWidth(2);
    h->SetMarkerStyle(23+bx);
    h->SetMarkerColor(color);
    h->DrawCopy("same");
    std::stringstream ss; ss << "BX = "<<bx;
    l.AddEntry(h, ss.str().c_str());
    harr.Add(h);
  }
  string ctag = tag;
  if (tag=="GmtEmu") ctag="GMT:";
  l.SetHeader(ctag.c_str());
  l.DrawClone();
  harr.Delete();
  return c;

}

TCanvas* pTimingL1Corr(const std::string& what, const std::string& opt)
{
  std::string timing = "TimingL1_";
  TCanvas* c = new TCanvas( ("c"+timing+what+opt).c_str(),(timing+what+opt).c_str() ,600,600);
  TH2D* h = (TH2D*)gROOT->FindObject( ("h"+timing+what+opt).c_str() );
  h->SetLineWidth(2);
  h->SetLineColor(4);
  h->GetXaxis()->SetNdivisions(5);
  h->GetYaxis()->SetNdivisions(5);
  h->GetXaxis()->SetTitle("RPC candidate arrival [BX]");
  if (what=="RpcVsDt") h->GetYaxis()->SetTitle("DT  candidate arrical [BX]");
  if (what=="RpcVsCsc") h->GetYaxis()->SetTitle("CSC  candidate arrical [BX]");

  //c->SetLogz();
  h->DrawCopy("box text");

  TLatex t;
  t.SetNDC(true);t.SetTextSize( 0.05); t.SetTextColor(2);
  t.SetText(0.2, 0.76, what.c_str()); t.DrawClone();
  t.SetText(0.2, 0.71, ("'"+opt+"'").c_str()); t.DrawClone();

  return c;
}

TCanvas* pTimingL1DR()
{
  TCanvas* c = new TCanvas("cTimingL1DR","cTimingL1DR",600,600);
  c->SetLogy();
  c->SetGrid(0,1);

  TH1D* h_Csc = (TH1D*)gROOT->FindObject("hTimingL1_DR_Csc");
  TH1D* h_Rpc = (TH1D*)gROOT->FindObject("hTimingL1_DR_Rpc");
  TH1D* h_Dt = (TH1D*)gROOT->FindObject( "hTimingL1_DR_Dt");
  TH1D* h_Gmt = (TH1D*)gROOT->FindObject("hTimingL1_DR_Gmt");
  double max = h_Rpc->GetMaximum();
  double m1 = h_Csc->GetMaximum();
  double m3 = h_Dt->GetMaximum();
  if (m3 > max) max = m3;
  if (m1 > max) max = m1;
  
  h_Rpc->SetMinimum(0.8);
  h_Rpc->SetLineColor(2);
  h_Csc->SetLineColor(8);
  h_Dt->SetLineColor(4);
  if (h_Gmt) h_Gmt->SetLineColor(1);

  h_Rpc->SetXTitle("#DeltaR");
  h_Rpc->SetYTitle("# events");
  h_Rpc->SetMaximum(max*1.5);
  h_Rpc->DrawCopy();
  h_Csc->DrawCopy("same");
  h_Dt->DrawCopy("same");
  if (h_Gmt) h_Gmt->DrawCopy("same");

  TLegend l(0.6, 0.69, 0.93,.89);

  stringstream str; double integral;
  str.str(""); integral = h_Rpc->GetEntries(); str << "RPC (ev: "<<std::setprecision(3)<< integral<<")"; l.AddEntry(h_Rpc, str.str().c_str());
  str.str(""); integral = h_Dt->GetEntries();  str << " DT (ev: "<<std::setprecision(3)<< integral<<")"; l.AddEntry(h_Dt, str.str().c_str());
  str.str(""); integral = h_Csc->GetEntries(); str << "CSC (ev: "<<std::setprecision(3)<< integral<<")"; l.AddEntry(h_Csc, str.str().c_str());
  if (h_Gmt) {
    str.str(""); integral = h_Gmt->Integral(); str << "GMT (ev: "<<std::setprecision(3)<< integral<<")"; l.AddEntry(h_Gmt, str.str().c_str());
  }
  l.DrawClone("same");

  return c;
}



void plotsTimingL1()
{
  utilsPlotsSaver(pTimingL1DR());
/*

  utilsPlotsSaver(pTimingL1("Rpc"));   
  utilsPlotsSaver(pTimingL1("Dt"));   
  utilsPlotsSaver(pTimingL1("Csc"));   
*/

/*
  utilsPlotsSaver(pTimingL1DistFor("DR9999"));
  utilsPlotsSaver(pTimingL1DistFor("DR0.5"));
  utilsPlotsSaver(pTimingL1DistFor("DR0.3"));
  utilsPlotsSaver(pTimingL1DistFor("DR0.2"));
  utilsPlotsSaver(pTimingL1DistFor("DR0.1"));
  utilsPlotsSaver(pTimingL1DistFor("DR0.06"));

  utilsPlotsSaver(pTimingL1DistFor("L1PT0.1_MuPT0"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT4_MuPT0"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT6_MuPT0"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT10_MuPT0"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT16_MuPT0"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT25_MuPT0"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT50_MuPT0"));
*/


//  utilsPlotsSaver(pTimingL1DistFor("merged","_noBXm2"));
//  utilsPlotsSaver(pTimingL1DistFor("merged"));
//  utilsPlotsSaver(pTimingL1DistFor("merged","_noBXp2"));
//  utilsPlotsSaver(pTimingL1DistFor("combined"));



/*
*/
//  utilsPlotsSaver(pTimingL1FireBX(-2));
//  utilsPlotsSaver(pTimingL1FireBX(-1));
//  utilsPlotsSaver(pTimingL1FireBX(1));
//  utilsPlotsSaver(pTimingL1FireBX(2));

/*
  std::vector<int> bxes; 
  bxes.push_back(-1); bxes.push_back(1); //bxes.push_back(2);  
  utilsPlotsSaver(pTimingL1FirePrePost("GmtEmu", bxes));
  utilsPlotsSaver(pTimingL1FirePrePost_L1PerformancePaper());
*/
  utilsPlotsSaver(pTimingL1BXDist_L1PerformancePaper());

/*
  utilsPlotsSaver(pTimingL1FirePrePost("Dt", bxes));
  utilsPlotsSaver(pTimingL1FirePrePost("Csc", bxes));
  utilsPlotsSaver(pTimingL1FirePrePost("RpcEmu", bxes));

  utilsPlotsSaver(pTimingL1Corr("RpcVsDt","Open"));
  utilsPlotsSaver(pTimingL1Corr("RpcVsCsc","Open"));
  utilsPlotsSaver(pTimingL1Corr("RpcVsDt","Tight"));
  utilsPlotsSaver(pTimingL1Corr("RpcVsCsc","Tight"));
*/

//  utilsPlotsSaver(pTimingL1Gmt());
//  utilsPlotsSaver(pTimingL1RMS());
}
