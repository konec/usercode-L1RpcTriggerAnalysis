{
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
  gROOT.LoadMacro("~/root/FigUtils.C");
  MyDefaultStyle->cd();
  gROOT.LoadMacro("~/root/RegionLabels.C");


//--
  TFile file("../efficiencyHistos.root");
  file.ls();
//--------------------
  MyDefaultStyle->cd();
  gStyle->SetOptStat();


  //----muon distribution
  c0Mu = new TCanvas("c0Mu","muon distribution",1200,400);
  c0Mu.Divide(3,1);
  c0Mu.cd(1);
  hMuonPt->DrawCopy();
  c0Mu.cd(2);
  hMuonEta->SetMinimum(0);
  hMuonEta->DrawCopy();
  c0Mu.cd(3);
  hMuonPhi->SetMinimum(0);
  hMuonPhi->DrawCopy();
  c0Mu.Print(0,".eps");

  //----track distribution
  c0Tk = new TCanvas("c0Tk","track distribution",1200,400);
  c0Tk.Divide(3,1);
  c0Tk.cd(1);
  hTrackPt->DrawCopy();
  c0Tk.cd(2);
  hTrackEta->DrawCopy();
  c0Tk.cd(3);
  hTrackPhi->DrawCopy();


  //----hits population
  c1H = new TCanvas("c1H","hits populatoin",900,500);
  c1H.Divide(2,1);
  c1H.cd(1);
  hHitsB.SetMinimum(0.);
  hHitsB.DrawCopy(); 
  c1H.cd(2);
  hHitsE.SetMinimum(0.);
  hHitsE.DrawCopy();
  c1H.Print(0,".eps");

  //----hits population
  c1D = new TCanvas("c1D","dets populatoin",900,500);
  c1D.Divide(2,1);
  c1D.cd(1);
  hDetsB_100.SetMinimum(0.);
  hDetsB_100.DrawCopy(); 
  c1D.cd(2);
  hDetsE_100.SetMinimum(0.);
  hDetsE_100.DrawCopy();
  c1D.Print(0,".eps");

// goto end:

  //----hits efficiency
  c2 =  new TCanvas("c2","c2",-2);
  TH1D * heffH = DivideErr(hEfficHits_N,hEfficHits_D,"hEfficHits","B");
  heffH->SetMinimum(0.);
  heffH->SetMaximum(1.002);
  heffH->SetXTitle("eta");
  heffH->SetYTitle("efficiency");
  heffH->SetMarkerStyle(25);
  heffH->SetMarkerColor(4);
  heffH->DrawCopy("E");
  c2.Print(0,".eps");

  //----hits efficiency
  c2R =  new TCanvas("c2R","c2R",-2);
  TH1D * heffHR = DivideErr(hEfficHits_H,hEfficHits_N,"hEfficHitsR","B");
  heffHR->SetMinimum(0.);
  heffHR->SetMaximum(1.002);
  heffHR->SetXTitle("eta");
  heffHR->SetYTitle("efficiency");
  heffHR->SetMarkerStyle(25);
  heffHR->SetMarkerColor(4);
  heffHR->DrawCopy("E");
  c2R.Print(0,".eps");

  //--- efficiency for trigger wrt Mu 
  c3 =  new TCanvas("c3","c3",-2);
  TH1D * heffM = DivideErr(hEfficMu_N,hEfficMu_D,"hEfficMu","B");
  heffM->SetMinimum(0.);
  heffM->SetMaximum(1.002);
  heffM->SetXTitle("eta");
  heffM->SetYTitle("efficiency");
  heffM->SetMarkerStyle(25);
  heffM->SetMarkerColor(4);
  heffM->DrawCopy("E");
  c3.Print(0,".eps");

  //---- efficiency for trigger wrt Tk 
  c4 =  new TCanvas("c4","c4",-2);
  TH1D * heffT = DivideErr(hEfficTk_N,hEfficTk_D,"hEfficTk","B");
  heffT->SetMinimum(0.);
  heffT->SetMaximum(1.002);
  heffT->SetXTitle("eta");
  heffT->SetYTitle("efficiency");
  heffT->SetMarkerStyle(25);
  heffT->SetMarkerColor(4);
  heffT->DrawCopy("E");
  c4.Print(0,".eps");

  c5 = new TCanvas("c5","c5",-2);
  TH1D * heffMuPt = DivideErr(hEfficMuPt_N,hEfficMuPt_D,"hEfficMuPt","B");
  TH1D * heffMuPt7 = DivideErr(hEfficMuPt7_N,hEfficMuPt_D,"hEfficMuPt7","B");
  heffMuPt->SetMinimum(0.);
  heffMuPt->SetMaximum(1.002);
  heffMuPt->SetXTitle("p_{T} [GeV/c]");
  heffMuPt->SetYTitle("efficiency");
  heffMuPt->SetMarkerStyle(25);
  heffMuPt->SetMarkerColor(4);
  heffMuPt->Draw("E");
  heffMuPt7->SetMarkerStyle(23);
  heffMuPt7->SetMarkerColor(2);
  heffMuPt7->DrawCopy("E same");
  c5.Print(0,".eps");

  c6 = new TCanvas("c6","c6",-2);
  TH1D * heffTkPt = DivideErr(hEfficTkPt_N,hEfficTkPt_D,"hEfficTkPt","B");
  TH1D * heffTkPt7 = DivideErr(hEfficTkPt7_N,hEfficTkPt_D,"hEfficTkPt7","B");
  heffTkPt->SetMinimum(0.);
  heffTkPt->SetMaximum(1.002);
  heffTkPt->SetXTitle("p_{T} [GeV/c]");
  heffTkPt->SetYTitle("efficiency");
  heffTkPt->SetMarkerStyle(25);
  heffTkPt->SetMarkerColor(4);
  heffTkPt->Draw("E");
  heffTkPt7->SetMarkerStyle(23);
  heffTkPt7->SetMarkerColor(2);
  heffTkPt7->DrawCopy("E same");
  c6.Print(0,".eps");

  c7 = new TCanvas("c7","c7",1200,500);
  c7->Divide(2,1);
  c7->cd(1);
  hDistL1Rpc->DrawCopy("box");
  c7->cd(2);
  hDistL1Other->DrawCopy("box");
  c7.Print(0,".eps");

  c8= new TCanvas("c8","c8",-2);
  hL1RpcBX->SetXTitle("L1Rpc wrt L1 [BX]");
  Int_t integral = hL1RpcBX->Integral(); 
  hL1RpcBX->Scale(1./integral);
  gPad->SetLogy();
  hL1RpcBX->SetNdivisions(605,"X");
  hL1RpcBX->DrawCopy();
  c8.Print(0,".eps");
  c8.Print(0,".png");

//:end
  //----chamber  efficiency
  c9 =  new TCanvas("c9","c9",-1);
  c9->Divide(2,1);
  c9->cd(1);
  TH1D * heffChambBar = DivideErr(hEfficChambBar_N,hEfficChambBar_D,"hEfficChambBar","B");
  heffChambBar->SetMinimum(0.);
  heffChambBar->SetMaximum(1.002);
  heffChambBar->SetXTitle("layer");
  heffChambBar->SetYTitle("efficiency");
  heffChambBar->DrawCopy("E");
  c9->cd(2);
  TH1D * heffChambEnd = DivideErr(hEfficChambEnd_N,hEfficChambEnd_D,"hEfficChambEnd","B");
  heffChambEnd->SetMinimum(0.);
  heffChambEnd->SetMaximum(1.002);
  heffChambEnd->SetXTitle("layer");
  heffChambEnd->SetYTitle("efficiency");
  heffChambEnd->DrawCopy("E");
  c9.Print(0,".eps");

  //----chamber  efficiency
  c10 =  new TCanvas("c10","c10",-1);
  c10->Divide(2,1);
  c10->cd(1);
  hEffLumi->Draw();
  c10->cd(2);
  hGraph->SetMarkerStyle(25);
  hGraph->SetMarkerColor(2);
  hGraph->Draw("Paint E");
  c10.Print(0,".eps");
}

