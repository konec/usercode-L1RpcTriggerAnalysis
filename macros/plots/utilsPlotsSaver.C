#ifndef utilsPlotsSaver_C
#define utilsPlotsSaver_C

#include <cstring>
#include <iostream>
#include <cmath>
#include "TSystem.h"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"

void utilsPlotsSaver(TCanvas *c)
{
  std::string base = c->GetName();
//  std::cout <<" BASE: "<<base<<std::endl;

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
//  TDirectory * dir = gDirectory; 
//  TFile fout( ( "root/"+base+".root").c_str(),"RECREATE");
//  fout.cd(); c->Write(); dir->cd(); fout.Close();      
}

#endif 
