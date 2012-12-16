#include "TROOT.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TVirtualPad.h"
#include "TPaveStats.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGaxis.h"
#include "TFile.h"
#include "TLegend.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#include "utilsPlotsSaver.h"
#include "utilsObjArray.h"


TCanvas* pMuonDist_1D( bool useHelper)
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  //TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  //ff->cd();


//
// upload histos from "helper file" (optional).
//
  TH1D *hMuonPt_BMF, *hMuonEta_BMF, *hMuonPhi_BMF;
  hMuonPt_BMF = hMuonEta_BMF = hMuonPhi_BMF = 0;
  if (useHelper) {
    TDirectory *dBefore = gDirectory;
    TFile file_BMF("../l1RpcHelper.root");
    hMuonPt_BMF =  (TH1D*) file_BMF.Get("hMuonPt_BMF")->Clone("hMuonPt_BMF_C");  hMuonPt_BMF->SetDirectory(0);
    hMuonEta_BMF = (TH1D*) file_BMF.Get("hMuonEta_BMF")->Clone("hMuonEta_BMF_C"); hMuonEta_BMF->SetDirectory(0);
    hMuonPhi_BMF = (TH1D*) file_BMF.Get("hMuonPhi_BMF")->Clone("hMuonPhi_BMF_C"); hMuonPhi_BMF->SetDirectory(0);

//    hMuonPt_BMF  = new TH1D(* (TH1D*) file_BMF.Get("hMuonPt_BMF"));   hMuonPt_BMF->SetDirectory(0);
//    hMuonEta_BMF = new TH1D(* (TH1D*) file_BMF.Get("hMuonEta_BMF")); hMuonEta_BMF->SetDirectory(0);
//    hMuonPhi_BMF = new TH1D(* (TH1D*) file_BMF.Get("hMuonPhi_BMF")); hMuonPhi_BMF->SetDirectory(0);

//    g_L1RpcGlobalTObjects.Add( file_BMF.Get("hMuonPt_BMF")->Clone("hMuonPt_BMF_C") );
//    g_L1RpcGlobalTObjects.Add( file_BMF.Get("hMuonEta_BMF")->Clone("hMuonEta_BMF_C") );
//    g_L1RpcGlobalTObjects.Add( file_BMF.Get("hMuonPhi_BMF")->Clone("hMuonPhi_BMF_C") );
    file_BMF.Close();
    dBefore->cd();
  }

  TCanvas * c = new TCanvas("cMuonDist_1D","Global muons distributions",1400,500);
  c->Divide(3,1);

//
// p1
//
  TVirtualPad * p1 = c->cd(1); p1->SetLogy(1); p1->SetLogx(1);
  TH1D* hMuonPt_DIS = (TH1D*) gROOT->FindObject("hMuonPt_DIS"); 
  if(hMuonPt_BMF)  hMuonPt_DIS->SetMaximum( hMuonPt_BMF->GetMaximum() * 250); 
  hMuonPt_DIS->SetXTitle("Glb.muon p_{T} [GeV/c]");
  hMuonPt_DIS->SetLineColor(2);
  hMuonPt_DIS->GetXaxis()->SetRange(3,33);
  hMuonPt_DIS->DrawCopy();

  TLegend l(0.3, 0.65, 0.93,.88);
  int nev;
  std::stringstream str; 

  if (hMuonPt_BMF) {
    nev= hMuonPt_BMF->GetEntries();
    str.str(""); str<<"all muons (ev: "<<std::setprecision(2) << scientific << (double) nev<<")"; 
    hMuonPt_BMF->SetLineColor(8);
    hMuonPt_BMF->DrawCopy("same");
    l.AddEntry(hMuonPt_BMF, str.str().c_str());
  }

  nev= hMuonPt_DIS->GetEntries();
  str.str(""); str<<"kinematic sel. (ev: "<< std::setprecision(2) << scientific << (double)nev<<")";
  l.AddEntry(hMuonPt_DIS, str.str().c_str() );

  TH1D* hMuonPt_MEN=(TH1D*)gROOT->FindObject("hMuonPt_MEN");
  if (hMuonPt_MEN) {
    hMuonPt_MEN->SetLineColor(4);
    hMuonPt_MEN->DrawCopy("same");
    nev= hMuonPt_MEN->GetEntries();
    str.str(""); str<<"trigger sel. (ev: "<< setprecision(3) << scientific  << (double) nev<<")";
    l.AddEntry(hMuonPt_MEN, str.str().c_str());
  }

  l.DrawClone("same");


//
// p2
//
  TVirtualPad * p2 = c->cd(2); p2->SetLogy(1);
  TH1D* hMuonEta_DIS=(TH1D*)gROOT->FindObject("hMuonEta_DIS");
  if(hMuonEta_BMF) hMuonEta_DIS->SetMaximum( hMuonEta_BMF->GetMaximum()); 
  TH1D* hMuonEta_MEN=(TH1D*)gROOT->FindObject("hMuonEta_MEN");
//  if(hMuonEta_MEN) hMuonEta_DIS->SetMinimum( max(1, (int)hMuonEta_MEN->GetMinimum()));
  hMuonEta_DIS->SetMinimum(1.e3);
  hMuonEta_DIS->SetLineColor(2);
  hMuonEta_DIS->SetXTitle("Glb.muon #eta");
  hMuonEta_DIS->DrawCopy();
  if (hMuonEta_BMF) {
   hMuonEta_BMF->SetLineColor(8);
   hMuonEta_BMF->DrawCopy("same");
  }
  if (hMuonEta_MEN) {
   hMuonEta_MEN->SetLineColor(4);
   hMuonEta_MEN->DrawCopy("same");
  }


//
// p3
//
  c->cd(3); 
  TH1D* hMuonPhi_DIS=(TH1D*)gROOT->FindObject("hMuonPhi_DIS");
  hMuonPhi_DIS->Scale(1./hMuonPhi_DIS->Integral());
  hMuonPhi_DIS->SetMinimum(0.);
  hMuonPhi_DIS->SetXTitle("Glb.muon #phi [rad]");
  hMuonPhi_DIS->SetYTitle("arbitrary units");
  hMuonPhi_DIS->SetLineColor(2);
  hMuonPhi_DIS->DrawCopy();
  if (hMuonPhi_BMF) {
   hMuonPhi_BMF->Scale(1./hMuonPhi_BMF->Integral());
   hMuonPhi_BMF->SetLineColor(8);
   hMuonPhi_BMF->DrawCopy("same");
  }
  TH1D* hMuonPhi_MEN=(TH1D*)gROOT->FindObject("hMuonPhi_MEN");
  if (hMuonPhi_MEN) {
   hMuonPhi_MEN->Scale(1./hMuonPhi_MEN->Integral());
   hMuonPhi_MEN->SetLineColor(4);
   hMuonPhi_MEN->DrawCopy("same");
  }

  return c;
}


TCanvas* pMuonDist_PtVsEtaDist()
{
  // below 2 lines help when another TFile has been opened in memory
  // otherwise FindObject fails
  //TFile *ff = (TFile*)(gROOT->GetListOfFiles()->First());
  //ff->cd();

  TCanvas * c = new TCanvas("cMuonDist_PtVsEtaDist","Global muons pt-eta distributions",1400,500);
  c->Divide(3,1);

  TVirtualPad* p1 = c->cd(1); p1->SetLogy(1); 
  TH2D* hMuonPtVsEta_Tk =  (TH2D*)gROOT->FindObject("hMuonPtVsEta_Tk");
  if(!hMuonPtVsEta_Tk) return 0;
  hMuonPtVsEta_Tk->SetAxisRange(2.,140.,"Y");
  hMuonPtVsEta_Tk->SetXTitle("Glb.muon #eta");
  hMuonPtVsEta_Tk->SetYTitle("Glb.muon p_{T} [GeV/c]");
  hMuonPtVsEta_Tk->DrawCopy("box");

  TVirtualPad* p2 = c->cd(2); p2->SetLogy(1); 
  TH2D* hMuonPtVsEta_Ma =  (TH2D*)gROOT->FindObject("hMuonPtVsEta_Ma"); 
  if(!hMuonPtVsEta_Ma) return 0;
  hMuonPtVsEta_Ma->SetAxisRange(2.,140.,"Y");
  hMuonPtVsEta_Ma->SetXTitle("Glb.muon #eta");
  hMuonPtVsEta_Ma->SetYTitle("Glb.muon p_{T} [GeV/c]");
  hMuonPtVsEta_Ma->DrawCopy("box");

  TVirtualPad* p3 = c->cd(3); p3->SetLogy(1); 
  TH2D* hMuonPtVsEta_Gl =  (TH2D*)gROOT->FindObject("hMuonPtVsEta_Gl");
  if(!hMuonPtVsEta_Gl) return 0;
  hMuonPtVsEta_Gl->SetAxisRange(2.,140.,"Y");
  hMuonPtVsEta_Gl->SetXTitle("Glb.muon #eta");
  hMuonPtVsEta_Gl->SetYTitle("Glb.muon p_{T} [GeV/c]");
  hMuonPtVsEta_Gl->DrawCopy("box");
  return c;
}

void plotsMuonDist()
{
  int digits=TGaxis::GetMaxDigits(); // for TAxis labels
  // make and save plots
  TGaxis::SetMaxDigits(3); // global option
  utilsPlotsSaver( pMuonDist_1D(true) );
//  utilsPlotsSaver( pMuonDist_PtVsEtaDist() );

  // restore previous gStyle settings
  TGaxis::SetMaxDigits(digits);

}
