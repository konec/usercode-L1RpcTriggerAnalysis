#include "TROOT.h"
#include "TDirectory.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TStyle.h"
#include "TText.h"
#include "TLatex.h"
#include "TString.h"
#include "TGaxis.h"
#include "TGraphErrors.h"
#include "TFile.h"
#include "TVirtualPad.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include "utilsPlotsSaver.h"

TCanvas* pDigiSpecPosCorr2(const std::string &det) 
{
  std::stringstream tag;
  tag <<"DigiSpec_PosCorr_"<<det<<"2";
  std::string cName = "c"+tag.str();
  TCanvas* c = new TCanvas(cName.c_str(), cName.c_str(),-2);
  std::string hName = "h"+tag.str();
  TH2D *h = (TH2D*)gROOT->FindObject(hName.c_str());
  h->DrawCopy("box");
  return c;
}

TCanvas* pDigiSpecDetVsPt(const std::string &det, const std::string &opt)
{
  std::stringstream tag;
  tag << "DigiSpec_"<<det<<"VsPt_"<<opt;  
  std::string cName = "c"+tag.str();
  TCanvas* c = new TCanvas(cName.c_str(), cName.c_str(), 900, 900);
  TVirtualPad* p; TH2D *h; 
  TText t;  t.SetTextColor(4); t.SetTextAlign(22); t.SetTextSize(0.044);
  TText tt;tt.SetTextColor(4); tt.SetTextAlign(12); tt.SetTextSize(0.044);

  c->Divide(3,3);
  for (unsigned int i=1; i<=9; ++i) {
    p = c->cd(i);
    std::stringstream hName; hName<<"h"<<tag.str()<<"_"<<i;
//    std::cout <<"hName: " << hName.str() << std::endl;
    h = (TH2D*)gROOT->FindObject(hName.str().c_str());
    if (h==0) continue;
    p->SetLogx();
    h->GetXaxis()->SetRange(4,32);
     std::string fullTitle(h->GetTitle());
    std::string title(fullTitle, 10, fullTitle.size());
    h->DrawCopy("box");
    t.DrawTextNDC(0.53,0.94, title.c_str());
    std::stringstream nEvents; nEvents <<"Integral: "<<h->Integral();
    tt.DrawTextNDC(0.17, 0.17, nEvents.str().c_str());
  }
  return c;
}

void plotsDigiSpec()
{
  utilsPlotsSaver( pDigiSpecPosCorr2("RPC"));
  utilsPlotsSaver( pDigiSpecPosCorr2("CSC"));
  utilsPlotsSaver( pDigiSpecPosCorr2("DT"));

  utilsPlotsSaver( pDigiSpecDetVsPt("RpcB","PosB"));
  utilsPlotsSaver( pDigiSpecDetVsPt("RpcE","PosB"));
  utilsPlotsSaver( pDigiSpecDetVsPt("Csc","PosB"));
  utilsPlotsSaver( pDigiSpecDetVsPt("Dt","PosB"));
  utilsPlotsSaver( pDigiSpecDetVsPt("Dt","BendB"));
  utilsPlotsSaver( pDigiSpecDetVsPt("Csc","BendB"));

  utilsPlotsSaver( pDigiSpecDetVsPt("RpcB","PosE"));
  utilsPlotsSaver( pDigiSpecDetVsPt("RpcE","PosE"));
  utilsPlotsSaver( pDigiSpecDetVsPt("Csc","PosE"));
  utilsPlotsSaver( pDigiSpecDetVsPt("Dt","PosE"));
  utilsPlotsSaver( pDigiSpecDetVsPt("Dt","BendE"));
  utilsPlotsSaver( pDigiSpecDetVsPt("Csc","BendE"));
}

