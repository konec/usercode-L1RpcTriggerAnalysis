#include "utilsPlotsSaver.h"

#include <cstring>
#include <iostream>
#include <string>
#include <iostream>
#include "TSystem.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"

void utilsPlotsSaver(TCanvas *c)
{
  if(!c) return; // do nothing

  std::string base = c->GetName();
  std::cout <<" CANVAS NAME: "<<base<<std::endl;

  gSystem->mkdir("./fig_C");
  gSystem->mkdir("./fig_pdf");
  gSystem->mkdir("./fig_eps");
  gSystem->mkdir("./fig_png");
  gSystem->mkdir("./fig_root");

  //eps
  c->Print( ("fig_eps/"+base+".eps").c_str(),"eps");
  //png
  c->Print( ("fig_png/"+base+".png").c_str(),"png");
  //cxx
  c->Print( ("fig_C/"+base+".C").c_str(),"cxx");
  //pdf
  c->Print( ("fig_pdf/"+base+".pdf").c_str(),"pdf");


  //root
  TDirectory *dir = gDirectory; 
  TFile fout( ( "fig_root/"+base+".root").c_str(),"RECREATE");
  c->Write(); 
  fout.Close();      
  dir->cd(); 
}
