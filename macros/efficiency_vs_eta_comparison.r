{
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
  gROOT.LoadMacro("FigUtils.C");
  MyDefaultStyle->cd();
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(1);

 
//Efficiency vs. eta, comparison of 2 root files
//File names: efficiencyHistos_1.root, efficiencyHistos_2.root
//Define legend

  c2 =  new TCanvas("c2","det in concidence for trigger",-2);
  c2->SetFillColor(0);
  gPad->SetGrid();
  TFile file("efficiencyHistos_1.root");
  file.ls();

  TH1D * heffM = DivideErr(hEfficMu_N,hEfficMu_D,"hEfficMu","B");
  heffM->SetMinimum(0.);
  heffM->SetMaximum(1.002);
  heffM->SetXTitle("#eta"); 
  heffM->SetYTitle("efficiency");
  heffM->SetLineColor(kBlack);   
  heffM->SetFillStyle(3003);     
  heffM->SetFillColor(kBlack);

  TFile file1("efficiencyHistos_2.root");
  file1.ls();

  TH1D * heffM1 = DivideErr(hEfficMu_N,hEfficMu_D,"hEfficMu","B");
  heffM1->SetMinimum(0.);
  heffM1->SetMaximum(1.002);
  heffM1->SetXTitle("#eta");
  heffM1->SetYTitle("efficiency");
  heffM1->SetLineColor(kRed);
  heffM1->SetFillStyle(3003);
  heffM1->SetFillColor(kRed);
  TLegend legend3(-1.5,0.1,1.5,0.2,"","");
  legend3->SetFillColor(kWhite); 
  legend3.AddEntry(heffM1,"XXXX");
  legend3.AddEntry(heffM,"YYYY");
            
  heffM1->DrawCopy("hist");      
  heffM->DrawCopy("hist same");  
  
  legend3.Draw();
  c2->Print(0,".png");  
}
