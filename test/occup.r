{
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
  gROOT.LoadMacro("~/root/FigUtils.C");
  MyDefaultStyle->cd();
  TFile f("merge.root");
  
  TCanvas coccup("coccup","coccup",-2);
  gPad.SetLeftMargin(0.32);
  topSpread.SetXTitle("LB data delay [BX+3]");
  topOccup.SetXTitle("LB data delay [BX+3]");
  topOccup.SetTitle("Top 10 occupancy");
  topOccup.Draw("text");
  coccup.Print(0,".eps");

  TCanvas cspread("cspread","cspread",-2);
  gPad.SetLeftMargin(0.32);
  topSpread.SetXTitle("LB data delay [BX+3]");
  topSpread.SetTitle("Top 10 spread");
  topSpread.Draw("text");
  cspread.Print(0,".eps");
  
}
