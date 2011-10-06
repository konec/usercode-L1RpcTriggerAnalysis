#include <vector>
#include "TCanvas.h"
#include "FigUtils.C"

void makeMainPlots(float ptCut=15.0, string name="efficiencyHistos.root", 
		   string outDir="", string outPrefix="mainPlots")
{
  gSystem->mkdir("./png");
  gSystem->mkdir("./eps");
  gSystem->mkdir("./C");
  gSystem->mkdir("./root");

  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
  //gROOT.LoadMacro("FigUtils.C");
  MyDefaultStyle->cd();

  outDir+="./";
  stringstream outName;
  stringstream cutsDescription;
  cutsDescription<<"p_{T} code > "<<ptCut<<" GeV/c";
  std::vector<TCanvas*> canvasToWrite;

//--
  //TFile file("efficiencyHistos.root");
  TFile * file = new TFile(name.c_str());
  file->cd();
  file->ls();
//-------------------- 
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(1);
  gStyle->SetPalette(1);

  //----muon distribution
  c0Mu = new TCanvas("plot_GlbMu","Global muons distributions",1200,400);
  c0Mu->SetFillColor(0);
  c0Mu->Draw();
  c0Mu->Divide(3,1);
  c0Mu->cd(1);
  hMuonPt->DrawCopy();
  c0Mu->cd(2);
  hMuonEta->SetMinimum(0);
  hMuonEta->DrawCopy();
  c0Mu->cd(3);
  hMuonPhi->SetMinimum(0);
  hMuonPhi->DrawCopy();

  //c0Mu->Print(0,"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c0Mu->GetName()<<".png";
  c0Mu->Print(outName.str().c_str(),"png");
  c0Mu->Print(outName.str().c_str(),"png"); // this crazy line is need to properly create PNG when c->Divide is used
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c0Mu->GetName()<<".eps";
  c0Mu->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c0Mu->GetName()<<".C";
  c0Mu->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c0Mu);

  //----track distribution
  c0Tk = new TCanvas("plot_TrkMu","Tracker muons distributions",1200,400);
  c0Tk->Draw();
  c0Tk->SetFillColor(0);
  c0Tk->Divide(3,1);
  c0Tk->cd(1);
  hTrackPt->DrawCopy();
  c0Tk->cd(2);
  hTrackEta->DrawCopy();
  c0Tk->cd(3);
  hTrackPhi->DrawCopy();

  //c0Tk->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c0Tk->GetName()<<".png";
  c0Tk->Print(outName.str().c_str(),"png");
  c0Tk->Print(outName.str().c_str(),"png"); // this crazy line is need to properly create PNG when c->Divide is used
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c0Tk->GetName()<<".eps";
  c0Tk->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c0Tk->GetName()<<".C";
  c0Tk->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c0Tk);

//
// DET, HIT Efficiency vs ETA
//

  //----dets barrel
  c1EDB = new TCanvas("plot_DetsBarrel","Barrel DETs population",500,500); 
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

  TLegend * legend = new TLegend(0.2,0.2,1.9,0.5,"","");
//TLegend legend(1.3,0.7,1.9,1.0,"","");
  legend->AddEntry(hDB1,"layer 1","l");
  legend->AddEntry(hDB2,"layer 2","l");
  legend->AddEntry(hDB3,"layer 3","l");
  legend->AddEntry(hDB4,"layer 4","l");
  legend->AddEntry(hDB5,"layer 5","l");
  legend->AddEntry(hDB6,"layer 6","l");
  legend->Draw();

  //c1EDB->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c1EDB->GetName()<<".png";
  c1EDB->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c1EDB->GetName()<<".eps";
  c1EDB->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c1EDB->GetName()<<".C";
  c1EDB->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c1EDB);

  //---hits barrel
  c1EHB = new TCanvas("plot_HitsBarrel","Barrel HITs population",500,500); 
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
  TLegend * legend2 = new TLegend(0.2,0.2,1.9,0.5,"","");
//TLegend * legend2 = new TLegend(1.3,0.7,1.9,1.0,"","");
  legend2->AddEntry(hHB1,"layer 1","l");
  legend2->AddEntry(hHB2,"layer 2","l");
  legend2->AddEntry(hHB3,"layer 3","l");
  legend2->AddEntry(hHB4,"layer 4","l");
  legend2->AddEntry(hHB5,"layer 5","l");
  legend2->AddEntry(hHB6,"layer 6","l");
  legend2->Draw();
  //c1EHB->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c1EHB->GetName()<<".png";
  c1EHB->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c1EHB->GetName()<<".eps";
  c1EHB->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c1EHB->GetName()<<".C";
  c1EHB->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c1EHB);
  
  //
  // HIT AND LAYER EFFICIENCY AS ETA
  //
  //---dets endcap
  c1EDE = new TCanvas("plot_DetsEndcap","Endcap DETs population",500,500); 
  c1EDE->SetFillColor(0);
  TH1D * hDE1 = DivideErr(hEfficDetE_N1,hEfficHitDet_D,"hDE1","B"); 
  hDE1->SetMaximum(1.05);
  hDE1->SetTitle("Fraction of muons crossing Endcap layer");
  TH1D * hDE2 = DivideErr(hEfficDetE_N2,hEfficHitDet_D,"hDE2","B"); 
  TH1D * hDE3 = DivideErr(hEfficDetE_N3,hEfficHitDet_D,"hDE3","B"); 
  hDE1->SetLineColor(1); hDE1->DrawCopy("hist");
  hDE2->SetLineColor(2); hDE2->DrawCopy("hist same");
  hDE3->SetLineColor(3); hDE3->DrawCopy("hist same");
//TLegend * legendDE = new TLegend(0.2,0.2,1.9,0.5,"","");
  TLegend * legendDE = new TLegend(-0.5,0.8,.5,1,"","");
  legendDE->AddEntry(hDE1,"layer 1","l");
  legendDE->AddEntry(hDE2,"layer 2","l");
  legendDE->AddEntry(hDE3,"layer 3","l");
  legendDE->Draw();
  //c1EDE->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c1EDE->GetName()<<".png";
  c1EDE->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c1EDE->GetName()<<".eps";
  c1EDE->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c1EDE->GetName()<<".C";
  c1EDE->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c1EDE);

  //---hits endcap
  c1EHE = new TCanvas("plot_HitsEndcap","Endcap hits population",500,500); 
  c1EHE->SetFillColor(0);
  TH1D * hHE1 = DivideErr(hEfficHitE_N1,hEfficHitDet_D,"hDE1","B"); 
  hHE1->SetMaximum(1.05);
  hHE1->SetTitle("Fraction of muons hitting Endcap layer");
  TH1D * hHE2 = DivideErr(hEfficHitE_N2,hEfficHitDet_D,"hDE2","B"); 
  TH1D * hHE3 = DivideErr(hEfficHitE_N3,hEfficHitDet_D,"hDE3","B"); 
  hHE1->SetLineColor(1); hHE1->DrawCopy("hist");
  hHE2->SetLineColor(2); hHE2->DrawCopy("hist same");
  hHE3->SetLineColor(3); hHE3->DrawCopy("hist same");
  TLegend * legendHE = new TLegend(0.2,0.2,1.9,0.5,"","");
//TLegend * legendHE = new TLegend(-0.5,0.8,.5,1,"","");
  legendHE->AddEntry(hHE1,"layer 1","l");
  legendHE->AddEntry(hHE2,"layer 2","l");
  legendHE->AddEntry(hHE3,"layer 3","l");
  legendHE->Draw();
  //c1EHE->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c1EHE->GetName()<<".png";
  c1EHE->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c1EHE->GetName()<<".eps";
  c1EHE->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c1EHE->GetName()<<".C";
  c1EHE->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c1EHE);

//
// HITS AND LAYER SUMMARY 
//
  c9 =  new TCanvas("plot_ChambEffLayer","Chamber efficiency per layer",-1);
  c9->SetFillColor(0);
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
  hEfficChambEnd_D->Print("all");
  heffChambEnd->SetMinimum(0.4);
  heffChambEnd->SetMaximum(1.002);
  heffChambEnd->SetXTitle("layer");
  heffChambEnd->SetYTitle("efficiency");
  heffChambEnd->DrawCopy("E");
  heffChambEnd->Print("all");
  //c9->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c9->GetName()<<".png";
  c9->Print(outName.str().c_str(),"png");
  c9->Print(outName.str().c_str(),"png"); // this crazy line is need to properly create PNG when c->Divide is used
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c9->GetName()<<".eps";
  c9->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c9->GetName()<<".C";
  c9->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c9);

//
// CROSSED LAYERS AND DETS
//
  //----number of crossed dets 
  Double_t sum4_6,sum3_6;
  c1D = new TCanvas("plot_CrossedDets","Number of crossed DETs",900,500);
  c1D->SetFillColor(0);
  c1D->Divide(2,1);
  c1D->cd(1);
  hDetsB_100->Scale(1/hDetsB_100->Integral());
  sum4_6 =   hDetsB_100->GetBinContent(5)
           + hDetsB_100->GetBinContent(6)
           + hDetsB_100->GetBinContent(7);
  sum3_6 = sum4_6 + hDetsB_100->GetBinContent(4); 
  std::cout <<" Barrel Dets (4-6): "<<sum4_6<<" (3-6): "<<sum3_6 <<std::endl;
  hDetsB_100->SetMinimum(0.);
  hDetsB_100->DrawCopy(); 
  c1D->cd(2);
  hDetsE_100->Scale(1/hDetsE_100->Integral());
  std::cout <<" Endcap Dets (3-3): "<< hDetsE_100->GetBinContent(4) <<std::endl;
  hDetsE_100->SetMinimum(0.);
  hDetsE_100->DrawCopy();
  //c1D->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c1D->GetName()<<".png";
  c1D->Print(outName.str().c_str(),"png");
  c1D->Print(outName.str().c_str(),"png"); // this crazy line is need to properly create PNG when c->Divide is used
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c1D->GetName()<<".eps";
  c1D->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c1D->GetName()<<".C";
  c1D->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c1D);

  //----number of deposited hits  
  c1H = new TCanvas("plot_DepositedHits","Number of deposited HITs",900,500);
  c1H->SetFillColor(0);  
  c1H->Divide(2,1);
  c1H->cd(1);
  hHitsB->Scale(1/hHitsB->Integral());
  sum4_6 =   hHitsB->GetBinContent(5)
           + hHitsB->GetBinContent(6)
           + hHitsB->GetBinContent(7);
  sum3_6 = sum4_6 + hHitsB->GetBinContent(4); 
  std::cout <<" Barrel Hits (4-6): "<<sum4_6<<" (3-6): "<<sum3_6 <<std::endl;
  hHitsB->SetMinimum(0.);
  hHitsB->DrawCopy(); 
  c1H->cd(2);
  hHitsE->Scale(1/hHitsE->Integral());
  std::cout <<" Endcap Hits (3-3): "<< hHitsE->GetBinContent(4) <<std::endl;
  hHitsE->SetMinimum(0.);
  hHitsE->DrawCopy();
  //c1H->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c1H->GetName()<<".png";
  c1H->Print(outName.str().c_str(),"png");
  c1H->Print(outName.str().c_str(),"png"); // this crazy line is need to properly create PNG when c->Divide is used
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c1H->GetName()<<".eps";
  c1H->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c1H->GetName()<<".C";
  c1H->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c1H);

/////////////////////////////////

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
  std::cout <<"4z6: L1RPC:  Barrel: "<<tb/mb <<" MB="<<mb <<" Endcap: "<<te/me<<" ME="<<me<< std::endl;
  std::cout <<"3z6: L1RPC:  Barrel: "<<tb_3z6/mb <<" MB="<<mb <<" Endcap: "<<te_3z6/me<<" ME="<<me<< std::endl;
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
  std::cout <<"L1RPC wrt global muon:  Barrel: "<<tb/mb<<" MB="<<mb <<" Endcap: "<<te/me<<" ME="<<me
            <<" average: "<<ta/ma << std::endl;
  heffM->SetMinimum(0.);
  heffM->SetMaximum(1.002);
  heffM->SetXTitle("eta");
  heffM->SetYTitle("efficiency");
  heffM->SetLineColor(kBlue);

/////////////////////////////////

  c2 =  new TCanvas("plot_DetHitTrgEffVsEta","Efficiency vs eta",-2);
  c2->SetFillColor(0);

  heffM->DrawCopy("hist");
  heffH->DrawCopy("hist same");
  heffD->DrawCopy("hist same");

  TLegend * legend3 = new TLegend(-0.5,0.1,1.5,0.2,"","");
  legend3->AddEntry(heffD,"DETs eff for '4/6' trigger");
  legend3->AddEntry(heffH,"HITs eff for '4/6' trigger");
  legend3->AddEntry(heffM,"L1RPC eff wrt glb.muon");
//legend3->AddEntry(heffT,"required Hits & L1RPC");
  legend3->Draw();

  //c2->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c2->GetName()<<".png";
  c2->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c2->GetName()<<".eps";
  c2->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c2->GetName()<<".C";
  c2->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c2);

/////////////////////////////////

  c2_3z6 = new TCanvas("plot_DetHitTrgEffVsEta_3z6","Efficiency vs eta",-2);
  c2_3z6->SetFillColor(0);

  heffM->DrawCopy("hist");
  heffH_3z6->DrawCopy("hist same");
  heffD_3z6->DrawCopy("hist same");
  
  TLegend * legend3_3z6 = new TLegend(-0.5,0.1,1.5,0.2,"","");
  legend3_3z6->AddEntry(heffD_3z6,"DETs eff for '3/6' trigger");
  legend3_3z6->AddEntry(heffH_3z6,"HITs eff for '3/6' trigger");
  legend3_3z6->AddEntry(heffM,"L1RPC eff wrt glb.muon");
  legend3_3z6->Draw();

  //c2_3z6->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c2_3z6->GetName()<<".png";
  c2_3z6->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c2_3z6->GetName()<<".eps";
  c2_3z6->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c2_3z6->GetName()<<".C";
  c2_3z6->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c2_3z6);

/////////////////////////////////

  //--- trigger efficiency for wrt Mu 
  c3 =  new TCanvas("plot_TrgEffGlbMuVsEta","L1RPC eff wrt glb.muon vs eta",-2);
  c3->SetFillColor(0);
  TH1D * heffM = DivideErr(hEfficMu_N,hEfficMu_D,"hEfficMu","B");
  Double_t me = hEfficMu_D->Integral(5,7)+hEfficMu_D->Integral(27,29);
  Double_t mb = hEfficMu_D->Integral(12,28);
  Double_t te = hEfficMu_N->Integral(5,7)+ hEfficMu_N->Integral(27,29);
  Double_t tb = hEfficMu_N->Integral(12,28);
  Double_t ta = hEfficMu_N->Integral(1,33);
  Double_t ma = hEfficMu_D->Integral(1,33);
  std::cout <<"L1RPC wrt global muon:  Barrel: "<<tb/mb<<" MB="<<mb <<" Endcap: "<<te/me<<" ME="<<me
            <<" average: "<<ta/ma << std::endl;
  heffM->SetMinimum(0.);
  heffM->SetMaximum(1.002);
  heffM->SetXTitle("eta");
  heffM->SetYTitle("efficiency");
  heffM->SetMarkerStyle(25);
  heffM->SetMarkerColor(4);
  heffM->DrawCopy("E");

  //c3->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c3->GetName()<<".png";
  c3->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c3->GetName()<<".eps";
  c3->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c3->GetName()<<".C";
  c3->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c3);

/////////////////////////////////

  //---- trigger efficiency wrt Trk 
  c4 =  new TCanvas("plot_TrgEffTrkMuVsEta","L1RPC eff wrt trk.muon vs eta",-2);
  c4->SetFillColor(0);
  TH1D * heffT = DivideErr(hEfficTk_N,hEfficTk_D,"hEfficTk","B");
  heffT->SetMinimum(0.);
  heffT->SetMaximum(1.002);
  heffT->SetXTitle("eta");
  heffT->SetYTitle("efficiency");
  heffT->SetMarkerStyle(25);
  heffT->SetMarkerColor(4);
  heffT->DrawCopy("E");

  //c4->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c4->GetName()<<".png";
  c4->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c4->GetName()<<".eps";
  c4->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c4->GetName()<<".C";
  c4->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c4);

/////////////////////////////////

  //--- trigger efficiency wrt Mu vs PT (barrel+endcap) 
  c5 = new TCanvas("plot_TrgEffGlbMuVsPt","L1RPC eff wrt glb.muon vs p_{T}",-2);
  c5->SetFillColor(0);
  TH1D * heffMuPt = DivideErr(hEfficMuPt_N,hEfficMuPt_D,"hEfficMuPt","B");
  TH1D * heffMuPt7 = DivideErr(hEfficMuPt7_N,hEfficMuPt_D,"hEfficMuPt7","B");
  heffMuPt->SetMinimum(0.);
  heffMuPt->SetMaximum(1.002);
  heffMuPt->SetXTitle("glb.muon p_{T} [GeV/c]");
  heffMuPt->SetYTitle("efficiency");
  heffMuPt->SetMarkerStyle(25);
  heffMuPt->SetMarkerColor(4); // blue
  heffMuPt->Draw("E");
  heffMuPt7->SetMarkerStyle(23);
  heffMuPt7->SetMarkerColor(2); // red
  heffMuPt7->DrawCopy("E same");

  TLegend * legend5 = new TLegend(80,0.2,120,0.4,"","");
  legend5->AddEntry(heffMuPt,"any p_{T} code");
  legend5->AddEntry(heffMuPt7,cutsDescription.str().c_str());
  legend5->Draw();

  //c5->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c5->GetName()<<".png";
  c5->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c5->GetName()<<".eps";
  c5->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c5->GetName()<<".C";
  c5->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c5);

/////////////////////////////////

  //--- trigger efficiency wrt Mu vs PT (barrel) 
  c5_barrel = new TCanvas("plot_TrgEffGlbMuVsPt_Barrel","L1RPC eff wrt glb.muon vs p_{T}, Barrel",-2);
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

  TLegend * legend5_barrel = new TLegend(80,0.2,120,0.4,"","");
  legend5_barrel->AddEntry(heffMuPt_barrel,"any p_{T} code");
  legend5_barrel->AddEntry(heffMuPt7_barrel,cutsDescription.str().c_str());
  legend5_barrel->Draw();

  //c5_barrel->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c5_barrel->GetName()<<".png";
  c5_barrel->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c5_barrel->GetName()<<".eps";
  c5_barrel->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c5_barrel->GetName()<<".C";
  c5_barrel->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c5_barrel);
  
/////////////////////////////////

  //--- trigger efficiency wrt Mu vs PT (overlap)  
  c5_overlap = new TCanvas("plot_TrgEffGlbMuVsPt_Overlap","L1RPC eff wrt glb.muon vs p_{T}, Overlap",-2);
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

  TLegend * legend5_overlap = new TLegend(80,0.2,120,0.4,"","");
  legend5_overlap->AddEntry(heffMuPt_overlap,"any p_{T} code");
  legend5_overlap->AddEntry(heffMuPt7_overlap,cutsDescription.str().c_str());
  legend5_overlap->Draw();

  //c5_overlap->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c5_overlap->GetName()<<".png";
  c5_overlap->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c5_overlap->GetName()<<".eps";
  c5_overlap->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c5_overlap->GetName()<<".C";
  c5_overlap->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c5_overlap);
  
/////////////////////////////////

  //--- trigger efficiency wrt Mu vs PT (endcap)  
  c5_endcap = new TCanvas("plot_TrgEffGlbMuVsPt_Endcap","L1RPC eff wrt glb.muon vs p_{T}, Endcap",-2);
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

  TLegend * legend5_endcap = new TLegend(80,0.2,120,0.4,"","");
  legend5_endcap->AddEntry(heffMuPt_endcap,"any p_{T} code");
  legend5_endcap->AddEntry(heffMuPt7_endcap,cutsDescription.str().c_str());
  legend5_endcap->Draw();

  //c5_endcap->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c5_endcap->GetName()<<".png";
  c5_endcap->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c5_endcap->GetName()<<".eps";
  c5_endcap->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c5_endcap->GetName()<<".C";
  c5_endcap->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c5_endcap);

/////////////////////////////////

  //--- trigger efficiency wrt Mu vs PT (endcap negative)
  c5_endcapN = new TCanvas("plot_TrgEffGlbMuVsPt_EndcapN","L1RPC eff wrt glb.muon vs p_{T}, Endcap '-'",-2);
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

  TLegend * legend5_endcapN = new TLegend(80,0.2,120,0.4,"","");
  legend5_endcapN->AddEntry(heffMuPt_endcapN,"any p_{T} code");
  legend5_endcapN->AddEntry(heffMuPt7_endcapN,cutsDescription.str().c_str());
  legend5_endcapN->Draw();

  //c5_endcapN->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c5_endcapN->GetName()<<".png";
  c5_endcapN->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c5_endcapN->GetName()<<".eps";
  c5_endcapN->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c5_endcapN->GetName()<<".C";
  c5_endcapN->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c5_endcapN);
  
/////////////////////////////////

  //--- trigger efficiency wrt Mu vs PT (endcap positive)
  c5_endcapP = new TCanvas("plot_TrgEffGlbMuVsPt_EndcapP","L1RPC eff wrt glb.muon vs p_{T}, Endcap '+'",-2);
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

  TLegend * legend5_endcapP = new TLegend(80,0.2,120,0.4,"","");
  legend5_endcapP->AddEntry(heffMuPt_endcapP,"any p_{T} code");
  legend5_endcapP->AddEntry(heffMuPt7_endcapP,cutsDescription.str().c_str());
  legend5_endcapP->Draw();

  //c5_endcapP->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c5_endcapP->GetName()<<".png";
  c5_endcapP->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c5_endcapP->GetName()<<".eps";
  c5_endcapP->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c5_endcapP->GetName()<<".C";
  c5_endcapP->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c5_endcapP);

/////////////////////////////////

  //--- trigger efficiency wrt Trk vs PT (barrel+endcap)
  c6 = new TCanvas("plot_TrgEffTrkMuVsPt","L1RPC efficiency wrt tracker muon vs p_{T}",-2);
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

  TLegend * legend6 = new TLegend(80,0.2,120,0.4,"","");
  legend6->AddEntry(heffTkPt,"any p_{T} code");
  legend6->AddEntry(heffTkPt7,cutsDescription.str().c_str());
  legend6->Draw();

  //c6->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c6->GetName()<<".png";
  c6->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c6->GetName()<<".eps";
  c6->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c6->GetName()<<".C";
  c6->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c6);

/////////////////////////////////

  //--- L1RPC and L1GMT 2D occupancy plots (eta vs phi)
  c7 = new TCanvas("plot_TrgOccupVsEtaVsPhi","L1RPC 2D occupancy",1200,500);
  c7->SetFillColor(0);
  c7->Divide(2,1);
  c7->cd(1);
  hDistL1Rpc->DrawCopy("box");
  c7->cd(2);
  hDistL1Other->DrawCopy("box");

  //c7->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c7->GetName()<<".png";
  c7->Print(outName.str().c_str(),"png");
  c7->Print(outName.str().c_str(),"png"); // this crazy line is need to properly create PNG when c->Divide is used
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c7->GetName()<<".eps";
  c7->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c7->GetName()<<".C";
  c7->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c7);

/////////////////////////////////

  //--- repeat same plots with COLZ option
  //--- L1RPC and L1GMT 2D occupancy plots (eta vs phi)
  c7_col = new TCanvas("plot_TrgOccupVsEtaVsPhi_col","L1RPC 2D occupancy",1200,500);
  c7_col->SetFillColor(0);
  c7_col->Divide(2,1);
  c7_col->cd(1);
  hDistL1Rpc->DrawCopy("colz");
  c7_col->cd(2);
  hDistL1Other->DrawCopy("colz");

  //c7_col->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c7_col->GetName()<<".png";
  c7_col->Print(outName.str().c_str(),"png");
  c7_col->Print(outName.str().c_str(),"png"); // this crazy line is need to properly create PNG when c->Divide is used
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c7_col->GetName()<<".eps";
  c7_col->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c7_col->GetName()<<".C";
  c7_col->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c7_col);

/////////////////////////////////

  //--- L1RPC timing plot wrt L1
  c8= new TCanvas("plot_TrgBxWrtL1","L1RPC timing",-2);
  c8->SetFillColor(0);
  hL1RpcBX->SetXTitle("L1RPC wrt L1Accept [BX]");
  Int_t integral = hL1RpcBX->Integral(); 
  hL1RpcBX->Scale(1./integral);
  gPad->SetLogy();
  hL1RpcBX->SetNdivisions(605,"X");
  hL1RpcBX->DrawCopy();

  //c8->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c8->GetName()<<".png";
  c8->Print(outName.str().c_str(),"png");
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c8->GetName()<<".eps";
  c8->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c8->GetName()<<".C";
  c8->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c8);

/////////////////////////////////

  //----chamber efficiency per run
  c10 =  new TCanvas("plot_ChambEffPerRun","Chamber eff per run",-1);  
  c10->SetFillColor(0);
  c10->Divide(2,1);
  c10->cd(1);
  hEffRun->Draw();
  c10->cd(2);

  hGraphRun->SetMaximum(1.);
  hGraphRun->SetMinimum(0.8); 
  hGraphRun->SetMarkerStyle(25);
  hGraphRun->SetMarkerColor(2);
  hGraphRun->Draw("Paint E");

  //c10->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c10->GetName()<<".png";
  c10->Print(outName.str().c_str(),"png");
  c10->Print(outName.str().c_str(),"png"); // this crazy line is need to properly create PNG when c->Divide is used
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c10->GetName()<<".eps";
  c10->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c10->GetName()<<".C";
  c10->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c10);

/////////////////////////////////

  //----chamber efficiency per run (3 regions)
  c11 =  new TCanvas("plot_ChambEffPerRun_BarrelEndcap","Chamber eff per run",-1);
  c11->SetFillColor(0);
  c11->Divide(3,1);

  c11->cd(1);
  hGraphRun->SetMaximum(1.);
  hGraphRun->SetMinimum(0.0); 
  hGraphRun->SetMarkerStyle(25);
  hGraphRun->SetMarkerColor(2);
  hGraphRun->Draw("Paint E");

  c11->cd(2);
  hGraphRunBarrel->SetMaximum(1.);
  hGraphRunBarrel->SetMinimum(0.8); 
  hGraphRunBarrel->SetMarkerStyle(25);
  hGraphRunBarrel->SetMarkerColor(2);
  hGraphRunBarrel->Draw("Paint E");

  c11->cd(3);
  hGraphRunEndcap->SetMaximum(1.);
  hGraphRunEndcap->SetMinimum(0.8); 
  hGraphRunEndcap->SetMarkerStyle(25);
  hGraphRunEndcap->SetMarkerColor(2);
  hGraphRunEndcap->Draw("Paint E");

  //c11->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c11->GetName()<<".png";
  c11->Print(outName.str().c_str(),"png");
  c11->Print(outName.str().c_str(),"png"); // this crazy line is need to properly create PNG when c->Divide is used
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c11->GetName()<<".eps";
  c11->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c11->GetName()<<".C";
  c11->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c11);

/////////////////////////////////

  //return;

  //----chamber efficiency per Lumi Section
  c10_ls =  new TCanvas("plot_ChambEffPerLS","Chamber eff per lumi section",-1);
  c10_ls->SetFillColor(0);
  c10_ls->Divide(2,1);
  c10_ls->cd(1);
  hEffLumi->Draw();
  c10_ls->cd(2);
 
  hGraphLumi->SetMaximum(1.);
  hGraphLumi->SetMinimum(0.);
 
  hGraphLumi->SetMarkerStyle(25);
  hGraphLumi->SetMarkerColor(2);
  hGraphLumi->Draw("Paint E");

  //c10_ls->Print(0,".png");
  outName.str(""); // reset stream 
  outName<<outDir<<"png/"<<c10_ls->GetName()<<".png";
  c10_ls->Print(outName.str().c_str(),"png");
  c10_ls->Print(outName.str().c_str(),"png"); // this crazy line is need to properly create PNG when c->Divide is used
  outName.str(""); // reset stream 
  outName<<outDir<<"eps/"<<c10_ls->GetName()<<".eps";
  c10_ls->Print(outName.str().c_str(),"eps");
  outName.str(""); // reset stream 
  outName<<outDir<<"C/"<<c10_ls->GetName()<<".C";
  c10_ls->Print(outName.str().c_str(),"cxx");
  canvasToWrite.push_back(c10_ls);

  outName.str("");
  outName<<outDir<<"/root/"<<outPrefix<<".root";
  std::cout<<outName.str().c_str()<<std::endl;
  TFile * fout = new TFile(outName.str().c_str(),"RECREATE");
  fout->cd();
  for(int i=0;i<canvasToWrite.size();i++) (canvasToWrite[i])->Write();
  fout->Close();
  file->Close();
  return;
}
