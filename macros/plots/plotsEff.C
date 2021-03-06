#ifndef plotsEff_C
#define plotsEff_C

#include "TROOT.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TVirtualPad.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TLine.h"

#include <vector>
#include <sstream>
#include "utilsDivideErr.C"
#include "utilsPlotsSaver.h"


TCanvas* pEff_Effic()
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  //TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  //ff->cd();

  TCanvas* c = new TCanvas("cEff_Effic","L1 RPC relative efficiency",600,600);

  TVirtualPad * p1 = c->cd(1); p1->SetLogx(1); p1->SetGrid(0,1);
  TH1D* hN1 = (TH1D*)gROOT->FindObject("hEfficRpcPtCut_N");
  TH1D* hD =  (TH1D*)gROOT->FindObject("hEfficMuPt_D");
  TH1D *h1 = DivideErr( hN1, hD, "hEffRpcPtCut","B");
  if(!h1) return 0;
  h1->GetXaxis()->SetRange(4,100);
  h1->SetXTitle("Global muon p_{T} [GeV/c]  "); 
  h1->SetYTitle("Efficiency");
  h1->SetTitle("L1 RPC relative efficiency");
  h1->SetMaximum(0); 
  h1->SetMaximum(1.04); 
  h1->SetMarkerStyle(23);
  h1->SetMarkerColor(2);
  h1->DrawCopy();
 
  TH1D* hN2 = (TH1D*)gROOT->FindObject("hEfficRpcNoCut_N");
  TH1D * h2 = DivideErr(hN2, hD, "hEffRpcNoPtCut","B");
  if(!h2) return 0;
  h2->SetMarkerStyle(25);
  h2->SetMarkerColor(4);

  TLegend l(0.6, 0.29, 0.93,.42); 
  l.AddEntry(h2,"no p_{T} cut");
  l.AddEntry(h1,"p_{T}^{L1Rpc} #geq 16 GeV/c");
  l.DrawClone();
  h2->DrawCopy("same");

  return c;
}

TCanvas* pEff_Eta()
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  //TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  //ff->cd();

  std::string nc = "cEff_Eta";
  TCanvas* c = new TCanvas(nc.c_str(),nc.c_str(),600,600);
  TLegend l(0.7, 0.15, 0.94,.32);
  const int color[6] = {kBlack, kRed, kGreen, kBlue, kMagenta, kTeal};
  for (int icut=0; icut <=4; icut++) {
    std::stringstream ptCut;
    if (icut==0) ptCut<<"0";
    if (icut==1) ptCut<<"5";
    if (icut==2) ptCut<<"10";
    if (icut==3) ptCut<<"16";
    if (icut==4) ptCut<<"30";
    if (icut==5) ptCut<<"100";
    std::stringstream namehD;
    std::stringstream namehN;
    namehD << "hEff_EtaDenom" <<ptCut.str();
    namehN << "hEff_EtaCut"   <<ptCut.str();
    TH1D* hD=(TH1D*)gROOT->FindObject( namehD.str().c_str());
    TH1D* hN=(TH1D*)gROOT->FindObject( namehN.str().c_str());
    TH1D* h =DivideErr(hN,hD,(namehD.str()+"Eff").c_str(),"B");
    if(!h) return 0;
    h->SetTitle("Eff_Eta: # L1Rpc(p_{T}^{Rpc} #geq thr.)/ # Muon(p_{T}^{#mu} #geq thr.)");
    h->SetYTitle("Efficiency");
    h->SetMinimum(0); h->SetMaximum(1.04);
    h->SetMarkerStyle(21+icut);
    h->SetMarkerColor(color[icut]);
    if (icut==0)h->DrawCopy();else h->DrawCopy("same");
    std::string nameCut = ptCut.str()+" GeV/c"; if (icut==0) nameCut = "no p_{T} cut";
    l.AddEntry(h,nameCut.c_str());
  }
  l.DrawClone();
  return c;
}

TCanvas* pEff_Pt(std::string opt)
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  //TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  //ff->cd();

  std::string nc = "cEff_"+opt+"PtCut";
  TCanvas* c = new TCanvas(nc.c_str(),nc.c_str(),1400,500);
  c->Divide(3,1);
  std::string where[]={"_Bar","_Int","_End"};
  TLegend l(0.7, 0.15, 0.94,.52); 
  const int color[6] = {kBlack, kRed, kGreen, kBlue, kMagenta, kTeal};
  for (int i=1; i<=3; ++i) {
    TVirtualPad * p = c->cd(i); p->SetLogx(1);  p->SetGrid(0,1);
    TH1D* hD=(TH1D*)gROOT->FindObject( ("hEff_PtDenom"+where[i-1]).c_str());
    if (!hD) {
      std::cout <<" CANNOT FIND: " << "hEff_PtDenom"+where[i-1] << std::endl;
      continue;
    }
    for (int icut=0; icut <=5; icut++) {
      std::stringstream ptCut;
      if (icut==0) ptCut<<"0";
      if (icut==1) ptCut<<"5";
      if (icut==2) ptCut<<"10";
      if (icut==3) ptCut<<"16";
      if (icut==4) ptCut<<"30";
      if (icut==5) ptCut<<"100";
      std::stringstream name; name<<"hEff_"<<opt<<"PtCut"<<ptCut.str() <<where[i-1];
      TH1D* hN=(TH1D*)gROOT->FindObject( name.str().c_str() );
      if (!hN) {
       std::cout <<" CANNOT FIND: " << name.str() << std::endl;
       continue;
      }
      TH1D* h =DivideErr(hN,hD,(name.str()+"Eff").c_str(),"B");
      if(!h) return 0;
      h->SetTitle(("Eff_"+opt+"PtCut"+where[i-1]).c_str());
      h->SetYTitle("Efficiency");
      h->SetMinimum(0); h->SetMaximum(1.04);
      h->GetXaxis()->SetRange(4,100);
      h->SetMarkerStyle(21+icut);
      h->SetMarkerColor(color[icut]);
      if (icut==0)h->DrawCopy();else h->DrawCopy("same");
      std::string nameCut = ptCut.str()+" GeV/c"; if (icut==0) nameCut = "no p_{T} cut";
      if (i==1) l.AddEntry(h,nameCut.c_str());
    }
    l.DrawClone();
  }
  return c;
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
TCanvas* pEff_Pt2(int otfPtCut=16){

  std::string nc(TString::Format("cEff_OtfVsGMT_PtCut%d",otfPtCut).Data());
  TCanvas* c = new TCanvas(nc.c_str(),nc.c_str(),460,500);

  TLegend l(0.5592105,0.5783898,0.7982456,0.7309322,NULL,"brNDC");
  l.SetTextSize(0.05);
  l.SetFillStyle(4000);
  l.SetBorderSize(0);
  l.SetFillColor(10);
  const int color[6] = {kBlack, kRed, kGreen, kBlue, kMagenta, kTeal};
  c->SetLogx(1);  c->SetGrid(0,1);

  TH1D* hD=(TH1D*)gROOT->FindObject("hEff_PtDenom_Int");

  int icut = 3;
  std::string ptCut(TString::Format("%d",otfPtCut).Data());
  std::string name = "hEff_OtfPtCut"+ptCut+"_Int";

  TH1D* hNOtf=(TH1D*)gROOT->FindObject( name.c_str() );

  TH1D* hOtf =DivideErr(hNOtf,hD,"Pt_Int","B");

  int iBin = hOtf->FindBin(otfPtCut);
  float effOtf = hOtf->GetBinContent(iBin);
  std::cout<<"Otf eff: "<<effOtf<<std::endl;
  /////////////
  const double ptCuts[30] ={0., 0.1, 
			    1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5., 6., 7., 8., 
			    10., 12., 14., 16., 18., 20., 25., 30., 35., 40., 45., 
			    50., 60., 70., 80., 90., 100.};
  int match = -1;
  float delta = 999.0;
 for (int i=0; i<30; ++i) {
   std::string tmp = "hEff_GmtPtCut%1.0f_Int";
   if( int(ptCuts[i]*10)%10==5)  tmp = "hEff_GmtPtCut%1.1f_Int";
   TH1D* hN=(TH1D*)gROOT->FindObject(TString::Format(tmp.c_str(),ptCuts[i]).Data());
   TH1D* h =DivideErr(hN,hD,"hTmp","B");  
   float eff = h->GetBinContent(iBin);
   if(fabs(eff-effOtf)<delta){
     match = i;
     delta = fabs(eff-effOtf);
   }
 }
 std::string tmp = "hEff_GmtPtCut%1.0f_Int";
 if( int(ptCuts[match]*10)%10==5)  tmp = "hEff_GmtPtCut%1.1f_Int";
 TH1D* hN=(TH1D*)gROOT->FindObject(TString::Format(tmp.c_str(),ptCuts[match]).Data());
 TH1D* hGmt =DivideErr(hN,hD,"hGmt","B");  
 std::cout<<"Gmt eff: "<<hGmt->GetBinContent(iBin)<<std::endl;
 hGmt->SetMarkerStyle(21+icut);
 hGmt->SetMarkerColor(color[icut]+10);
 /////////////
  hOtf->SetTitle("Eff_OtfPtCut_Int");
  hOtf->SetYTitle("Efficiency");
  hOtf->SetMinimum(0.0001); 
  hOtf->SetMaximum(1.04);
  hOtf->GetXaxis()->SetRange(4,100);
  hOtf->SetMarkerStyle(8);
  hOtf->SetMarkerColor(color[icut]);
  hOtf->DrawCopy();
  hGmt->DrawCopy("same");
  std::string nameCut = ptCut+" GeV/c"; 
  if (icut==0) nameCut = "no p_{T} cut";
  l.AddEntry(hOtf,("Otf, "+nameCut).c_str());
  tmp = "Gmt, %1.0f GeV/c";
  if( int(ptCuts[match]*10)%10==5)  tmp = "Gmt, %1.1f GeV/c";
  l.AddEntry(hGmt,TString::Format(tmp.c_str(),ptCuts[match]).Data());
  l.DrawClone();

  TLine aLine(0,0,0,0);
  aLine.SetLineColor(2);
  aLine.SetLineWidth(3);
  aLine.DrawLine(ptCuts[match],0,ptCuts[match],1.04);

  return c;

}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
TCanvas* pEff_RpcPtCut_Q()
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  //TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  //ff->cd();

  std::string nc = "cEff_RpcPtCut_Q";
  TCanvas* c = new TCanvas(nc.c_str(),nc.c_str(),1200,600);
  c->Divide(2,1);
  std::string what[]={"_Qeq0","_Qgt0"};
  const int color[6] = {kBlack, kRed, kGreen, kBlue, kMagenta, kTeal};
  for (int i=1; i<=2; ++i) {
    TLegend l = (i==1) ? TLegend(0.68, 0.51, 0.92,.88) : TLegend(0.68, 0.15, 0.92,.52);
    TVirtualPad * p = c->cd(i); p->SetLogx(1);  p->SetGrid(0,1);
    TH1D* hD=(TH1D*)gROOT->FindObject( ("hEff_PtDenom"+what[i-1]).c_str());
    if (!hD) {
      std::cout <<" CANNOT FIND: " << "hEff_PtDenom"+what[i-1] << std::endl;
      continue;
    }
    for (int icut=0; icut <=4; icut++) {
      std::stringstream ptCut;
      if (icut==0) ptCut<<"0";
      if (icut==1) ptCut<<"5";
      if (icut==2) ptCut<<"10";
      if (icut==3) ptCut<<"16";
      if (icut==4) ptCut<<"30";
      if (icut==5) ptCut<<"100";
      std::stringstream name; name<<"hEff_RpcPtCut"<<ptCut.str() <<what[i-1];
      TH1D* hN=(TH1D*)gROOT->FindObject( name.str().c_str() );
      if (!hN) {
       std::cout <<" CANNOT FIND: " << name.str() << std::endl;
       continue;
      }
      TH1D* h =DivideErr(hN,hD,(name.str()+"Eff").c_str(),"B");
      if(!h) return 0;
      h->SetTitle(("Eff_RpcPtCut"+what[i-1]).c_str());
      h->SetYTitle("Efficiency");
      h->SetMinimum(0); h->SetMaximum(1.04);
      h->GetXaxis()->SetRange(4,100);
      h->SetMarkerStyle(21+icut);
      h->SetMarkerColor(color[icut]);
      if (icut==0)h->DrawCopy();else h->DrawCopy("same");
      std::string nameCut = ptCut.str()+" GeV/c"; if (icut==0) nameCut = "no p_{T} cut";
      l.AddEntry(h,nameCut.c_str());
    }
    l.DrawClone();
  }
  return c;
}

void plotsEff(){
  //utilsPlotsSaver( pEff_Pt("Rpc") );
 //utilsPlotsSaver( pEff_Pt("Oth") );
 utilsPlotsSaver( pEff_Pt("Gmt") );
 utilsPlotsSaver( pEff_Pt("Otf") );
 utilsPlotsSaver( pEff_Pt2(5) );
 utilsPlotsSaver( pEff_Pt2(10) );
 utilsPlotsSaver( pEff_Pt2(16) );
 utilsPlotsSaver( pEff_Pt2(20) );
 utilsPlotsSaver( pEff_Pt2(25) );
// utilsPlotsSaver( pEff_RpcPtCut_Q() );
// utilsPlotsSaver( pEff_Eta() );
 //utilsPlotsSaver( pEff_Effic() );
}
#endif
