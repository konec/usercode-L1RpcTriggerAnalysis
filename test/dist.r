{
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
  gROOT.LoadMacro("~/root/FigUtils.C");
  MyDefaultStyle->cd();

//  TCanvas cRMSB("cRMSB","cRMSB",-2);

  TCanvas cep("cep","cep",-2);
//  TFile f("merge.root");
  TFile f("analysis.root");
  cep.cd();
  delaySummary.SetLineColor(2);
  delaySummary.SetLineWidth(3);
  delaySummary.SetXTitle("LB data delay [BX]");
  delaySummary.SetYTitle("entries");
//  gPad->SetLogy();
  delaySummary.GetXaxis()->SetNdivisions(8);
  delaySummary.SetTitleOffset(2.0,"Y");
  delaySummary.DrawCopy();
   delaySummary.Print("all");
  cep.Print(0,".eps");

goto end;

  TCanvas cRMSEP("cRMSEP","cRMSEP",-2);
  cRMSEP.cd();
  delaySpread.SetLineColor(2);
  delaySpread.DrawCopy("box");

  TCanvas cwp2("cwp2","cwp2",-2);
  TFile f("histos_w+2_471.root");
  delaySummary.SetLineColor(3);
  delaySummary.SetLineWidth(3);
  delaySummary.SetXTitle("LB data delay [BX]");
  delaySummary.SetYTitle("entries [bx]");
  delaySummary.GetXaxis()->SetNdivisions(8);
  delaySummary.SetTitleOffset(1.8,"Y");
  delaySummary.DrawCopy();
  cRMSB.cd();
  delaySpread.SetLineColor(3);
  delaySpread.DrawCopy("box");

  
  TCanvas cwp1("cwp1","cwp1",-2);
  TFile f("histos_w+1_471.root");
  delaySummary.SetLineColor(4);
  delaySummary.SetLineWidth(3);
  delaySummary.SetXTitle("LB data delay [BX]");
  delaySummary.SetYTitle("entries [bx]");
  delaySummary.GetXaxis()->SetNdivisions(8);
  delaySummary.SetTitleOffset(1.8,"Y");
  delaySummary.DrawCopy();
  cRMSB.cd();
  delaySpread.SetLineColor(4);
  delaySpread.DrawCopy("box same");

  TCanvas cw0("cw0","cw0",-2);
  TFile f("histos_w0_471.root");
  delaySummary.SetLineColor(5);
  delaySummary.SetLineWidth(3);
  delaySummary.SetXTitle("LB data delay [BX]");
  delaySummary.SetYTitle("entries [bx]");
  delaySummary.GetXaxis()->SetNdivisions(8);
  delaySummary.SetTitleOffset(1.8,"Y");
  delaySummary.DrawCopy();
  cRMSB.cd();
  delaySpread.SetLineColor(5);
  delaySpread.DrawCopy("box same");

  TCanvas cm1("cwm1","cwm1",-2);
  TFile fwp2("histos_w-1_471.root");
  delaySummary.SetLineColor(6);
  delaySummary.SetLineWidth(3);
  delaySummary.SetXTitle("LB data delay [BX]");
  delaySummary.SetYTitle("entries [bx]");
  delaySummary.GetXaxis()->SetNdivisions(8);
  delaySummary.SetTitleOffset(1.8,"Y");
  delaySummary.DrawCopy();
  cRMSB.cd();
  delaySpread.SetLineColor(6);
  delaySpread.DrawCopy("box same");

  TCanvas cwm2("cwm2","cwm2",-2);
  TFile f("histos_w-2_471.root");
  delaySummary.SetLineColor(7);
  delaySummary.SetLineWidth(3);
  delaySummary.SetXTitle("LB data delay [BX]");
  delaySummary.SetYTitle("entries [bx]");
  delaySummary.GetXaxis()->SetNdivisions(8);
  delaySummary.SetTitleOffset(1.8,"Y");
  delaySummary.DrawCopy();
  cRMSB.cd();
  delaySpread.SetLineColor(7);
  delaySpread.DrawCopy("box same");

  TCanvas cen("cen","cen",-2);
  TFile f("histos_en_471F.root");
  delaySummary.SetLineColor(8);
  delaySummary.SetLineWidth(3);
  delaySummary.SetXTitle("LB data delay [BX]");
  delaySummary.SetYTitle("entries [bx]");
  delaySummary.GetXaxis()->SetNdivisions(8);
  delaySummary.SetTitleOffset(1.8,"Y");
  delaySummary.DrawCopy();
  TCanvas cRMSEN("cRMSEN","cRMSEN",-2);
  cRMSEN.cd();
  delaySpread.SetLineColor(8);
  delaySpread.DrawCopy("box");

  cep.Print(0,".eps");
  cwp2.Print(0,".eps");
  cwp1.Print(0,".eps");
  cw0.Print(0,".eps");
  cwm1.Print(0,".eps");
  cwm2.Print(0,".eps");
  cen.Print(0,".eps");
  cRMSB.Print(0,".eps");
  cRMSEN.Print(0,".eps");
  cRMSEP.Print(0,".eps");

end:
}

