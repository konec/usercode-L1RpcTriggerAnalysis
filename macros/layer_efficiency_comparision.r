{
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
  gROOT.LoadMacro("FigUtils.C");
  MyDefaultStyle->cd();
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(1);

  c9 =  new TCanvas("c9","c9",-1);
  c9->SetFillColor(0);
  c9->Divide(2,1);
  c9->cd(1);
  gPad->SetGrid();
  c9->cd(2);
  gPad->SetGrid();


//Comment or uncomment what needed
//Default - Layer efficiency comparision of 2 root files.

//Layer efficiency comparision of 2 (3) root files.
//File names: efficiencyHistos_1.root, efficiencyHistos_2.root, (efficiencyHistos_3.root)

  TFile file_1("efficiencyHistos_1.root");
  file_1.ls();

  c9->cd(1);

  TH1D * heffChambBar_1 = DivideErr(hEfficChambBar_N,hEfficChambBar_D,"hEfficChambBar","B");
  heffChambBar_1->SetMinimum(0.8);
  heffChambBar_1->SetMaximum(1.002);
  heffChambBar_1->SetXTitle("layer");
  heffChambBar_1->SetYTitle("efficiency");
  heffChambBar_1->SetLineColor(kGreen);
  heffChambBar_1->DrawCopy("E");

  c9->cd(2);

  TH1D * heffChambEnd_1 = DivideErr(hEfficChambEnd_N,hEfficChambEnd_D,"hEfficChambEnd","B");
  heffChambEnd_1->SetMinimum(0.8);
  heffChambEnd_1->SetMaximum(1.002);
  heffChambEnd_1->SetXTitle("layer");
  heffChambEnd_1->SetYTitle("efficiency");
  heffChambEnd_1->SetLineColor(kGreen);
  heffChambEnd_1->DrawCopy("E");

  TFile file_2("efficiencyHistos_2.root");
  file_2.ls();

  c9->cd(1);

  TH1D * heffChambBar_2 = DivideErr(hEfficChambBar_N,hEfficChambBar_D,"hEfficChambBar","B");
  heffChambBar_2->SetMinimum(0.8);
  heffChambBar_2->SetMaximum(1.002);
  heffChambBar_2->SetXTitle("layer");
  heffChambBar_2->SetYTitle("efficiency");
  heffChambBar_2->SetLineColor(kBlue);
  heffChambBar_2->DrawCopy("E same");

  c9->cd(2);

  TH1D * heffChambEnd_2 = DivideErr(hEfficChambEnd_N,hEfficChambEnd_D,"hEfficChambEnd","B");
  heffChambEnd_2->SetMinimum(0.8);
  heffChambEnd_2->SetMaximum(1.002);
  heffChambEnd_2->SetXTitle("layer");
  heffChambEnd_2->SetYTitle("efficiency");
  heffChambEnd_2->SetLineColor(kBlue);
  heffChambEnd_2->DrawCopy("E same");

/*  TFile file_3("efficiencyHistos_3.root");
  file_3.ls();

  c9->cd(1);  

  TH1D * heffChambBar_3 = DivideErr(hEfficChambBar_N,hEfficChambBar_D,"hEfficChambBar","B");
  heffChambBar_3->SetMinimum(0.8);
  heffChambBar_3->SetMaximum(1.002);
  heffChambBar_3->SetXTitle("layer");
  heffChambBar_3->SetYTitle("efficiency");
  heffChambEnd_3->SetLineColor(kRed);
  heffChambBar_3->DrawCopy("E same");

  c9->cd(2);

  TH1D * heffChambEnd_3 = DivideErr(hEfficChambEnd_N,hEfficChambEnd_D,"hEfficChambEnd","B");
  heffChambEnd_3->SetMinimum(0.8);
  heffChambEnd_3->SetMaximum(1.002);
  heffChambEnd_3->SetXTitle("layer");
  heffChambEnd_3->SetYTitle("efficiency");
  heffChambEnd_3->SetLineColor(kRed);
  heffChambEnd_3->DrawCopy("E same"); */

//Layer efficiency comparision between data from 1 rootfile and detector group data
//To use put values of layer efficiency taken from detector group in place of 0.9 in lines heffChambXXX_2->SetBinContent(1,0.9); 
//and error values in place of 0. or 0.000001 in lines heffChambXXX_2->SetBinError(1,0.000001);
//Small value in the first bin - no error bars.

/*  TFile file_1("efficiencyHistos_1.root");
  file_1.ls();

  c9->cd(1);

  TH1D * heffChambBar_1 = DivideErr(hEfficChambBar_N,hEfficChambBar_D,"hEfficChambBar","B");
  heffChambBar_1->SetMinimum(0.8);
  heffChambBar_1->SetMaximum(1.002);
  heffChambBar_1->SetXTitle("layer");
  heffChambBar_1->SetYTitle("efficiency");
  heffChambBar_1->SetLineColor(kGreen);
  heffChambBar_1->DrawCopy("E");

  c9->cd(2);

  TH1D * heffChambEnd_1 = DivideErr(hEfficChambEnd_N,hEfficChambEnd_D,"hEfficChambEnd","B");
  heffChambEnd_1->SetMinimum(0.8);
  heffChambEnd_1->SetXTitle("layer");
  heffChambEnd_1->SetYTitle("efficiency");
  heffChambEnd_1->SetLineColor(kGreen);
  heffChambEnd_1->DrawCopy("E"); 

  c9->cd(1);

  TH1D * heffChambBar_2 = DivideErr(hEfficChambBar_N,hEfficChambBar_D,"hEfficChambBar","B");
  heffChambBar_2->SetMinimum(0.8);
  heffChambBar_2->SetMaximum(1.002);
  heffChambBar_2->SetXTitle("layer");
  heffChambBar_2->SetYTitle("efficiency");
  heffChambBar_2->SetLineColor(kBlue);
  heffChambBar_2->SetBinContent(1,0.9);
  heffChambBar_2->SetBinContent(2,0.9);
  heffChambBar_2->SetBinContent(3,0.9);
  heffChambBar_2->SetBinContent(4,0.9);
  heffChambBar_2->SetBinContent(5,0.9);
  heffChambBar_2->SetBinContent(6,0.9);
  heffChambBar_2->SetBinError(1,0.000001);
  heffChambBar_2->SetBinError(2,0.);
  heffChambBar_2->SetBinError(3,0.);
  heffChambBar_2->SetBinError(4,0.);
  heffChambBar_2->SetBinError(5,0.);
  heffChambBar_2->SetBinError(6,0.);
  heffChambBar_2->DrawCopy("E same");

  c9->cd(2);

  TH1D * heffChambEnd_2 = DivideErr(hEfficChambEnd_N,hEfficChambEnd_D,"hEfficChambEnd","B");
  heffChambEnd_2->SetMinimum(0.8);
  heffChambEnd_2->SetMaximum(1.002);
  heffChambEnd_2->SetXTitle("layer");
  heffChambEnd_2->SetYTitle("efficiency");
  heffChambEnd_2->SetBinContent(1,0.9);
  heffChambEnd_2->SetBinContent(2,0.9);
  heffChambEnd_2->SetBinContent(3,0.9);
  heffChambEnd_2->SetBinError(1,0.000001);
  heffChambEnd_2->SetBinError(2,0.);
  heffChambEnd_2->SetBinError(3,0.);
  heffChambEnd_2->SetLineColor(kBlue);
  heffChambEnd_2->DrawCopy("E same"); */

  c9.Print(0,".png");
}





