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

TCanvas* pTimingL1DistFor(const std::string & opt)
{
  std::string timing  = "TimingL1_";
  const unsigned int ntags = 6; 
  std::string tags[ntags] = {"GmtEmu","Rpc","Dt","Csc","Gmt","RpcEmu"};
  TCanvas* c = new TCanvas( ("c"+timing+"DistFor_"+opt).c_str(),(timing+"DistFor_"+opt).c_str() ,600,600);
  c->SetLogy();
  c->SetGrid(0,1);
  TLegend l(0.7, 0.69, 0.93,.89);
  bool first = true;
  for (unsigned int itag=0;  itag<ntags; ++itag) {
    string name = "h"+timing+tags[itag]; 
    TH1D* h = 0;
    if (opt=="merged") {
      TH1D* h1 = (TH1D*)gROOT->FindObject( (name+"_L1PT16_MuPT0").c_str() );
      TH1D* h2 = (TH1D*)gROOT->FindObject( (name+"_L1PT20_MuPT0").c_str() );
      TH1D* h3 = (TH1D*)gROOT->FindObject( (name+"_L1PT25_MuPT0").c_str() );
      h = (TH1D*) h1->Clone((name+"merged").c_str());
      h->Add(h2,1.);
      h->Add(h3,1.);
    } else {
      name = name+"_"+opt;
      h = (TH1D*)gROOT->FindObject( name.c_str() );
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
      h->SetMaximum(8.95);
      h->SetMinimum(1.e-6);
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
    l.AddEntry(h, tags[itag].c_str());
  }
  l.DrawClone();
  return c;
}


TCanvas* pTimingL1FireBX(int bx)
{

  std::string timing  = "TimingL1_";
  const unsigned int ntags = 6;
  std::string tags[ntags] = {"GmtEmu","Rpc","Dt","Csc","Gmt","RpcEmu"};
//  TH1D * harray[ntags] = {0};
  std::stringstream sCanvas; sCanvas <<"c"<<timing<<"FireBX"<<bx;
  TCanvas* c = new TCanvas( sCanvas.str().c_str(), sCanvas.str().c_str(),600,600);
  c->SetLogy();
  c->SetLogx();
  c->SetGrid(0,1);
  TLegend l(0.7, 0.69, 0.93,.89);
  TH1D hDummy("hDummy","hDummy",100,2.,100.);
  hDummy.SetMinimum(1.e-5);
  hDummy.SetMaximum(1.);
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
      str <<"h"<<timing <<tags[itag] <<"_L1PT"<<pt<<"_MuPT0";
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
    l.AddEntry(h,tags[itag].c_str());
    harr.Add(h);
  }
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
  TH1D hDummy("hDummy","hDummy",100,2.,100.);
  hDummy.SetMinimum(1.e-6);
  hDummy.SetMaximum(1.);
  hDummy.DrawCopy();
  TObjArray harr;
  for (unsigned int idx=0; idx < bxes.size(); idx++) {
    int bx = bxes[idx];
    TH1D* h = (TH1D*)gROOT->FindObject("hPrefire")->Clone( ("hPrefire"+tag).c_str() ); 
    for (int ibin=1; ibin <=  h->GetNbinsX(); ibin++) {
      double pt = h->GetBinLowEdge(ibin) ;
      std::stringstream str;
      str <<"h"<<timing <<tag <<"_L1PT"<<pt<<"_MuPT16";
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
  l.SetHeader(tag.c_str());
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
  h->GetXaxis()->SetTitle("First RPC candidate arrival [BX]");
  if (what=="RpcVsDt") h->GetYaxis()->SetTitle("First DT  candidate arrical [BX]");
  if (what=="RpcVsCsc") h->GetYaxis()->SetTitle("First CSC  candidate arrical [BX]");

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
  h_Rpc->DrawCopy();
  h_Csc->DrawCopy("same");
  h_Dt->DrawCopy("same");
  if (h_Gmt) h_Gmt->DrawCopy("same");

  TLegend l(0.6, 0.69, 0.93,.89);

  stringstream str; int integral;
  str.str(""); integral = h_Rpc->GetEntries(); str << "RPC (ev: "<<std::setprecision(3)<< (double) integral<<")"; l.AddEntry(h_Rpc, str.str().c_str());
  str.str(""); integral = h_Dt->GetEntries();  str << " DT (ev: "<<std::setprecision(3)<< (double)integral<<")"; l.AddEntry(h_Dt, str.str().c_str());
  str.str(""); integral = h_Csc->GetEntries(); str << "CSC (ev: "<<std::setprecision(3)<< (double)integral<<")"; l.AddEntry(h_Csc, str.str().c_str());
  if (h_Gmt) {
    str.str(""); integral = h_Gmt->Integral(); str << "GMT (ev: "<<std::setprecision(3)<< (double)integral<<")"; l.AddEntry(h_Gmt, str.str().c_str());
  }
  l.DrawClone("same");

  return c;
}



void plotsTimingL1()
{
/*
  utilsPlotsSaver(pTimingL1DR());

  utilsPlotsSaver(pTimingL1("Rpc"));   
  utilsPlotsSaver(pTimingL1("Dt"));   
  utilsPlotsSaver(pTimingL1("Csc"));   

  utilsPlotsSaver(pTimingL1DistFor("DR9999"));
  utilsPlotsSaver(pTimingL1DistFor("DR0.5"));
  utilsPlotsSaver(pTimingL1DistFor("DR0.3"));
  utilsPlotsSaver(pTimingL1DistFor("DR0.2"));
  utilsPlotsSaver(pTimingL1DistFor("DR0.1"));
  utilsPlotsSaver(pTimingL1DistFor("DR0.06"));

  utilsPlotsSaver(pTimingL1DistFor("L1PT0.1_MuPT0"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT5_MuPT0"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT7_MuPT0"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT12_MuPT0"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT16_MuPT0"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT20_MuPT0"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT25_MuPT0"));

  utilsPlotsSaver(pTimingL1DistFor("L1PT0.1_MuPT16"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT5_MuPT16"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT7_MuPT16"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT12_MuPT16"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT16_MuPT16"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT20_MuPT16"));
  utilsPlotsSaver(pTimingL1DistFor("L1PT25_MuPT16"));

  utilsPlotsSaver(pTimingL1DistFor("merged"));

  utilsPlotsSaver(pTimingL1FireBX(-2));
  utilsPlotsSaver(pTimingL1FireBX(-1));
  utilsPlotsSaver(pTimingL1FireBX(1));
  utilsPlotsSaver(pTimingL1FireBX(2));
*/

/*
*/
  std::vector<int> bxes; 
  bxes.push_back(-1); bxes.push_back(1);bxes.push_back(2);  
  utilsPlotsSaver(pTimingL1FirePrePost("GmtEmu", bxes));
  utilsPlotsSaver(pTimingL1FirePrePost("Dt", bxes));
  utilsPlotsSaver(pTimingL1FirePrePost("Csc", bxes));
  utilsPlotsSaver(pTimingL1FirePrePost("RpcEmu", bxes));

/*
  utilsPlotsSaver(pTimingL1Corr("RpcVsDt","Open"));
  utilsPlotsSaver(pTimingL1Corr("RpcVsCsc","Open"));
  utilsPlotsSaver(pTimingL1Corr("RpcVsDt","Tight"));
  utilsPlotsSaver(pTimingL1Corr("RpcVsCsc","Tight"));
*/

//  utilsPlotsSaver(pTimingL1Gmt());
//  utilsPlotsSaver(pTimingL1RMS());
}
