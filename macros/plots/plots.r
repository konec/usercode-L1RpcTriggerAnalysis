{

// To produce plots run this commsnd:
//
// root -l -q -b plots.r
//
// NOTE: without -b (batch) option wide plots (>1200 pixels) wont be drawn!
//

  //
  // little cleanup
  //
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();


#include "utilsPlotsSaver.h"
#include "utilsObjArray.h"
#include "utilsHistoFromGraph.h"

  gROOT->LoadMacro("utilsL1RpcStyle.C+");
  gROOT->LoadMacro("utilsPlotsSaver.C+g");
  gROOT->LoadMacro("utilsObjArray.C+g");
  gROOT->LoadMacro("utilsHistoFromGraph.C+");

  gROOT->LoadMacro("plotsMuonDist.C+");
  gROOT->LoadMacro("plotsRpcMisc.C+");
  gROOT->LoadMacro("plotsEff.C+");
  gROOT->LoadMacro("plotsRpcVsOth.C+");
  gROOT->LoadMacro("plotsDet.C+");
  gROOT->LoadMacro("plotsClu.C+");
  gROOT->LoadMacro("plotsSynch.C+");
  gROOT->LoadMacro("plotsTimingL1.C+");
  gROOT->LoadMacro("plotsMenu.C+");
  gROOT->LoadMacro("plotsEvent.C+");

  if (std::string(gStyle->GetName()) != "L1RpcStyle" ) {
    if (0==gROOT->GetListOfStyles()->FindObject("L1RpcStyle")) utilsL1RpcStyle();
    gROOT->SetStyle("L1RpcStyle");
    gROOT->ForceStyle();
    gStyle->ls();
  }

  //
  // get histos
  //
  g_L1RpcGlobalTObjects.SetOwner();

  TFile file_pressure("../PressureGraph.root");
  TGraphErrors *gr_Pressure = file_pressure.IsZombie() ? 0 : file_pressure.Get("Graph")->Clone("gr_Pressure");
  if (gr_Pressure) g_L1RpcGlobalTObjects.Add(gr_Pressure);
  file_pressure.Close();


  TFile file_humidity("../HumidityGraph.root");
  TGraphErrors *gr_Humidity = file_humidity.IsZombie() ? 0 : file_humidity.Get("Graph")->Clone("gr_Humidity");
  if (gr_Humidity) g_L1RpcGlobalTObjects.Add(gr_Humidity);
  file_humidity.Close();

  TFile file("../l1RpcAnalysis.root");
  if (file.IsZombie()) return;
  file.ls(); 

  //
  // plots
  //
//  plotsMuonDist();
//  plotsMenu();
//  plotsEvent();

//   plotsEff();
//    plotsRpcVsOth();
//    plotsDet();
//    plotsClu();
//    plotsRpcMisc();
//    plotsSynch();
  plotsTimingL1();



  //
  // cleanup
  //
  g_L1RpcGlobalTObjects.Delete();
//  file.Close();
   

  gROOT->ProcessLine(".U plotsMenu_C.so");
  gROOT->ProcessLine(".U plotsTimingL1_C.so");
  gROOT->ProcessLine(".U plotsSynch_C.so");
  gROOT->ProcessLine(".U plotsClu_C.so");
  gROOT->ProcessLine(".U plotsEff_C.so");
  gROOT->ProcessLine(".U plotsDet_C.so");
  gROOT->ProcessLine(".U plotsRpcMisc_C.so");
  gROOT->ProcessLine(".U plotsRpcVsOth_C.so");
  gROOT->ProcessLine(".U plotsMuonDist_C.so");

  gROOT->ProcessLine(".U utilsHistoFromGraph_C.so");
  gROOT->ProcessLine(".U utilsObjArray_C.so");
  gROOT->ProcessLine(".U utilsPlotsSaver_C.so");

  std::cout <<"----------------->END"<<std::endl;
}
