#ifndef plotsRpcVsOth_C
#define plotsRpcVsOth_C

#include <iomanip>
#include <sstream>
#include "TROOT.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TVirtualPad.h"
#include "TText.h"
#include "TH1D.h"
#include "TH2D.h"

#include "utilsPlotsSaver.C"
#include "utilsDivideErr.C"


TCanvas* pRpcVsOth_EventsStat()
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  ff->cd();

  std::string nc = "cRpcVsOth_EventsStat";
  TCanvas * c = new TCanvas(nc.c_str(),nc.c_str(),1200,400);
  
  c->Divide(3,1);
  
  for (int i=1; i<=3; i++) {
    TH1D* h;
    TVirtualPad *p = c->cd(i); p->SetLogx(1);  p->SetLogy(1); p->SetGrid(0,1);
    std::string name;
    if (i==1) name="hRpcVsOth_EventsAnyPt_Bar";
    if (i==2) name="hRpcVsOth_EventsAnyPt_Int";
    if (i==3) name="hRpcVsOth_EventsAnyPt_End";
    h=(TH1D*)gROOT->FindObject(name.c_str());
    if(!h) return 0;
    h->GetXaxis()->SetTitleOffset(1.3);
    h->GetYaxis()->SetLabelOffset(0.006);
    h->GetYaxis()->SetTitleOffset(1.3);
    h->SetStats(kFALSE);
    h->SetMinimum(0.9); h->DrawCopy();
  }
  return c;
}

void pRpcVsOth_Events_helper(TH1D* h)
{
  if(!h) return;
  for (Int_t ibin=1; ibin <= h->GetNbinsX() ;ibin++) {
    Float_t content = h->GetBinContent(ibin);
    Float_t center  = h->GetBinCenter(ibin);
    TText t; t.SetTextFont(20);t.SetTextSize(0.039);t.SetTextColor(4); 
    if (content<0.5) t.SetTextAlign(21); else t.SetTextAlign(23);
    Float_t where = (content<0.5) ? content+0.015: content-0.015;
    std::stringstream mytext; 
    mytext << setprecision(3) <<content;
    t.DrawText(center, where, mytext.str().c_str());
  }
}

TCanvas* pRpcVsOth_Events(const std::string & opt)
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  ff->cd();

  std::string nc = "cRpcVsOth_Events"+opt;
  TCanvas* c = new TCanvas(nc.c_str(),nc.c_str(),1200,400);
  c->Divide(3,1);

  std::string where[]={"_Bar","_Int","_End"};
  for (int i=1; i<=3; ++i) {
    TVirtualPad * p = c->cd(i); p->SetLogx(1); p->SetGrid(0,1);
    TH1D* hN=(TH1D*)gROOT->FindObject( ("hRpcVsOth_Events"+opt+where[i-1]).c_str());
    TH1D* hD=(TH1D*)gROOT->FindObject( ("hRpcVsOth_EventsAnyPt"+where[i-1]).c_str());
    TH1D* h = DivideErr(hN,hD,"h","B");
    if(!h) return 0;
    h->GetXaxis()->SetTitleOffset(1.3); 
    h->SetMarkerSize(1.6);
    h->SetYTitle("efficiency"); 
    h->SetStats(kFALSE);
    h->SetMinimum(0); h->SetMaximum(1.04); h->DrawCopy("");
    pRpcVsOth_Events_helper(h);
    h->Delete();
  }
  return c;
}

TCanvas* pRpcVsOth_EffCoarse(std::string opt="Rpc")
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  ff->cd();

  std::string nc = "cRpcVsOth_EffCoarse"+opt;
  TCanvas* c = new TCanvas(nc.c_str(),nc.c_str(),1200,400);
  c->Divide(3,1);
  std::string where[]={"_Bar","_Int","_End"};
  for (int i=1; i<=3; ++i) {
    TVirtualPad * p = c->cd(i); p->SetLogx(1); p->SetGrid(0,1);
    TH1D* h1=(TH1D*)gROOT->FindObject( ("hRpcVsOth_EffCoarse"+opt+where[i-1]).c_str());
    TH1D* h2=(TH1D*)gROOT->FindObject( ("hRpcVsOth_EffCoarseMu"+where[i-1]).c_str());
    TH1D* h =DivideErr(h1,h2,"h","B");
    if(!h) return 0;
    h->SetYTitle("efficiency");
    h->SetMinimum(0); h->SetMaximum(1.04);
    h->DrawCopy();
    pRpcVsOth_Events_helper(h);
    h->Delete();
  }
  return c;
}


void pRpcVsOth_QualEvLostBy_helper(TH2D* h) 
{
  if(!h) return;
  std::stringstream entr; 
  entr<<"Entries: "<<h->GetEntries();
  TText t; 
  t.SetTextColor(4); 
  t.SetTextAlign(21); 
  t.SetTextSize(0.045);
  t.DrawText(3.,7.7, entr.str().c_str());
}

TCanvas* pRpcVsOth_QualEvLostBy( const std::string & opt)
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  ff->cd();

  //  gStyle->SetOptStat(10);

  std::string nc = "cRpcVsOth_QualEvLostBy"+opt;
  TCanvas* c = new TCanvas(nc.c_str(),nc.c_str(),1200,400);
  c->Divide(3,1);
  TH2D *h;
  string name;

  c->cd(1); 
  name = "hRpcVsOth_QualEvLostBy"+opt+"_Bar";
  h=(TH2D*)gROOT->FindObject( name.c_str());
  if (!h) {
    std::cout <<" Cannot find: "<<name<<std::endl; 
    return 0;
  }
  h->SetMarkerSize(1.6); h->SetYTitle("quality DT"); h->DrawCopy("box text");
  pRpcVsOth_QualEvLostBy_helper(h);

  c->cd(2); 
  name = "hRpcVsOth_QualEvLostBy"+opt+"_Int";
  h=(TH2D*)gROOT->FindObject( name.c_str());
  if (!h) {
    std::cout <<" Cannot find: "<<name<<std::endl; 
    return 0;
  }
  h->SetMarkerSize(1.6); h->SetYTitle("quality DT/CSC"); h->DrawCopy("box text");
  pRpcVsOth_QualEvLostBy_helper(h);

  c->cd(3); 
  name = "hRpcVsOth_QualEvLostBy"+opt+"_End";
  h=(TH2D*)gROOT->FindObject( name.c_str());
  if (!h) {
    std::cout <<" Cannot find: "<<name<<std::endl; 
    return 0;
  }
  h->SetMarkerSize(1.6); h->SetYTitle("quality CSC"); h->DrawCopy("box text");
  pRpcVsOth_QualEvLostBy_helper(h);

  return c;
}

void plotsRpcVsOth()
{
  utilsPlotsSaver( pRpcVsOth_EventsStat() );
  utilsPlotsSaver( pRpcVsOth_EffCoarse("RpcPtCut") );
  utilsPlotsSaver( pRpcVsOth_EffCoarse("RpcNoCut") );
  utilsPlotsSaver( pRpcVsOth_EffCoarse("OthPtCut") );
  utilsPlotsSaver( pRpcVsOth_EffCoarse("OthNoCut") );
  utilsPlotsSaver( pRpcVsOth_Events("MinPt") );
  utilsPlotsSaver( pRpcVsOth_Events("MaxPt") );
  utilsPlotsSaver( pRpcVsOth_Events("RanPt") );
  utilsPlotsSaver( pRpcVsOth_Events("RpcPt") );
  utilsPlotsSaver( pRpcVsOth_Events("OthPt") );
  utilsPlotsSaver( pRpcVsOth_Events("AlgPt") );
  utilsPlotsSaver( pRpcVsOth_QualEvLostBy("RpcPt") );
  utilsPlotsSaver( pRpcVsOth_QualEvLostBy("OthPt") );
  utilsPlotsSaver( pRpcVsOth_QualEvLostBy("AlgPt") );
  utilsPlotsSaver( pRpcVsOth_QualEvLostBy("MinPt") );
  utilsPlotsSaver( pRpcVsOth_QualEvLostBy("MaxPt") );
}

#endif
