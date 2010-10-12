{
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
  gROOT.LoadMacro("/afs/cern.ch/cms/L1/rpc/plots/macros/FigUtils.C");
//  MyDefaultStyle->cd();
  //gROOT.LoadMacro("RegionLabels.C");


//--
  TFile file("efficiencyHistos.root");
  file.ls();
//--------------------
  //MyDefaultStyle->cd();
  gStyle->SetOptStat(0);

  // goto c10;
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
  c0Mu.Print(0,".gif");

  //----track distribution
  c0Tk = new TCanvas("c0Tk","track distribution",1200,400);
  c0Tk.Divide(3,1);
  c0Tk.cd(1);
  hTrackPt->DrawCopy();
  c0Tk.cd(2);
  hTrackEta->DrawCopy();
  c0Tk.cd(3);
  hTrackPhi->DrawCopy();


//
// DET, HIT Efficiency vs ETA
//

  //----dets barrel
  c1EDB = new TCanvas("c1EDB","barrel dets populatoin",500,500); 
  TH1D * hDB1 = DivideErr(hEfficDetB_N1,hEfficHitDet_D,"hDB1","B"); 
  hDB1->SetMaximum(1.05);
  TH1D * hDB2 = DivideErr(hEfficDetB_N2,hEfficHitDet_D,"hDB2","B"); 
  TH1D * hDB3 = DivideErr(hEfficDetB_N3,hEfficHitDet_D,"hDB3","B"); 
  TH1D * hDB4 = DivideErr(hEfficDetB_N4,hEfficHitDet_D,"hDB4","B");
  TH1D * hDB5 = DivideErr(hEfficDetB_N5,hEfficHitDet_D,"hDB5","B");
  TH1D * hDB6 = DivideErr(hEfficDetB_N6,hEfficHitDet_D,"hDB5","B");
  hDB1->SetLineColor(1); hDB1->DrawCopy("hist");
  hDB2->SetLineColor(2); hDB2->DrawCopy("hist same");
  hDB3->SetLineColor(3); hDB3->DrawCopy("hist same");
  hDB4->SetLineColor(4); hDB4->DrawCopy("hist same");
  hDB5->SetLineColor(5); hDB5->DrawCopy("hist same");
  hDB6->SetLineColor(6); hDB6->DrawCopy("hist same");
  TLegend legend(0.2,0.2,1.9,0.5,"","");
  legend.AddEntry(hDB1,"layer 1");
  legend.AddEntry(hDB2,"layer 2");
  legend.AddEntry(hDB3,"layer 3");
  legend.AddEntry(hDB4,"layer 4");
  legend.AddEntry(hDB5,"layer 5");
  legend.AddEntry(hDB6,"layer 6");
  legend.Draw();
  c1EDB->Print(0,".eps");
  c1EDB->Print(0,".gif");

  //---hits barrel
  c1EHB = new TCanvas("c1EHB","barrel hits populatoin",500,500); 
  TH1D * hHB1 = DivideErr(hEfficHitB_N1,hEfficHitDet_D,"hHB1","B"); 
  hHB1->SetMaximum(1.05);
  TH1D * hHB2 = DivideErr(hEfficHitB_N2,hEfficHitDet_D,"hHB2","B"); 
  TH1D * hHB3 = DivideErr(hEfficHitB_N3,hEfficHitDet_D,"hHB3","B"); 
  TH1D * hHB4 = DivideErr(hEfficHitB_N4,hEfficHitDet_D,"hHB4","B");
  TH1D * hHB5 = DivideErr(hEfficHitB_N5,hEfficHitDet_D,"hHB5","B");
  TH1D * hHB6 = DivideErr(hEfficHitB_N6,hEfficHitDet_D,"hHB5","B");
  hHB1->SetLineColor(1); hHB1->DrawCopy("hist");
  hHB2->SetLineColor(2); hHB2->DrawCopy("hist same");
  hHB3->SetLineColor(3); hHB3->DrawCopy("hist same");
  hHB4->SetLineColor(4); hHB4->DrawCopy("hist same");
  hHB5->SetLineColor(5); hHB5->DrawCopy("hist same");
  hHB6->SetLineColor(6); hHB6->DrawCopy("hist same");
  TLegend legend2(0.2,0.2,1.9,0.5,"","");
  legend2.AddEntry(hHB1,"layer 1");
  legend2.AddEntry(hHB2,"layer 2");
  legend2.AddEntry(hHB3,"layer 3");
  legend2.AddEntry(hHB4,"layer 4");
  legend2.AddEntry(hHB5,"layer 5");
  legend2.AddEntry(hHB6,"layer 6");
  legend2.Draw();
  c1EHB->Print(0,".eps");
  c1EHB->Print(0,".gif");
  
  //
  // HIT AND LAYER EFFICIENCY AS ETA
  //
  //---dets endcap
  c1EDE = new TCanvas("c1EDE"," endcap dets populatoin",500,500); 
  TH1D * hDE1 = DivideErr(hEfficDetE_N1,hEfficHitDet_D,"hDE1","B"); 
  hDE1->SetMaximum(1.05);
  TH1D * hDE2 = DivideErr(hEfficDetE_N2,hEfficHitDet_D,"hDE2","B"); 
  TH1D * hDE3 = DivideErr(hEfficDetE_N3,hEfficHitDet_D,"hDE3","B"); 
  hDE1->SetLineColor(1); hDE1->DrawCopy("hist");
  hDE2->SetLineColor(2); hDE2->DrawCopy("hist same");
  hDE3->SetLineColor(3); hDE3->DrawCopy("hist same");
  TLegend legendDE(0.2,0.2,1.9,0.5,"","");
  legendDE.AddEntry(hDE1,"layer 1");
  legendDE.AddEntry(hDE2,"layer 2");
  legendDE.AddEntry(hDE3,"layer 3");
  legendDE.Draw();
  c1EDE->Print(0,".eps");
  c1EDE->Print(0,".gif");

  //---hits endcap
  c1EHE = new TCanvas("c1EHE"," endcap hits populatoin",500,500); 
  TH1D * hHE1 = DivideErr(hEfficHitE_N1,hEfficHitDet_D,"hDE1","B"); 
  hHE1->SetMaximum(1.05);
  TH1D * hHE2 = DivideErr(hEfficHitE_N2,hEfficHitDet_D,"hDE2","B"); 
  TH1D * hHE3 = DivideErr(hEfficHitE_N3,hEfficHitDet_D,"hDE3","B"); 
  hHE1->SetLineColor(1); hHE1->DrawCopy("hist");
  hHE2->SetLineColor(2); hHE2->DrawCopy("hist same");
  hHE3->SetLineColor(3); hHE3->DrawCopy("hist same");
  TLegend legendHE(0.2,0.2,1.9,0.5,"","");
  legendHE.AddEntry(hHE1,"layer 1");
  legendHE.AddEntry(hHE2,"layer 2");
  legendHE.AddEntry(hHE3,"layer 3");
  legendHE.Draw();
  c1EHE->Print(0,".eps");
  c1EHE->Print(0,".gif");

//
// HITS AND LAYER SUMMARY 
//
  c9 =  new TCanvas("c9","c9",-1);
  c9->Divide(2,1);
  c9->cd(1);
  TH1D * heffChambBar = DivideErr(hEfficChambBar_N,hEfficChambBar_D,"hEfficChambBar","B");
  heffChambBar->SetMinimum(0.4);
  heffChambBar->SetMaximum(1.002);
  heffChambBar->SetXTitle("layer");
  heffChambBar->SetYTitle("efficiency");
  heffChambBar->DrawCopy("E");
  heffChambBar->Print("all");
  c9->cd(2);
  TH1D * heffChambEnd = DivideErr(hEfficChambEnd_N,hEfficChambEnd_D,"hEfficChambEnd","B");
  //TH1D * heffChambEnd = hEfficChambEnd_N->DivideErr(hEfficChambEnd_D);
  hEfficChambEnd_D->Print("all");
  heffChambEnd->SetMinimum(0.4);
  heffChambEnd->SetMaximum(1.002);
  heffChambEnd->SetXTitle("layer");
  heffChambEnd->SetYTitle("efficiency");
  heffChambEnd->DrawCopy("E");
  heffChambEnd->Print("all");
  c9.Print(0,".eps");
  c9.Print(0,".gif");
//goto end;


//
// CROSSED LAYERS AND DETS
//
  //----number of crossed dets 
  Double_t sum4_6,sum3_6;
  c1D = new TCanvas("c1D","dets populatoin",900,500);
  c1D.Divide(2,1);
  c1D.cd(1);
//  hDetsB_100->Scale(1/hDetsB_100->Integral());
  sum4_6 =   hDetsB_100->GetBinContent(5)
           + hDetsB_100->GetBinContent(6)
           + hDetsB_100->GetBinContent(7);
  sum3_6 = sum4_6 + hDetsB_100->GetBinContent(4); 
  std::cout <<" BarrelDet (4-6): "<<sum4_6<<" (3-6): "<<sum3_6 <<std::endl;
  hDetsB_100.SetMinimum(0.);
  hDetsB_100.DrawCopy(); 
  c1D.cd(2);
//  hDetsE_100->Scale(1/hDetsE_100->Integral());
  std::cout <<" EndcapDet (3-3): "<< hDetsE_100->GetBinContent(4) <<std::endl;
  hDetsE_100.SetMinimum(0.);
  hDetsE_100.DrawCopy();
  c1D.Print(0,".eps");
  c1D.Print(0,".gif");

  //----number of hits left  
  c1H = new TCanvas("c1H","hits populatoin",900,500);
  c1H.Divide(2,1);
  c1H.cd(1);
  hHitsB->Scale(1/hHitsB->Integral());
  sum4_6 =   hHitsB->GetBinContent(5)
           + hHitsB->GetBinContent(6)
           + hHitsB->GetBinContent(7);
  sum3_6 = sum4_6 + hHitsB->GetBinContent(4); 
  std::cout <<" BarrelHit (4-6): "<<sum4_6<<" (3-6): "<<sum3_6 <<std::endl;
  hHitsB.SetMinimum(0.);
  hHitsB.DrawCopy(); 
  c1H.cd(2);
  hHitsE->Scale(1/hHitsE->Integral());
  std::cout <<" EndcapHit (3-3): "<< hHitsE->GetBinContent(4) <<std::endl;
  hHitsE.SetMinimum(0.);
  hHitsE.DrawCopy();
  c1H.Print(0,".eps");
  c1H.Print(0,".gif");

  //
  // EXPECTED EFFICIENCY FROM HITS
  //

  //----dets efficiency
  c2 =  new TCanvas("c2","det in concidence for trigger",-2);
  Double_t me = hEfficGeom_M->Integral(5,7)+hEfficGeom_M->Integral(27,29);
  Double_t mb = hEfficGeom_M->Integral(12,28);
  Double_t de = hEfficGeom_D->Integral(5,7)+hEfficGeom_D->Integral(27,29);
  Double_t db = hEfficGeom_D->Integral(12,28);
//  Double_t me = hEfficGeom_M->Integral(2,7)+hEfficGeom_M->Integral(58,63);
//  Double_t mb = hEfficGeom_M->Integral(17,48);
//  Double_t de = hEfficGeom_D->Integral(2,7)+hEfficGeom_D->Integral(58,63);
//  Double_t db = hEfficGeom_D->Integral(17,48);
  std::cout <<"DET:  Barrel: "<<db/mb <<" Endcap: "<<de/me<< std::endl;
  TH1D * heffD = DivideErr( hEfficGeom_D, hEfficGeom_M,"hEfficGeomD","B");
  heffD->SetMinimum(0.);
  heffD->SetMaximum(1.002);
  heffD->SetXTitle("eta");
  heffD->SetYTitle("efficiency");
  heffD->SetMarkerStyle(25);
  heffD->SetMarkerColor(4);
  heffD->DrawCopy("E");
  c2.Print(0,".eps");
  c2.Print(0,".gif");

  //----hits efficiency
  c2R =  new TCanvas("c2R"," hit in concidence for trigger",-2);
  Double_t me = hEfficGeom_M->Integral(5,7)+hEfficGeom_M->Integral(27,29);
  Double_t mb = hEfficGeom_M->Integral(12,28);
  Double_t he = hEfficGeom_H->Integral(5,7)+hEfficGeom_H->Integral(27,29);
  Double_t hb = hEfficGeom_H->Integral(12,28);
  std::cout <<"HIT:  Barrel: "<<hb/mb <<" Endcap: "<<he/me<< std::endl;
  TH1D * heffH = DivideErr(hEfficGeom_H,hEfficGeom_M,"hEfficGeomH","B");
  heffH->SetMinimum(0.);
  heffH->SetMaximum(1.002);
  heffH->SetXTitle("eta");
  heffH->SetYTitle("efficiency");
  heffH->SetMarkerStyle(25);
  heffH->SetMarkerColor(4);
  heffH->DrawCopy("E");
  c2R.Print(0,".eps");
  c2R.Print(0,".gif");

  //----trigg efficiency
  c2R =  new TCanvas("c2R"," trg in hit concidence for trigger",-2);
  Double_t me = hEfficGeom_M->Integral(5,7)+hEfficGeom_M->Integral(27,29);
  Double_t mb = hEfficGeom_M->Integral(12,28);
  Double_t te = hEfficGeom_T->Integral(5,7)+hEfficGeom_T->Integral(27,29);
  Double_t tb = hEfficGeom_T->Integral(12,28);
  std::cout <<"L1R:  Barrel: "<<tb/mb <<" MB="<<mb <<" Endcap: "<<te/me<<" ME="<<me<< std::endl;
  TH1D * heffT = DivideErr(hEfficGeom_T,hEfficGeom_M,"hEfficGeomT","B");
  heffT->SetMinimum(0.);
  heffT->SetMaximum(1.002);
  heffT->SetXTitle("eta");
  heffT->SetYTitle("efficiency");
  heffT->SetMarkerStyle(25);
  heffT->SetMarkerColor(4);
  heffT->DrawCopy("E");
  c2R.Print(0,".eps");
  c2R.Print(0,".gif");

  //--- efficiency for trigger wrt Mu 
  c3 =  new TCanvas("c3","c3",-2);
  TH1D * heffM = DivideErr(hEfficMu_N,hEfficMu_D,"hEfficMu","B");
  Double_t me = hEfficMu_D->Integral(5,7)+hEfficMu_D->Integral(27,29);
  Double_t mb = hEfficMu_D->Integral(12,28);
  Double_t te = hEfficMu_N->Integral(5,7)+ hEfficMu_N->Integral(27,29);
  Double_t tb = hEfficMu_N->Integral(12,28);
  Double_t ta = hEfficMu_N->Integral(1,33);
  Double_t ma = hEfficMu_D->Integral(1,33);
  std::cout <<"L1R:  Barrel: "<<tb/mb<<" MB="<<mb <<" Endcap: "<<te/me<<" ME="<<me
            <<" average: "<<ta/ma << std::endl;
  heffM->SetMinimum(0.);
  heffM->SetMaximum(1.002);
  heffM->SetXTitle("eta");
  heffM->SetYTitle("efficiency");
  heffM->SetMarkerStyle(25);
  heffM->SetMarkerColor(4);
  heffM->DrawCopy("E");
  c3.Print(0,".eps");
  c3.Print(0,".gif");
  //goto end;

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
  c4.Print(0,".gif");

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
  c5.Print(0,".gif");

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
  c6.Print(0,".gif");

  c7 = new TCanvas("c7","c7",1200,500);
  c7->Divide(2,1);
  c7->cd(1);
  hDistL1Rpc->DrawCopy("box");
  c7->cd(2);
  hDistL1Other->DrawCopy("box");
  c7.Print(0,".eps");
  c7.Print(0,".gif");

  c8= new TCanvas("c8","c8",-2);
  hL1RpcBX->SetXTitle("L1Rpc wrt L1 [BX]");
  Int_t integral = hL1RpcBX->Integral(); 
  hL1RpcBX->Scale(1./integral);
  gPad->SetLogy();
  hL1RpcBX->SetNdivisions(605,"X");
  hL1RpcBX->DrawCopy();
  c8.Print(0,".eps");
  c8.Print(0,".gif");
 c10:
  //----chamber  efficiency
  c10 =  new TCanvas("c10","c10",-1);
  c10->Divide(2,1);
  c10->cd(1);
  hEffRun->Draw();
  c10->cd(2);

  hGraphRun->SetMaximum(1.);
  hGraphRun->SetMinimum(0.);

  hGraphRun->SetMarkerStyle(25);
  hGraphRun->SetMarkerColor(2);
  
  hGraphRun->Draw("Paint E");
  c10.Print(0,".eps");
  c10.Print(0,".gif");

  //----chamber  efficiency
  c10l =  new TCanvas("c10l","c10l",-1);
  c10l->Divide(2,1);
  c10l->cd(1);
  hEffLumi->Draw();
  c10l->cd(2);
 
  hGraphLumi->SetMaximum(1.);
  hGraphLumi->SetMinimum(0.);
 
  hGraphLumi->SetMarkerStyle(25);
  hGraphLumi->SetMarkerColor(2);
  hGraphLumi->Draw("Paint E");
  c10l.Print(0,".eps");
  c10l.Print(0,".gif");
  
end:
}


