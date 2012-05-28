#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TStyle.h"

#include "utilsL1RpcStyle.C"
#include "plotsMuonDist.C"
#include "plotsRpcMisc.C"
#include "plotsEff.C"
#include "plotsRpcVsOth.C"
#include "plotsDet.C"

void plots()
{
  //
  // little cleanup
  //
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();

  if (std::string(gStyle->GetName()) != "L1RpcStyle" ) {
    if (0==gROOT->GetListOfStyles()->FindObject("L1RpcStyle")) utilsL1RpcStyle();
    gROOT->SetStyle("L1RpcStyle");
    gROOT->ForceStyle();
    gStyle->ls();
  }

  //
  // get histos
  //
  TFile *file = new TFile("efficiencyHistos.root");
  if (file==NULL) return;
  file->ls();

  //
  // plots
  //
  // gStyle->SetOptTitle(1);

  plotsMuonDist();
  plotsRpcMisc();
  plotsEff();
  plotsRpcVsOth();
  plotsDet();
}
