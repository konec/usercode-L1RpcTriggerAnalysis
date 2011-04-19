{
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
  gROOT.LoadMacro("FigUtils.C");
 // gROOT.LoadMacro("FigUtils_1.C");
  MyDefaultStyle->cd();

//--
  TFile file("efficiencyHistos.root");
  file.ls();
//-------------------- 
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(1);

  //----muon distribution
  c0Mu = new TCanvas("c0Mu","muon distribution",1200,400);
  c0Mu->SetFillColor(0);
  c0Mu.Divide(3,1);
  c0Mu.cd(1);
  hMuonPt->DrawCopy();
  c0Mu.cd(2);
  hMuonEta->SetMinimum(0);
  hMuonEta->DrawCopy();
  c0Mu.cd(3);
  hMuonPhi->SetMinimum(0);
  hMuonPhi->DrawCopy();
  c0Mu.Print(0,".png");
  
  //----track distribution
  c0Tk = new TCanvas("c0Tk","track distribution",1200,400);
  c0Tk->SetFillColor(0);
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
  c1EDB->SetFillColor(0);
  TH1D * hDB1 = DivideErr(hEfficDetB_N1,hEfficHitDet_D,"hDB1","B"); 
  hDB1->SetMaximum(1.05);
  hDB1->SetTitle("Fraction of muons crossing Barrel layer");
  TH1D * hDB2 = DivideErr(hEfficDetB_N2,hEfficHitDet_D,"hDB2","B"); 
  TH1D * hDB3 = DivideErr(hEfficDetB_N3,hEfficHitDet_D,"hDB3","B"); 
  TH1D * hDB4 = DivideErr(hEfficDetB_N4,hEfficHitDet_D,"hDB4","B");
  TH1D * hDB5 = DivideErr(hEfficDetB_N5,hEfficHitDet_D,"hDB5","B");
  TH1D * hDB6 = DivideErr(hEfficDetB_N6,hEfficHitDet_D,"hDB6","B");
  hDB1->SetLineColor(1); hDB1->DrawCopy("hist");
  hDB2->SetLineColor(2); hDB2->DrawCopy("hist same");
  hDB3->SetLineColor(3); hDB3->DrawCopy("hist same");
  hDB4->SetLineColor(4); hDB4->DrawCopy("hist same");
  hDB5->SetLineColor(5); hDB5->DrawCopy("hist same");
  hDB6->SetLineColor(6); hDB6->DrawCopy("hist same");
  TLegend legend(0.2,0.2,1.9,0.5,"","");
//TLegend legend(1.3,0.7,1.9,1.0,"","");
  legend.AddEntry(hDB1,"layer 1","l");
  legend.AddEntry(hDB2,"layer 2","l");
  legend.AddEntry(hDB3,"layer 3","l");
  legend.AddEntry(hDB4,"layer 4","l");
  legend.AddEntry(hDB5,"layer 5","l");
  legend.AddEntry(hDB6,"layer 6","l");
  legend.Draw();
  c1EDB->Print(0,".png");

  //---hits barrel
  c1EHB = new TCanvas("c1EHB","barrel hits populatoin",500,500); 
  c1EHB->SetFillColor(0);
  TH1D * hHB1 = DivideErr(hEfficHitB_N1,hEfficHitDet_D,"hHB1","B"); 
  hHB1->SetMaximum(1.05);
  hHB1->SetTitle("Fraction of muons hitting Barrel layer");
  TH1D * hHB2 = DivideErr(hEfficHitB_N2,hEfficHitDet_D,"hHB2","B"); 
  TH1D * hHB3 = DivideErr(hEfficHitB_N3,hEfficHitDet_D,"hHB3","B"); 
  TH1D * hHB4 = DivideErr(hEfficHitB_N4,hEfficHitDet_D,"hHB4","B");
  TH1D * hHB5 = DivideErr(hEfficHitB_N5,hEfficHitDet_D,"hHB5","B");
  TH1D * hHB6 = DivideErr(hEfficHitB_N6,hEfficHitDet_D,"hHB6","B");
  hHB1->SetLineColor(1); hHB1->DrawCopy("hist");
  hHB2->SetLineColor(2); hHB2->DrawCopy("hist same");
  hHB3->SetLineColor(3); hHB3->DrawCopy("hist same");
  hHB4->SetLineColor(4); hHB4->DrawCopy("hist same");
  hHB5->SetLineColor(5); hHB5->DrawCopy("hist same");
  hHB6->SetLineColor(6); hHB6->DrawCopy("hist same");
  TLegend legend2(0.2,0.2,1.9,0.5,"","");
//TLegend legend2(1.3,0.7,1.9,1.0,"","");
  legend2.AddEntry(hHB1,"layer 1","l");
  legend2.AddEntry(hHB2,"layer 2","l");
  legend2.AddEntry(hHB3,"layer 3","l");
  legend2.AddEntry(hHB4,"layer 4","l");
  legend2.AddEntry(hHB5,"layer 5","l");
  legend2.AddEntry(hHB6,"layer 6","l");
  legend2.Draw();
  c1EHB->Print(0,".png");
  
  //
  // HIT AND LAYER EFFICIENCY AS ETA
  //
  //---dets endcap
  c1EDE = new TCanvas("c1EDE"," endcap dets populatoin",500,500); 
  c1EDE->SetFillColor(0);
  TH1D * hDE1 = DivideErr(hEfficDetE_N1,hEfficHitDet_D,"hDE1","B"); 
  hDE1->SetMaximum(1.05);
  hDE1->SetTitle("Fraction of muons crossing Endcap layer");
  TH1D * hDE2 = DivideErr(hEfficDetE_N2,hEfficHitDet_D,"hDE2","B"); 
  TH1D * hDE3 = DivideErr(hEfficDetE_N3,hEfficHitDet_D,"hDE3","B"); 
  hDE1->SetLineColor(1); hDE1->DrawCopy("hist");
  hDE2->SetLineColor(2); hDE2->DrawCopy("hist same");
  hDE3->SetLineColor(3); hDE3->DrawCopy("hist same");
//TLegend legendDE(0.2,0.2,1.9,0.5,"","");
  TLegend legendDE(-0.5,0.8,.5,1,"","");
  legendDE.AddEntry(hDE1,"layer 1","l");
  legendDE.AddEntry(hDE2,"layer 2","l");
  legendDE.AddEntry(hDE3,"layer 3","l");
  legendDE.Draw();
  c1EDE->Print(0,".png");

  //---hits endcap
  c1EHE = new TCanvas("c1EHE"," endcap hits populatoin",500,500); 
  c1EHE->SetFillColor(0);
  TH1D * hHE1 = DivideErr(hEfficHitE_N1,hEfficHitDet_D,"hDE1","B"); 
  hHE1->SetMaximum(1.05);
  hHE1->SetTitle("Fraction of muons hitting Endcap layer");
  TH1D * hHE2 = DivideErr(hEfficHitE_N2,hEfficHitDet_D,"hDE2","B"); 
  TH1D * hHE3 = DivideErr(hEfficHitE_N3,hEfficHitDet_D,"hDE3","B"); 
  hHE1->SetLineColor(1); hHE1->DrawCopy("hist");
  hHE2->SetLineColor(2); hHE2->DrawCopy("hist same");
  hHE3->SetLineColor(3); hHE3->DrawCopy("hist same");
  TLegend legendHE(0.2,0.2,1.9,0.5,"","");
//TLegend legendHE(-0.5,0.8,.5,1,"","");
  legendHE.AddEntry(hHE1,"layer 1","l");
  legendHE.AddEntry(hHE2,"layer 2","l");
  legendHE.AddEntry(hHE3,"layer 3","l");
  legendHE.Draw();
  c1EHE->Print(0,".png");

//
// HITS AND LAYER SUMMARY 
//
  c9 =  new TCanvas("c9","c9",-1);
  c9->SetFillColor(0);
  c9->Divide(2,1);
  c9->cd(1);
  TH1D * heffChambBar = DivideErr(hEfficChambBar_N,hEfficChambBar_D,"hEfficChambBar","B");
  heffChambBar->SetMinimum(0.4);
  heffChambBar->SetMaximum(1.002);
  heffChambBar->SetXTitle("layer");
  heffChambBar->SetYTitle("efficiency");
  TH1D * heffChambBarDet = (TH1D*)heffChambBar->Clone();
  heffChambBarDet->SetBinContent(1,0.925);
  heffChambBarDet->SetBinContent(2,0.925);
  heffChambBarDet->SetBinContent(3,0.925);
  heffChambBarDet->SetBinContent(4,0.925);
  heffChambBarDet->SetBinContent(5,0.925);
  heffChambBarDet->SetBinContent(6,0.925);
  heffChambBarDet->SetLineColor(kBlue);
  heffChambBar->DrawCopy("E");
  heffChambBarDet->DrawCopy("E same");
  heffChambBar->Print("all");

  c9->cd(2);
  TH1D * heffChambEnd = DivideErr(hEfficChambEnd_N,hEfficChambEnd_D,"hEfficChambEnd","B");
  hEfficChambEnd_D->Print("all");
  heffChambEnd->SetMinimum(0.4);
  heffChambEnd->SetMaximum(1.002);
  heffChambEnd->SetXTitle("layer");
  heffChambEnd->SetYTitle("efficiency");
  TH1D * heffChambEndDet = (TH1D*)heffChambEnd->Clone();
  heffChambEndDet->SetBinContent(1,0.925);
  heffChambEndDet->SetBinContent(2,0.925);
  heffChambEndDet->SetBinContent(3,0.925);
  heffChambEndDet->SetLineColor(kBlue);
  heffChambEnd->DrawCopy("E");
  heffChambEndDet->DrawCopy("E same");
  heffChambEnd->Print("all");
  c9.Print(0,".png");

//
// CROSSED LAYERS AND DETS
//
  //----number of crossed dets 
  Double_t sum4_6,sum3_6;
  c1D = new TCanvas("c1D","dets populatoin",900,500);
  c1D->SetFillColor(0);  
  c1D.Divide(2,1);
  c1D.cd(1);
  hDetsB_100->Scale(1/hDetsB_100->Integral());
  sum4_6 =   hDetsB_100->GetBinContent(5)
           + hDetsB_100->GetBinContent(6)
           + hDetsB_100->GetBinContent(7);
  sum3_6 = sum4_6 + hDetsB_100->GetBinContent(4); 
  std::cout <<" BarrelDet (4-6): "<<sum4_6<<" (3-6): "<<sum3_6 <<std::endl;
  hDetsB_100.SetMinimum(0.);
  hDetsB_100.DrawCopy(); 
  c1D.cd(2);
  hDetsE_100->Scale(1/hDetsE_100->Integral());
  std::cout <<" EndcapDet (3-3): "<< hDetsE_100->GetBinContent(4) <<std::endl;
  hDetsE_100.SetMinimum(0.);
  hDetsE_100.DrawCopy();
  c1D.Print(0,".png");

  //----number of hits left  
  c1H = new TCanvas("c1H","hits populatoin",900,500);
  c1H->SetFillColor(0);  
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
  c1H.Print(0,".png");

  //
  // EXPECTED EFFICIENCY FROM HITS
  //
  //----dets efficiency
  Double_t me = hEfficGeom_M->Integral(5,7)+hEfficGeom_M->Integral(27,29);
  Double_t mb = hEfficGeom_M->Integral(12,28);
  Double_t de = hEfficGeom_D->Integral(5,7)+hEfficGeom_D->Integral(27,29);
  Double_t db = hEfficGeom_D->Integral(12,28);
  Double_t de_3z6 = hEfficGeom_D_3z6->Integral(5,7)+hEfficGeom_D_3z6->Integral(27,29);
  Double_t db_3z6 = hEfficGeom_D_3z6->Integral(12,28);
  std::cout <<"4z6: DET:  Barrel: "<<db/mb <<" Endcap: "<<de/me<< std::endl;
  std::cout <<"3z6: DET:  Barrel: "<<db_3z6/mb <<" Endcap: "<<de_3z6/me<< std::endl;
  TH1D * heffD = DivideErr( hEfficGeom_D, hEfficGeom_M,"hEfficGeomD","B");
  TH1D * heffD_3z6 = DivideErr( hEfficGeom_D_3z6, hEfficGeom_M,"hEfficGeomD_3z6","B");
  heffD->SetMinimum(0.);
  heffD->SetMaximum(1.002);
  heffD->SetXTitle("eta");
  heffD->SetYTitle("efficiency");
  heffD_3z6->SetMinimum(0.);   
  heffD_3z6->SetMaximum(1.002);
  heffD_3z6->SetXTitle("eta");
  heffD_3z6->SetYTitle("efficiency");
  heffD->SetLineColor(kRed);
  heffD_3z6->SetLineColor(kRed);

  //----hits efficiency
  Double_t he = hEfficGeom_H->Integral(5,7)+hEfficGeom_H->Integral(27,29);
  Double_t hb = hEfficGeom_H->Integral(12,28);
  Double_t he_3z6 = hEfficGeom_H_3z6->Integral(5,7)+hEfficGeom_H_3z6->Integral(27,29);
  Double_t hb_3z6 = hEfficGeom_H_3z6->Integral(12,28);
  std::cout <<"4z6: HIT:  Barrel: "<<hb/mb <<" Endcap: "<<he/me<< std::endl;
  std::cout <<"3z6: HIT:  Barrel: "<<hb_3z6/mb <<" Endcap: "<<he_3z6/me<< std::endl;
  TH1D * heffH = DivideErr(hEfficGeom_H,hEfficGeom_M,"hEfficGeomH","B");
  TH1D * heffH_3z6 = DivideErr(hEfficGeom_H_3z6,hEfficGeom_M,"hEfficGeomH_3z6","B");
  heffH->SetMinimum(0.);
  heffH->SetMaximum(1.002);
  heffH->SetXTitle("eta");
  heffH->SetYTitle("efficiency");
  heffH_3z6->SetMinimum(0.);   
  heffH_3z6->SetMaximum(1.002);
  heffH_3z6->SetXTitle("eta");
  heffH_3z6->SetYTitle("efficiency");
  heffH->SetLineColor(kGreen);
  heffH_3z6->SetLineColor(kGreen);

  //----trigg efficiency
  Double_t te = hEfficGeom_T->Integral(5,7)+hEfficGeom_T->Integral(27,29);
  Double_t tb = hEfficGeom_T->Integral(12,28);
  Double_t te_3z6 = hEfficGeom_T_3z6->Integral(5,7)+hEfficGeom_T_3z6->Integral(27,29);
  Double_t tb_3z6 = hEfficGeom_T_3z6->Integral(12,28);
  std::cout <<"4z6: L1R:  Barrel: "<<tb/mb <<" MB="<<mb <<" Endcap: "<<te/me<<" ME="<<me<< std::endl;
  std::cout <<"3z6: L1R:  Barrel: "<<tb_3z6/mb <<" MB="<<mb <<" Endcap: "<<te_3z6/me<<" ME="<<me<< std::endl;
  TH1D * heffT = DivideErr(hEfficGeom_T,hEfficGeom_M,"hEfficGeomT","B");
  TH1D * heffT_3z6 = DivideErr(hEfficGeom_T_3z6,hEfficGeom_M,"hEfficGeomT_3z6","B");
  heffT->SetMinimum(0.);
  heffT->SetMaximum(1.002);
  heffT->SetXTitle("eta");
  heffT->SetYTitle("efficiency");
  heffT_3z6->SetMinimum(0.);
  heffT_3z6->SetMaximum(1.002);
  heffT_3z6->SetXTitle("eta"); 
  heffT_3z6->SetYTitle("efficiency");
  heffT->SetLineColor(kBlack);
  heffT_3z6->SetLineColor(kBlack);

  //--- efficiency for trigger wrt Mu 
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
  heffM->SetLineColor(kBlue);

  TLegend legend3(-0.5,0.1,1.5,0.2,"","");
  legend3.AddEntry(heffD,"Dets in coincidence required by trigger");
  legend3.AddEntry(heffH,"Hits in coincidence required by trigger");
  legend3.AddEntry(heffM,"L1RPC");
//legend3.AddEntry(heffT,"required Hits & L1RPC");

  c2 =  new TCanvas("c2","det in concidence for trigger",-2);
  c2->SetFillColor(0);

  heffM->DrawCopy("hist");
  heffH->DrawCopy("hist same");
  heffD->DrawCopy("hist same");

  legend3.Draw();
  c2->Print(0,".png");

  c2_3z6 = new TCanvas("c2_3z6","det in concidence for trigger",-2);
  c2_3z6->SetFillColor(0);

  heffM->DrawCopy("hist");
  heffH_3z6->DrawCopy("hist same");
  heffD_3z6->DrawCopy("hist same");
  
  legend3.Draw();
  c2_3z6->Print(0,".png");
  
 /* //efficiency 2D
  
  c2_2D = new TCanvas("c2_2D","c2_2D",1200,500);
  c2_2D->SetFillColor(0);
  c2_2D->Divide(2,1);
  c2_2D->cd(1);
  cout << hDistN<<endl;
  cout << hDistD<<endl;
 // TH2D* hDistD = (TH2D*)file->Get("hDistD");
    hDistN->DrawCopy("B");
 //  hDistN->Divide(hDistD);
  TH2D * heff2D = DivideErr( hDistN, hDistD,"heff2D","B");
   heff2D->DrawCopy("B");

  c2_2D->cd(2);
    TH2D * heff2D_good = DivideErr( hDistN_good, hDistD_good,1,1,"COLZ");
  heff2D_good->DrawCopy("COLZ");
  c2_2D.Print(0,".png");*/

  //--- efficiency for trigger wrt Mu 
  c3 =  new TCanvas("c3","c3",-2);
  c3->SetFillColor(0);
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
  c3.Print(0,".png");

  //---- efficiency for trigger wrt Tk 
  c4 =  new TCanvas("c4","c4",-2);
  c4->SetFillColor(0);
  TH1D * heffT = DivideErr(hEfficTk_N,hEfficTk_D,"hEfficTk","B");
  heffT->SetMinimum(0.);
  heffT->SetMaximum(1.002);
  heffT->SetXTitle("eta");
  heffT->SetYTitle("efficiency");
  heffT->SetMarkerStyle(25);
  heffT->SetMarkerColor(4);
  heffT->DrawCopy("E");
  c4.Print(0,".png");

  c5 = new TCanvas("c5","c5",-2);
  c5->SetFillColor(0);
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
  c5.Print(0,".png");
  
  //barrel
  
  c5_barrel = new TCanvas("c5_barrel","c5_barrel",-2);
  c5_barrel->SetFillColor(0);
  TH1D * heffMuPt_barrel = DivideErr(hEfficMuPt_N_barrel,hEfficMuPt_D_barrel,"hEfficMuPt_barrel","B");
  TH1D * heffMuPt7_barrel = DivideErr(hEfficMuPt7_N_barrel,hEfficMuPt_D_barrel,"hEfficMuPt7_barrel","B");
  heffMuPt_barrel->SetMinimum(0.);
  heffMuPt_barrel->SetMaximum(1.002);
  heffMuPt_barrel->SetXTitle("p_{T} [GeV/c]");
  heffMuPt_barrel->SetYTitle("efficiency");
  heffMuPt_barrel->SetMarkerStyle(25);
  heffMuPt_barrel->SetMarkerColor(4);
  heffMuPt_barrel->Draw("E");
  heffMuPt7_barrel->SetMarkerStyle(23);
  heffMuPt7_barrel->SetMarkerColor(2);
  heffMuPt7_barrel->DrawCopy("E same");
  c5_barrel.Print(0,".png");
  
    //overlap
  
  c5_overlap = new TCanvas("c5_overlap","c5_overlap",-2);
  c5_overlap->SetFillColor(0);
  TH1D * heffMuPt_overlap = DivideErr(hEfficMuPt_N_overlap,hEfficMuPt_D_overlap,"hEfficMuPt_overlap","B");
  TH1D * heffMuPt7_overlap = DivideErr(hEfficMuPt7_N_overlap,hEfficMuPt_D_overlap,"hEfficMuPt7_overlap","B");
  heffMuPt_overlap->SetMinimum(0.);
  heffMuPt_overlap->SetMaximum(1.002);
  heffMuPt_overlap->SetXTitle("p_{T} [GeV/c]");
  heffMuPt_overlap->SetYTitle("efficiency");
  heffMuPt_overlap->SetMarkerStyle(25);
  heffMuPt_overlap->SetMarkerColor(4);
  heffMuPt_overlap->Draw("E");
  heffMuPt7_overlap->SetMarkerStyle(23);
  heffMuPt7_overlap->SetMarkerColor(2);
  heffMuPt7_overlap->DrawCopy("E same");
  c5_overlap.Print(0,".png");

  
    //endcap
  
  c5_endcap = new TCanvas("c5_endcap","c5_endcap",-2);
  c5_endcap->SetFillColor(0);
  TH1D * heffMuPt_endcap = DivideErr(hEfficMuPt_N_endcap,hEfficMuPt_D_endcap,"hEfficMuPt_endcap","B");
  TH1D * heffMuPt7_endcap = DivideErr(hEfficMuPt7_N_endcap,hEfficMuPt_D_endcap,"hEfficMuPt7_endcap","B");
  heffMuPt_endcap->SetMinimum(0.);
  heffMuPt_endcap->SetMaximum(1.002);
  heffMuPt_endcap->SetXTitle("p_{T} [GeV/c]");
  heffMuPt_endcap->SetYTitle("efficiency");
  heffMuPt_endcap->SetMarkerStyle(25);
  heffMuPt_endcap->SetMarkerColor(4);
  heffMuPt_endcap->Draw("E");
  heffMuPt7_endcap->SetMarkerStyle(23);
  heffMuPt7_endcap->SetMarkerColor(2);
  heffMuPt7_endcap->DrawCopy("E same");
  c5_endcap.Print(0,".png");
  
    //endcapN
  
  c5_endcapN = new TCanvas("c5_endcapN","c5_endcapN",-2);
  c5_endcapN->SetFillColor(0);
  TH1D * heffMuPt_endcapN = DivideErr(hEfficMuPt_N_endcapN,hEfficMuPt_D_endcapN,"hEfficMuPt_endcapN","B");
  TH1D * heffMuPt7_endcapN = DivideErr(hEfficMuPt7_N_endcapN,hEfficMuPt_D_endcapN,"hEfficMuPt7_endcapN","B");
  heffMuPt_endcapN->SetMinimum(0.);
  heffMuPt_endcapN->SetMaximum(1.002);
  heffMuPt_endcapN->SetXTitle("p_{T} [GeV/c]");
  heffMuPt_endcapN->SetYTitle("efficiency");
  heffMuPt_endcapN->SetMarkerStyle(25);
  heffMuPt_endcapN->SetMarkerColor(4);
  heffMuPt_endcapN->Draw("E");
  heffMuPt7_endcapN->SetMarkerStyle(23);
  heffMuPt7_endcapN->SetMarkerColor(2);
  heffMuPt7_endcapN->DrawCopy("E same");
  c5_endcapN.Print(0,".png");

    //endcapP
  
  c5_endcapP = new TCanvas("c5_endcapP","c5_endcapP",-2);
  c5_endcapP->SetFillColor(0);
  TH1D * heffMuPt_endcapP = DivideErr(hEfficMuPt_N_endcapP,hEfficMuPt_D_endcapP,"hEfficMuPt_endcapP","B");
  TH1D * heffMuPt7_endcapP = DivideErr(hEfficMuPt7_N_endcapP,hEfficMuPt_D_endcapP,"hEfficMuPt7_endcapP","B");
  heffMuPt_endcapP->SetMinimum(0.);
  heffMuPt_endcapP->SetMaximum(1.002);
  heffMuPt_endcapP->SetXTitle("p_{T} [GeV/c]");
  heffMuPt_endcapP->SetYTitle("efficiency");
  heffMuPt_endcapP->SetMarkerStyle(25);
  heffMuPt_endcapP->SetMarkerColor(4);
  heffMuPt_endcapP->Draw("E");
  heffMuPt7_endcapP->SetMarkerStyle(23);
  heffMuPt7_endcapP->SetMarkerColor(2);
  heffMuPt7_endcapP->DrawCopy("E same");
  c5_endcapP.Print(0,".png");

  c6 = new TCanvas("c6","c6",-2);
  c6->SetFillColor(0);
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
  c6.Print(0,".png");

  c7 = new TCanvas("c7","c7",1200,500);
  c7->SetFillColor(0);
  c7->Divide(2,1);
  c7->cd(1);
  hDistL1Rpc->DrawCopy("box");
  c7->cd(2);
  hDistL1Other->DrawCopy("box");
  c7.Print(0,".png");

  c8= new TCanvas("c8","c8",-2);
  c8->SetFillColor(0);
  hL1RpcBX->SetXTitle("L1Rpc wrt L1 [BX]");
  Int_t integral = hL1RpcBX->Integral(); 
  hL1RpcBX->Scale(1./integral);
  gPad->SetLogy();
  hL1RpcBX->SetNdivisions(605,"X");
  hL1RpcBX->DrawCopy();
  c8.Print(0,".png");

  //----chamber  efficiency
  c10 =  new TCanvas("c10","c10",-1);  
  c10->SetFillColor(0);
  c10->Divide(2,1);
  c10->cd(1);
  hEffRun->Draw();
  c10->cd(2);

  hGraphRun->SetMaximum(1.);
  hGraphRun->SetMinimum(0.); 
  hGraphRun->SetMarkerStyle(25);
  hGraphRun->SetMarkerColor(2);
  hGraphRun->Draw("Paint E");
  c10.Print(0,".png");

  //----chamber  efficiency
  c10l =  new TCanvas("c10l","c10l",-1);
  c10l->SetFillColor(0);
  c10l->Divide(2,1);
  c10l->cd(1);
  hEffLumi->Draw();
  c10l->cd(2);
 
  hGraphLumi->SetMaximum(1.);
  hGraphLumi->SetMinimum(0.);
 
  hGraphLumi->SetMarkerStyle(25);
  hGraphLumi->SetMarkerColor(2);
  hGraphLumi->Draw("Paint E");
  c10l.Print(0,".png");

  //for approval
 //--- propability of having trigger (there was a trigger if there was a hit)
  TH1D * heffTH_3z6 = DivideErr(hEfficGeom_T_3z6,hEfficGeom_H_3z6,"hEfficGeomTH","B");
  TH1D * heffTH = DivideErr(hEfficGeom_T,hEfficGeom_H,"hEfficGeomTH","B");

  c2TH =  new TCanvas("c2TH","c2TH",1200,600);

  heffTH->SetMinimum(0.);
  heffTH->SetMaximum(1.102);
  heffTH->SetXTitle("#eta");
  heffTH->SetYTitle("Efficiency");  


  heffTH->SetLineColor(kBlack);
  heffTH->SetFillStyle(3003);
  heffTH->SetFillColor(kBlack);
 // heffTH->DrawCopy("hist");

  heffTH_3z6->SetMinimum(0.);
  heffTH_3z6->SetMaximum(1.102);
  heffTH_3z6->SetXTitle("#eta");
  heffTH_3z6->SetYTitle("Efficiency");


  heffTH_3z6->SetLineColor(kBlack);
  heffTH_3z6->SetFillStyle(3003);
  heffTH_3z6->SetFillColor(kBlack);
  heffTH_3z6->DrawCopy("hist");
 
  TLine *line = new TLine(-1.6,1.0,1.6,1.0);
  line->SetLineStyle(3);
  line->Draw();

 /* TFile file1("efficiencyHistos2.root");
  file1.ls();
 //--- propability of having trigger (there was a trigger if there was a hit)
  TH1D * heffTH1 = DivideErr(hEfficGeom_T,hEfficGeom_H,"hEfficGeomTH","B");
  heffTH1->SetMinimum(0.);
  heffTH1->SetMaximum(1.102);

  heffTH1->SetLineColor(kGreen);   
  heffTH1->SetFillStyle(3003);
  heffTH1->SetFillColor(kGreen);
  heffTH1->DrawCopy("hist same");
  TLegend legend3(-.65,0.05,.65,0.20,"","");
  legend3.SetFillColor(0);

  legend3.AddEntry(heffTH1,"PAC Efficiency 2010");
  legend3.AddEntry(heffTH,"PAC Efficiency 2011");

  legend3.Draw();*/
  TText *t = new TText(0.625,.95,"CMS Preliminary 2010 @ 7TeV");
  t->SetNDC(true);t->SetTextSize(0.042); t->DrawClone();

  TLatex *tl = new TLatex(0.445,.32,"pure barrel   |#eta|<0.8");
  tl->SetNDC(true);tl->SetTextSize(0.042);tl->SetTextColor(2); tl->Draw();
  
  TLatex *tl = new TLatex(0.83,.15,"pure endcap #eta>1.25");
  tl->SetNDC(true);tl->SetTextSize(0.042);tl->SetTextColor(2);tl->SetTextAngle(90); tl->Draw();
 TLatex *tl = new TLatex(0.225,.15,"pure endcap #eta<-1.25");
  tl->SetNDC(true);tl->SetTextSize(0.042);tl->SetTextColor(2);tl->SetTextAngle(90); tl->Draw();

  c2TH->Print(0,".png");
}





