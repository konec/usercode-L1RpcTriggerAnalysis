TCanvas * getDefaultCanvas(float x=10,float y=30,float w=600,float h=600){
        TCanvas *c = new TCanvas("c1","RPC Trigger Efficiency",x,y,w,h);
        gStyle->SetCanvasBorderMode(0);
        gStyle->SetCanvasColor(kWhite);
        gStyle->SetCanvasDefH(600); //Height of canvas
        gStyle->SetCanvasDefW(600); //Width of canvas
        gStyle->SetCanvasDefX(0);   //POsition on screen
        gStyle->SetCanvasDefY(0);
        gStyle->SetPadBorderMode(0);
        gStyle->SetPadColor(kWhite);
        gStyle->SetPadGridX(true);
        gStyle->SetPadGridY(true);
        gStyle->SetGridColor(0);
        gStyle->SetGridStyle(3);
        gStyle->SetGridWidth(1);
        gStyle->SetFrameBorderMode(0);
        gStyle->SetFrameBorderSize(1);
        gStyle->SetFrameFillColor(0);
        gStyle->SetFrameFillStyle(0);
        gStyle->SetFrameLineColor(1);
        gStyle->SetFrameLineStyle(1);
        gStyle->SetFrameLineWidth(1);
        gStyle->SetHistLineColor(1);
        gStyle->SetHistLineStyle(0);
        gStyle->SetHistLineWidth(1);
        gStyle->SetEndErrorSize(2);
        gStyle->SetErrorX(0.);
        gStyle->SetMarkerStyle(20);
        gStyle->SetOptFit(1);
        gStyle->SetFitFormat("5.4g");
        gStyle->SetFuncColor(2);
        gStyle->SetFuncStyle(1);
        gStyle->SetFuncWidth(1);
        gStyle->SetOptDate(0);
        gStyle->SetOptFile(0);
        gStyle->SetOptStat(0);
        gStyle->SetStatColor(kWhite);
        gStyle->SetStatFont(42);
        gStyle->SetStatFontSize(0.025);
        gStyle->SetStatTextColor(1);
        gStyle->SetStatFormat("6.4g");
        gStyle->SetStatBorderSize(1);
        gStyle->SetStatH(0.1);
        gStyle->SetStatW(0.15);
        gStyle->SetPadTopMargin(0.05);
        gStyle->SetPadBottomMargin(0.13);
        gStyle->SetPadLeftMargin(0.15);
        gStyle->SetPadRightMargin(0.04);
        gStyle->SetOptTitle(0);
        gStyle->SetTitleFont(42);
        gStyle->SetTitleColor(1);
        gStyle->SetTitleTextColor(1);
        gStyle->SetTitleFillColor(10);
        gStyle->SetTitleFontSize(0.05);
        gStyle->SetTitleColor(1, "XYZ");
        gStyle->SetTitleFont(42, "XYZ");
        gStyle->SetTitleSize(0.06, "XYZ");
        gStyle->SetTitleXOffset(0.9);
        gStyle->SetTitleYOffset(1.25);
        gStyle->SetLabelColor(1, "XYZ");
        gStyle->SetLabelFont(42, "XYZ");
        gStyle->SetLabelOffset(0.007, "XYZ");
        gStyle->SetLabelSize(0.05, "XYZ");
        gStyle->SetAxisColor(1, "XYZ");
        gStyle->SetStripDecimals(kTRUE);
        gStyle->SetTickLength(0.03, "XYZ");
        gStyle->SetNdivisions(510, "XYZ");
        gStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
        gStyle->SetPadTickY(1);
        gStyle->SetOptLogx(0);
        gStyle->SetOptLogy(0);
        gStyle->SetOptLogz(0);
        gStyle->SetPaperSize(20.,20.);        
        return c;
}
TLegend * getDefaultLegend(bool IsEta){
        if (IsEta) TLegend *leg = new TLegend(0.3691275,0.1398601,0.7315436,0.3234266,NULL,"brNDC");
        else TLegend *leg = new TLegend(0.1510067,0.8146853,0.4228188,0.9475524,NULL,"brNDC");
        leg->SetHeader("RPC Trigger efficiency,|#eta|<8");
        leg->SetFillColor(0);       
        leg->SetLineColor(0);
        leg->SetTextSize(0.03);
        leg->SetMargin(0.3);
        return leg;
}
TH1 * getDefaultFrame(){        
        TH1 *hFrame = new TH1F("hFrame","",100,0,70);
        hFrame->SetMinimum(0);
        hFrame->SetMaximum(1.1);
        hFrame->SetDirectory(0);
        hFrame->SetStats(0);
        hFrame->SetLineColor(1);      
        return hFrame;
}
void setupHisto(TH1F *h, int color, int markerStyle){
        h->SetMarkerSize(1);
        h->SetMarkerStyle(markerStyle);
        h->SetLineColor(color);
        h->SetLineWidth(1);
        h->SetYTitle("L1 RPC efficiency");
        h->SetStats(kFALSE);
        h->SetMaximum(1.049);
        h->SetMinimum(0.0);
        h->SetMarkerColor(color);
}
void emptyBins(TH1F *h, TString sf){
        if (sf.Contains("DT")){ 
                Int_t nbins = h->GetNbinsX();
                for (int bin=0;bin<=nbins+1;bin++){
                        if (bin<=10||bin>=33) h->SetBinContent(bin,0);
                } 
        }
        if (sf.Contains("CSC")){ 
                Int_t nbins = h->GetNbinsX();
                for (int bin=0;bin<=nbins+1;bin++){
                        if (bin>=12&&bin<=31) h->SetBinContent(bin,0);
                } 
        }
}
// One histo
TCanvas * drawHistos(TH1F * h1,bool IsEta){       
        TCanvas *c1 = getDefaultCanvas();                     
        c1->cd();  
        if(IsEta){      
                Double_t xAxis[43] = {-2.1,-2.0,-1.9,-1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,-1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.1}; 
                TH1F *h = new TH1F("h","",42,xAxis);
                h->SetXTitle("#eta"); 
        }else {
                Double_t xAxis[15] = {0, 3, 6, 10,20, 25, 30, 35, 40, 45, 50, 60, 70, 80,100,}; 
                TH1F *h = new TH1F("h","",14,xAxis);
                h->SetXTitle("probe p_{T} [GeV/c]");     
        }
        h->SetYTitle("L1 RPC efficiency");
        setupHisto(h,1,8);
        h->Draw();
        if (IsEta){
                h1->Draw("hist same");
        } else {
                h1->Draw("E1SAME");     
        }
        return c1;
}
// Two histos
TCanvas * drawHistos(TH1F * h1,TH1F * h2,bool IsEta){       
        TCanvas *c1 = getDefaultCanvas();                     
        c1->cd();  
        if(IsEta){      
                Double_t xAxis[43] = {-2.1,-2.0,-1.9,-1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,-1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.1}; 
                TH1F *h = new TH1F("h","",42,xAxis);
                h->SetXTitle("#eta"); 
        }else {
           //     Double_t xAxis[15] = {0, 3, 6, 10,20, 25, 30, 35, 40, 45, 50, 60, 70, 80,100,}; 
            //    TH1F *h = new TH1F("h","",14,xAxis);
               Double_t xAxis[5] = {0,3, 6, 10,20}; 
               TH1F *h = new TH1F("h","",4,xAxis);
 
                h->SetXTitle("probe p_{T} [GeV/c]");     
        }
        h->SetYTitle("L1 RPC efficiency");
        setupHisto(h,1,8);
        h->Draw();
        if (IsEta){
                h1->Draw("hist same");
                h2->Draw("hist same");
        } else {
                h1->Draw("E1SAME");
                h2->Draw("E1SAME");      
        }
        return c1;
}
// Three histos
TCanvas * drawHistos(TH1F * h1,TH1F * h2, TH1F * h3, bool IsEta){       
        TCanvas *c1 = getDefaultCanvas();                     
        c1->cd();  
        if(IsEta){      
                Double_t xAxis[43] = {-2.1,-2.0,-1.9,-1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,-1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.1}; 
                TH1F *h = new TH1F("h","",42,xAxis);
                h->SetXTitle("#eta"); 
        }else {
                Double_t xAxis[15] = {0, 3, 6, 10,20, 25, 30, 35, 40, 45, 50, 60, 70, 80,100,}; 
                TH1F *h = new TH1F("h","",14,xAxis);
                h->SetXTitle("probe p_{T} [GeV/c]");     
        }
        h->SetYTitle("L1 RPC efficiency");
        setupHisto(h,1,8);
        h->Draw();
        if (IsEta){
                h1->Draw("hist same");
                h2->Draw("hist same");
                h3->Draw("hist same");
        } else {
                h1->Draw("E1SAME");
                h2->Draw("E1SAME");
                h3->Draw("E1SAME");        
        }
        return c1;
}
// Four histos
TCanvas * drawHistos(TH1F * h1,TH1F * h2, TH1F * h3, TH1F * h4, bool IsEta){       
        TCanvas *c1 = getDefaultCanvas();                     
        c1->cd();  
        if(IsEta){      
                Double_t xAxis[43] = {-2.1,-2.0,-1.9,-1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,-1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.1}; 
                TH1F *h = new TH1F("h","",42,xAxis);
                h->SetXTitle("#eta"); 
        }else {
                Double_t xAxis[15] = {0, 3, 6, 10,20, 25, 30, 35, 40, 45, 50, 60, 70, 80,100,}; 
                TH1F *h = new TH1F("h","",14,xAxis);
                h->SetXTitle("probe p_{T} [GeV/c]");     
        }
        h->SetYTitle("L1 RPC efficiency");
        setupHisto(h,1,8);
        h->Draw();
        if (IsEta){
                h1->Draw("hist same");
                h2->Draw("hist same");
                h3->Draw("hist same");
                h4->Draw("hist same");
        } else {
                h1->Draw("E1SAME");
                h2->Draw("E1SAME");
                h3->Draw("E1SAME");
                h4->Draw("E1SAME");        
        }
        return c1;
}
// Six histos
TCanvas * drawHistos(TH1F * h1,TH1F * h2, TH1F * h3, TH1F * h4, TH1F * h5, TH1F * h6, bool IsEta){       
        TCanvas *c1 = getDefaultCanvas();                     
        c1->cd();  
        if(IsEta){      
                Double_t xAxis[43] = {-2.1,-2.0,-1.9,-1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,-1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.1}; 
                TH1F *h = new TH1F("h","",42,xAxis);
                h->SetXTitle("#eta"); 
        }else {
                Double_t xAxis[15] = {0, 3, 6, 10,20, 25, 30, 35, 40, 45, 50, 60, 70, 80,100,}; 
                TH1F *h = new TH1F("h","",14,xAxis);
                h->SetXTitle("probe p_{T} [GeV/c]");     
        }
        h->SetYTitle("L1 RPC efficiency");
        setupHisto(h,1,8);
        h->Draw();
        if (IsEta){
                h1->Draw("hist same");
                h2->Draw("hist same");
                h3->Draw("hist same");
                h4->Draw("hist same");
                h5->Draw("hist same");
                h6->Draw("hist same");
        } else {
                h1->Draw("E1SAME");
                h2->Draw("E1SAME");
                h3->Draw("E1SAME");
                h4->Draw("E1SAME");
                h5->Draw("E1SAME");
                h6->Draw("E1SAME");      
        }
        return c1;
}
// Twelve histos
TCanvas * drawHistos(TH1F * h1,TH1F * h2, TH1F * h3, TH1F * h4, TH1F * h5, TH1F * h6, TH1F * h7,TH1F * h8, TH1F * h9, TH1F * h10, TH1F * h11, TH1F * h12,bool IsEta){       
        TCanvas *c1 = getDefaultCanvas();                     
        c1->cd();  
        if(IsEta){      
                //Double_t xAxis[43] = {-2.1,-2.0,-1.9,-1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,-1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1,0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.1}; 
                //TH1F *h = new TH1F("h","",42,xAxis);
                Double_t xAxis[56] = {-2.1,-1.9,-1.7,-1.61,-1.545,-1.48,-1.42,-1.36,-1.3,-1.24,-1.19,-1.14,-1.04,-0.985,-0.93,-0.88,-0.83,-0.775,-0.72,-0.65,-0.58,-0.51,-0.44,-0.355,-0.27,-0.17,-0.07,0.0,0.07,0.17,0.27,
                                            0.355,0.44,0.51,0.58,0.65,0.72,0.775,0.83,0.88,0.93,0.985,1.04,1.09,1.14,1.19,1.24,1.3,1.36,1.42,1.48, 1.545,1.61,1.7,1.9,2.1}; 
                TH1F *h = new TH1F("h","",55,xAxis);
                h->SetXTitle("#eta"); 
        }else {
                Double_t xAxis[15] = {0, 3, 6, 10,20, 25, 30, 35, 40, 45, 50, 60, 70, 80,100,}; 
                TH1F *h = new TH1F("h","",14,xAxis);
                h->SetXTitle("probe p_{T} [GeV/c]");     
        }
        h->SetYTitle("L1 RPC efficiency");
        setupHisto(h,1,8);
        h->Draw();
        if (IsEta){
                h1->Draw("hist same");
                h2->Draw("hist same");
                h3->Draw("hist same");
                h4->Draw("hist same");
                h5->Draw("hist same");
                h6->Draw("hist same");
                h7->Draw("hist same");
                h8->Draw("hist same");
                h9->Draw("hist same");
                h10->Draw("hist same");
                h11->Draw("hist same");
                h12->Draw("hist same");                
        } else {
                h1->Draw("E1SAME");
                h2->Draw("E1SAME");
                h3->Draw("E1SAME");
                h4->Draw("E1SAME");
                h5->Draw("E1SAME");
                h6->Draw("E1SAME");
                h7->Draw("E1SAME");
                h8->Draw("E1SAME");
                h9->Draw("E1SAME");
                h10->Draw("E1SAME");
                h11->Draw("E1SAME");
                h12->Draw("E1SAME");                       
        }
        return c1;
}
TString leg(TString sf){
        bool IsEta = sf.Contains("etaBins") ? true :false;
        TString det,pTcutraw,pTcut,reg;
        bool IsJPsi = sf.Contains("JPsi") ? true : false;
        if (IsEta) {if (sf.Contains("RPC")) det = "RPC Trigger Efficiency"; else if (sf.Contains("DT")) det = "DT Trigger Efficiency"; else if (sf.Contains("CSC")) det = "CSC Trigger Efficiency";} 
        else{if (sf.Contains("L1RPC")) det = "RPC"; else if (sf.Contains("DT")) det = "DT"; else if (sf.Contains("CSC")) det = "CSC";}        
        if (sf.Contains("Match_")) pTcut = "no p_{T}cut";
        else if (sf.Contains("etaBins")) pTcut = "";
        else if (sf.Contains("Value")) {
                pTcutraw = sf(sf.Index("Value",5)+5, 2);
                if (!pTcutraw.IsAlnum()) pTcutraw.Remove(1);
                pTcut =  "p_{T} #geq "+pTcutraw+" GeV/c"; 
        }
        if (!IsEta&&(sf.Contains("barrel")||sf.Contains("DT"))) reg = ", |#eta|<0.8, ";else if (sf.Contains("overlap")) reg =  ", 0.8<|#eta|<1.25, ";
        else if (!IsEta&&(sf.Contains("endcap")||sf.Contains("CSC"))) reg = ", 1.25<|#eta|<1.61, "; else reg = "";
        //TString leg(det+reg+pTcut);
        TString leg(pTcut);
        return leg;
}
// One histo
void makeEffPlots(string file1,                              
                               string title, 
                               string out){   
                                 
        using namespace std;
        
        // Open files
        TFile *f1 = TFile::Open(file1.c_str()); TString sf1(file1.c_str()); bool IsJPsi1 = sf1.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        bool IsEta = sf1.Contains("etaBins") ? true :false;
        gROOT->SetStyle("Plain");
        gStyle->SetTitleFillColor(kWhite);      
        TH1 *hFrame = getDefaultFrame();     
        // Get histograms
        if (IsEta){
                TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h1_2D = (TH2F*)h1c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");
        } else {
                TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h1_2D = (TH2F*)h1c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");
        }
        h1->SetTitle(title.c_str());  
          
        // Empty the bins outside of the range for the given subsystem
        if (IsEta){
                emptyBins(h1, sf1);
        }
        setupHisto(h1,1,8);
        
        TLegend *leg = getDefaultLegend(IsEta);
        leg->AddEntry(h1,leg(sf1),"p");
        TCanvas * c1 = drawHistos(h1,IsEta);
        leg->Draw();
        TText *t = new TText(0.4966443,0.9597902,"CMS Preliminary #sqrt{s} =7TeV");   
        t->SetNDC(true);t->SetTextSize(0.03496503); t->Draw();
        c1->Print(out.c_str());        
 }
 // One histo JPsi+Z
 void makeEffPlots(string file1, 
                               string file2,                              
                               string title, 
                               string out, bool ZandJPsi){   
                                 
        using namespace std;
        
        // Open files
        cout << "Opening file " <<file1.c_str()<<"..."<<endl;
        TFile *f1 = TFile::Open(file1.c_str()); TString sf1(file1.c_str()); bool IsJPsi1 = sf1.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        cout << "Opening file " <<file2.c_str()<<"..."<<endl;
        TFile *f2 = TFile::Open(file2.c_str()); TString sf2(file2.c_str()); bool IsJPsi2 = sf2.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        bool IsEta = sf1.Contains("etaBins") ? true :false;
        gROOT->SetStyle("Plain");
        gStyle->SetTitleFillColor(kWhite);      
        TH1 *hFrame = getDefaultFrame();     
        // Get histograms
        TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h1_2D = (TH2F*)h1c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");

        TCanvas *h2c = (TCanvas*)f2->Get("tnpTree"+(sf2.Contains("JPsi")?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h2_2D = (TH2F*)h2c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h2 = (TH1F*)h2_2D->ProjectionX("h2");
        h1->SetTitle(title.c_str());  
          
        // Empty the bins outside of the range for the given subsystem
        setupHisto(h1,1,8);
        setupHisto(h2,1,8);
        TLegend *leg = getDefaultLegend(IsEta);
        leg->AddEntry(h1,leg(sf1),"p");
        TCanvas * c1 = drawHistos(h1,h2,IsEta);
        leg->Draw();
        TText *t = new TText(0.4966443,0.9597902,"CMS Preliminary #sqrt{s} =7TeV");   
        t->SetNDC(true);t->SetTextSize(0.03496503); t->Draw();
        c1->Print(out.c_str());        
 }
 // Two histos
void makeEffPlots(string file1, 
                               string file2,                              
                               string title, 
                               string out){   
                                 
        using namespace std;
        
        // Open files
        TFile *f1 = TFile::Open(file1.c_str()); TString sf1(file1.c_str()); bool IsJPsi1 = sf1.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        TFile *f2 = TFile::Open(file2.c_str()); TString sf2(file2.c_str()); bool IsJPsi2 = sf2.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        bool IsEta = sf1.Contains("etaBins") ? true :false;
        gROOT->SetStyle("Plain");
        gStyle->SetTitleFillColor(kWhite);      
        TH1 *hFrame = getDefaultFrame();     
        // Get histograms
        if (IsEta){
                TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h1_2D = (TH2F*)h1c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");

                TCanvas *h2c = (TCanvas*)f2->Get("tnpTree"+(sf2.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h2_2D = (TH2F*)h2c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h2 = (TH1F*)h2_2D->ProjectionX("h2");

        } else {
                TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h1_2D = (TH2F*)h1c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");

                TCanvas *h2c = (TCanvas*)f2->Get("tnpTree"+(sf2.Contains("JPsi")?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h2_2D = (TH2F*)h2c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h2 = (TH1F*)h2_2D->ProjectionX("h2");
        }
        h1->SetTitle(title.c_str());  
          
        // Empty the bins outside of the range for the given subsystem
        if (IsEta){
                emptyBins(h1, sf1);
                emptyBins(h2, sf2); 
        }
        setupHisto(h1,1,8);
        setupHisto(h2,2,8);
        
        TLegend *leg = getDefaultLegend(IsEta);
        leg->AddEntry(h1,leg(sf1)+"MuOnia","p");
        leg->AddEntry(h2,leg(sf2)+"SingleMu","p");
        TCanvas * c1 = drawHistos(h1,h2,IsEta);
        leg->Draw();
        TText *t = new TText(0.4966443,0.9597902,"CMS Preliminary #sqrt{s} =7TeV");   
        t->SetNDC(true);t->SetTextSize(0.03496503); t->Draw();
        c1->Print(out.c_str());
 }
 // Two histos JPsi+Z
 void makeEffPlots(string file1, 
                               string file2,
                               string file3,
                               string file4,                             
                               string title, 
                               string out, bool ZandJPsi){   
                                 
        using namespace std;
        
        // Open files
        cout << "Opening file " <<file1.c_str()<<"..."<<endl;
        TFile *f1 = TFile::Open(file1.c_str()); TString sf1(file1.c_str()); bool IsJPsi1 = sf1.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        cout << "Opening file " <<file2.c_str()<<"..."<<endl;
        TFile *f2 = TFile::Open(file2.c_str()); TString sf2(file2.c_str()); bool IsJPsi2 = sf2.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        cout << "Opening file " <<file3.c_str()<<"..."<<endl;
        TFile *f3 = TFile::Open(file3.c_str()); TString sf3(file3.c_str()); bool IsJPsi3 = sf3.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        cout << "Opening file " <<file4.c_str()<<"..."<<endl;
        TFile *f4 = TFile::Open(file4.c_str()); TString sf4(file4.c_str()); bool IsJPsi4 = sf4.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        bool IsEta = sf1.Contains("etaBins") ? true :false;
        gROOT->SetStyle("Plain");
        gStyle->SetTitleFillColor(kWhite);      
        TH1 *hFrame = getDefaultFrame();     
        // Get histograms
        TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h1_2D = (TH2F*)h1c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");

        TCanvas *h2c = (TCanvas*)f2->Get("tnpTree"+(sf2.Contains("JPsi")?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h2_2D = (TH2F*)h2c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h2 = (TH1F*)h2_2D->ProjectionX("h2");  

        TCanvas *h3c = (TCanvas*)f3->Get("tnpTree"+(sf3.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h3_2D = (TH2F*)h3c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h3 = (TH1F*)h3_2D->ProjectionX("h3");
        
        TCanvas *h4c = (TCanvas*)f4->Get("tnpTree"+(sf4.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h4_2D = (TH2F*)h4c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h4 = (TH1F*)h4_2D->ProjectionX("h4");           
         
         h1->SetTitle(title.c_str()); 

        setupHisto(h1,1,8);
        setupHisto(h2,1,8);
        setupHisto(h3,2,23);
        setupHisto(h4,2,23);
        TLegend *leg = getDefaultLegend(IsEta);
        leg->AddEntry(h1,leg(sf1),"p");
        leg->AddEntry(h3,leg(sf3),"p");
        TCanvas * c1 = drawHistos(h1,h2,h3,h4,IsEta);
        leg->Draw();
        TText *t = new TText(0.4966443,0.9597902,"CMS Preliminary #sqrt{s} =7TeV");   
        t->SetNDC(true);t->SetTextSize(0.03496503); t->Draw();
        c1->Print(out.c_str());
 }
 // Three histos
 void makeEffPlots(string file1, 
                               string file2, 
                               string file3,                                
                               string title, 
                               string out){   
                                 
        using namespace std;
        
        // Open files
        TFile *f1 = TFile::Open(file1.c_str()); TString sf1(file1.c_str()); bool IsJPsi1 = sf1.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        TFile *f2 = TFile::Open(file2.c_str()); TString sf2(file2.c_str()); bool IsJPsi2 = sf2.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        TFile *f3 = TFile::Open(file3.c_str()); TString sf3(file3.c_str()); bool IsJPsi3 = sf3.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        bool IsEta = sf1.Contains("etaBins") ? true :false;
        gROOT->SetStyle("Plain");
        gStyle->SetTitleFillColor(kWhite);      
        TH1 *hFrame = getDefaultFrame();     
        // Get histograms
        if (IsEta){
                TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h1_2D = (TH2F*)h1c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");

                TCanvas *h2c = (TCanvas*)f2->Get("tnpTree"+(sf2.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h2_2D = (TH2F*)h2c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h2 = (TH1F*)h2_2D->ProjectionX("h2");
                
                TCanvas *h3c = (TCanvas*)f3->Get("tnpTree"+(sf3.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h3_2D = (TH2F*)h3c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h3 = (TH1F*)h3_2D->ProjectionX("h3");
        } else {
                TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h1_2D = (TH2F*)h1c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");

                TCanvas *h2c = (TCanvas*)f2->Get("tnpTree"+(sf2.Contains("JPsi")?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h2_2D = (TH2F*)h2c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h2 = (TH1F*)h2_2D->ProjectionX("h2");
                
                TCanvas *h3c = (TCanvas*)f3->Get("tnpTree"+(sf3.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h3_2D = (TH2F*)h3c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h3 = (TH1F*)h3_2D->ProjectionX("h3");
        }
        h1->SetTitle(title.c_str());  
          
        // Empty the bins outside of the range for the given subsystem
        if (IsEta){
                emptyBins(h1, sf1);
                emptyBins(h2, sf2); 
                emptyBins(h3, sf3);
        }
        setupHisto(h1,1,8);
        setupHisto(h2,2,23);
        setupHisto(h3,3,21);
        
        TLegend *leg = getDefaultLegend(IsEta);
        if (IsEta){
                leg->AddEntry(h1,leg(sf1),"l");
                leg->AddEntry(h2,leg(sf2),"l");
                leg->AddEntry(h3,leg(sf3),"l");        
        }else{
                leg->AddEntry(h1,leg(sf1),"p");
                leg->AddEntry(h2,leg(sf2),"p");
                leg->AddEntry(h3,leg(sf3),"p");        
        }        

        TCanvas * c1 = drawHistos(h1,h2,h3,IsEta);
        leg->Draw();
        TText *t = new TText(0.4966443,0.9597902,"CMS Preliminary #sqrt{s} =7TeV");   
        t->SetNDC(true);t->SetTextSize(0.03496503); t->Draw();
        c1->Print(out.c_str());
 }
 // Four histos
 void makeEffPlots(string file1, 
                               string file2, 
                               string file3,
                               string file4,                             
                               string title, 
                               string out){   
                                 
        using namespace std;
        
        // Open files
        TFile *f1 = TFile::Open(file1.c_str()); TString sf1(file1.c_str()); bool IsJPsi1 = sf1.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        TFile *f2 = TFile::Open(file2.c_str()); TString sf2(file2.c_str()); bool IsJPsi2 = sf2.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        TFile *f3 = TFile::Open(file3.c_str()); TString sf3(file3.c_str()); bool IsJPsi3 = sf3.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        TFile *f4 = TFile::Open(file4.c_str()); TString sf4(file4.c_str()); bool IsJPsi4 = sf4.Contains("JPsi") ? true : false; cout<<"File opened"<<endl;
        bool IsEta = sf1.Contains("etaBins") ? true :false;
        gROOT->SetStyle("Plain");
        gStyle->SetTitleFillColor(kWhite);      
        TH1 *hFrame = getDefaultFrame();     
        // Get histograms
        if (IsEta){
                TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h1_2D = (TH2F*)h1c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");

                TCanvas *h2c = (TCanvas*)f2->Get("tnpTree"+(sf2.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h2_2D = (TH2F*)h2c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h2 = (TH1F*)h2_2D->ProjectionX("h2");
                
                TCanvas *h3c = (TCanvas*)f3->Get("tnpTree"+(sf3.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h3_2D = (TH2F*)h3c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h3 = (TH1F*)h3_2D->ProjectionX("h3");
                
                TCanvas *h4c = (TCanvas*)f4->Get("tnpTree"+(sf4.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h4_2D = (TH2F*)h4c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h4 = (TH1F*)h4_2D->ProjectionX("h4");                
        } else {
                TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h1_2D = (TH2F*)h1c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");

                TCanvas *h2c = (TCanvas*)f2->Get("tnpTree"+(sf2.Contains("JPsi")?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h2_2D = (TH2F*)h2c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h2 = (TH1F*)h2_2D->ProjectionX("h2");
                
                TCanvas *h3c = (TCanvas*)f3->Get("tnpTree"+(sf3.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h3_2D = (TH2F*)h3c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h3 = (TH1F*)h3_2D->ProjectionX("h3");
                
                TCanvas *h4c = (TCanvas*)f4->Get("tnpTree"+(sf4.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h4_2D = (TH2F*)h4c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h4 = (TH1F*)h4_2D->ProjectionX("h4");                
        }
        h1->SetTitle(title.c_str());  
          
        // Empty the bins outside of the range for the given subsystem
        if (IsEta){
                emptyBins(h1, sf1);
                emptyBins(h2, sf2); 
                emptyBins(h3, sf3);
                emptyBins(h4, sf4);
        }
        setupHisto(h1,1,8);
        setupHisto(h2,2,23);
        setupHisto(h3,3,21);
        setupHisto(h4,4,22);
        
        TLegend *leg = getDefaultLegend(IsEta);
        if (IsEta){
                leg->AddEntry(h1,leg(sf1),"l");
                leg->AddEntry(h2,leg(sf2),"l");
                leg->AddEntry(h3,leg(sf3),"l");
                leg->AddEntry(h4,leg(sf4),"l");        
        }else{       
                leg->AddEntry(h1,leg(sf1),"p");
                leg->AddEntry(h2,leg(sf2),"p");
                leg->AddEntry(h3,leg(sf3),"p");
                leg->AddEntry(h4,leg(sf4),"p");
        }                
        TCanvas * c1 = drawHistos(h1,h2,h3,h4,IsEta);
        leg->Draw();
        TText *t = new TText(0.4966443,0.9597902,"CMS Preliminary #sqrt{s} =7TeV");   
        t->SetNDC(true);t->SetTextSize(0.03496503); t->Draw();    
        c1->Print(out.c_str());
 }
  // Six histos
 void makeEffPlots(string file1, 
                               string file2, 
                               string file3,
                               string file4,
                               string file5,                                
                               string file6,                             
                               string title, 
                               string out){   
                                 
        using namespace std;
        
        // Open files
        TFile *f1 = TFile::Open(file1.c_str()); TString sf1(file1.c_str()); bool IsJPsi1 = sf1.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        TFile *f2 = TFile::Open(file2.c_str()); TString sf2(file2.c_str()); bool IsJPsi2 = sf2.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        TFile *f3 = TFile::Open(file3.c_str()); TString sf3(file3.c_str()); bool IsJPsi3 = sf3.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        TFile *f4 = TFile::Open(file4.c_str()); TString sf4(file4.c_str()); bool IsJPsi4 = sf4.Contains("JPsi") ? true : false; cout<<"File opened"<<endl;
        TFile *f5 = TFile::Open(file5.c_str()); TString sf5(file5.c_str()); bool IsJPsi5 = sf5.Contains("JPsi") ? true : false; cout<<"File opened"<<endl;
        TFile *f6 = TFile::Open(file6.c_str()); TString sf6(file6.c_str()); bool IsJPsi6 = sf6.Contains("JPsi") ? true : false; cout<<"File opened"<<endl;                
        bool IsEta = sf1.Contains("etaBins") ? true :false;
        gROOT->SetStyle("Plain");
        gStyle->SetTitleFillColor(kWhite);      
        TH1 *hFrame = getDefaultFrame();     
        // Get histograms
        if (IsEta){
                TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h1_2D = (TH2F*)h1c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");

                TCanvas *h2c = (TCanvas*)f2->Get("tnpTree"+(sf2.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h2_2D = (TH2F*)h2c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h2 = (TH1F*)h2_2D->ProjectionX("h2");
                
                TCanvas *h3c = (TCanvas*)f3->Get("tnpTree"+(sf3.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h3_2D = (TH2F*)h3c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h3 = (TH1F*)h3_2D->ProjectionX("h3");
                
                TCanvas *h4c = (TCanvas*)f4->Get("tnpTree"+(sf4.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h4_2D = (TH2F*)h4c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h4 = (TH1F*)h4_2D->ProjectionX("h4");                
                
                TCanvas *h5c = (TCanvas*)f5->Get("tnpTree"+(sf5.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h5_2D = (TH2F*)h5c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h5 = (TH1F*)h5_2D->ProjectionX("h5");
                
                TCanvas *h6c = (TCanvas*)f6->Get("tnpTree"+(sf6.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/eta_pt_PLOT_pair_drM2_bin0");
                TH2F *h6_2D = (TH2F*)h6c->FindObject("eta_pt_PLOT_pair_drM2_bin0");
                TH1F *h6 = (TH1F*)h6_2D->ProjectionX("h6");                  
        } else {
                TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h1_2D = (TH2F*)h1c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");

                TCanvas *h2c = (TCanvas*)f2->Get("tnpTree"+(sf2.Contains("JPsi")?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h2_2D = (TH2F*)h2c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h2 = (TH1F*)h2_2D->ProjectionX("h2");
                
                TCanvas *h3c = (TCanvas*)f3->Get("tnpTree"+(sf3.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h3_2D = (TH2F*)h3c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h3 = (TH1F*)h3_2D->ProjectionX("h3");
                
                TCanvas *h4c = (TCanvas*)f4->Get("tnpTree"+(sf4.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h4_2D = (TH2F*)h4c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h4 = (TH1F*)h4_2D->ProjectionX("h4");                
                
                TCanvas *h5c = (TCanvas*)f5->Get("tnpTree"+(sf5.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h5_2D = (TH2F*)h5c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h5 = (TH1F*)h5_2D->ProjectionX("h5");          
                
                TCanvas *h6c = (TCanvas*)f6->Get("tnpTree"+(sf6.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
                TH2F *h6_2D = (TH2F*)h6c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
                TH1F *h6 = (TH1F*)h6_2D->ProjectionX("h6");                                          
        }
        h1->SetTitle(title.c_str());  
          
        // Empty the bins outside of the range for the given subsystem
        if (IsEta){
                emptyBins(h1, sf1);
                emptyBins(h2, sf2); 
                emptyBins(h3, sf3);
                emptyBins(h4, sf4);
                emptyBins(h5, sf5);
                emptyBins(h6, sf6);
        }
        setupHisto(h1,1,8);
        setupHisto(h2,2,23);
        setupHisto(h3,3,21);
        setupHisto(h4,4,22);
        setupHisto(h5,5,29);
        setupHisto(h6,6,24);
        
        TLegend *leg = getDefaultLegend(IsEta);
        if (IsEta){
                leg->AddEntry(h1,leg(sf1),"l");
                leg->AddEntry(h2,leg(sf2),"l");
                leg->AddEntry(h3,leg(sf3),"l");
                leg->AddEntry(h4,leg(sf4),"l");
                leg->AddEntry(h5,leg(sf5),"l");
                leg->AddEntry(h6,leg(sf6),"l");
        }else{
                leg->AddEntry(h1,leg(sf1),"p");
                leg->AddEntry(h2,leg(sf2),"p");
                leg->AddEntry(h3,leg(sf3),"p");
                leg->AddEntry(h4,leg(sf4),"p");
                leg->AddEntry(h5,leg(sf5),"p");
                leg->AddEntry(h6,leg(sf6),"p");                
        
        }
        TCanvas * c1 = drawHistos(h1,h2,h3,h4,h5,h6,IsEta);
        leg->Draw();
        TText *t = new TText(0.4966443,0.9597902,"CMS Preliminary #sqrt{s} =7TeV");   
        t->SetNDC(true);t->SetTextSize(0.03496503); t->Draw();      
        c1->Print(out.c_str());
 }
   // Six histos JPsi+Z
void makeEffPlots(string file1, 
                               string file2,
                               string file3,
                               string file4,               
                               string file5,
                               string file6,
                               string file7,
                               string file8,
                               string file9,
                               string file10,
                               string file11,
                               string file12,              
                               string title, 
                               string out, bool ZandJPsi){   
                                 
        using namespace std;
        
        // Open files
        cout << "Opening file " <<file1.c_str()<<"..."<<endl;
        TFile *f1 = TFile::Open(file1.c_str()); TString sf1(file1.c_str()); bool IsJPsi1 = sf1.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        cout << "Opening file " <<file2.c_str()<<"..."<<endl;
        TFile *f2 = TFile::Open(file2.c_str()); TString sf2(file2.c_str()); bool IsJPsi2 = sf2.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        cout << "Opening file " <<file3.c_str()<<"..."<<endl;
        TFile *f3 = TFile::Open(file3.c_str()); TString sf3(file3.c_str()); bool IsJPsi3 = sf3.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        cout << "Opening file " <<file4.c_str()<<"..."<<endl;
        TFile *f4 = TFile::Open(file4.c_str()); TString sf4(file4.c_str()); bool IsJPsi4 = sf4.Contains("JPsi") ? true : false; cout<<"File opened"<<endl; 
        cout << "Opening file " <<file5.c_str()<<"..."<<endl;
        TFile *f5 = TFile::Open(file5.c_str()); TString sf5(file5.c_str()); bool IsJPsi5 = sf5.Contains("JPsi") ? true : false; cout<<"File opened"<<endl;
        cout << "Opening file " <<file6.c_str()<<"..."<<endl;
        TFile *f6 = TFile::Open(file6.c_str()); TString sf6(file6.c_str()); bool IsJPsi6 = sf6.Contains("JPsi") ? true : false; cout<<"File opened"<<endl;
        cout << "Opening file " <<file7.c_str()<<"..."<<endl;
        TFile *f7 = TFile::Open(file7.c_str()); TString sf7(file7.c_str()); bool IsJPsi7 = sf7.Contains("JPsi") ? true : false; cout<<"File opened"<<endl;
        cout << "Opening file " <<file8.c_str()<<"..."<<endl;
        TFile *f8 = TFile::Open(file8.c_str()); TString sf8(file8.c_str()); bool IsJPsi8 = sf8.Contains("JPsi") ? true : false; cout<<"File opened"<<endl;
        cout << "Opening file " <<file9.c_str()<<"..."<<endl;
        TFile *f9 = TFile::Open(file9.c_str()); TString sf9(file9.c_str()); bool IsJPsi9 = sf9.Contains("JPsi") ? true : false; cout<<"File opened"<<endl;
        cout << "Opening file " <<file10.c_str()<<"..."<<endl;
        TFile *f10 = TFile::Open(file10.c_str()); TString sf10(file10.c_str()); bool IsJPsi10 = sf10.Contains("JPsi") ? true : false; cout<<"File opened"<<endl;
        cout << "Opening file " <<file11.c_str()<<"..."<<endl;
        TFile *f11 = TFile::Open(file11.c_str()); TString sf11(file11.c_str()); bool IsJPsi11 = sf11.Contains("JPsi") ? true : false; cout<<"File opened"<<endl;
        cout << "Opening file " <<file12.c_str()<<"..."<<endl;
        TFile *f12 = TFile::Open(file12.c_str()); TString sf12(file12.c_str()); bool IsJPsi12 = sf12.Contains("JPsi") ? true : false; cout<<"File opened"<<endl;
        
        bool IsEta = sf1.Contains("etaBins") ? true :false;
        gROOT->SetStyle("Plain");
        gStyle->SetTitleFillColor(kWhite);      
        TH1 *hFrame = getDefaultFrame();     
        // Get histograms
        TCanvas *h1c = (TCanvas*)f1->Get("tnpTree"+(sf1.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h1_2D = (TH2F*)h1c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h1 = (TH1F*)h1_2D->ProjectionX("h1");

        TCanvas *h2c = (TCanvas*)f2->Get("tnpTree"+(sf2.Contains("JPsi")?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h2_2D = (TH2F*)h2c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h2 = (TH1F*)h2_2D->ProjectionX("h2");  

        TCanvas *h3c = (TCanvas*)f3->Get("tnpTree"+(sf3.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h3_2D = (TH2F*)h3c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h3 = (TH1F*)h3_2D->ProjectionX("h3");
        
        TCanvas *h4c = (TCanvas*)f4->Get("tnpTree"+(sf4.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h4_2D = (TH2F*)h4c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h4 = (TH1F*)h4_2D->ProjectionX("h4");   
        
        TCanvas *h5c = (TCanvas*)f5->Get("tnpTree"+(sf5.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h5_2D = (TH2F*)h5c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h5 = (TH1F*)h5_2D->ProjectionX("h5");          
        
        TCanvas *h6c = (TCanvas*)f6->Get("tnpTree"+(sf6.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h6_2D = (TH2F*)h6c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h6 = (TH1F*)h6_2D->ProjectionX("h6"); 
        
        TCanvas *h7c = (TCanvas*)f7->Get("tnpTree"+(sf7.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h7_2D = (TH2F*)h7c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h7 = (TH1F*)h7_2D->ProjectionX("h7"); 
        
        TCanvas *h8c = (TCanvas*)f8->Get("tnpTree"+(sf8.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h8_2D = (TH2F*)h8c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h8 = (TH1F*)h8_2D->ProjectionX("h8"); 
        
        TCanvas *h9c = (TCanvas*)f9->Get("tnpTree"+(sf9.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h9_2D = (TH2F*)h9c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h9 = (TH1F*)h9_2D->ProjectionX("h9"); 
        
        TCanvas *h10c = (TCanvas*)f10->Get("tnpTree"+(sf10.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h10_2D = (TH2F*)h10c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h10 = (TH1F*)h10_2D->ProjectionX("h10"); 
        
        TCanvas *h11c = (TCanvas*)f11->Get("tnpTree"+(sf11.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h11_2D = (TH2F*)h11c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h11 = (TH1F*)h11_2D->ProjectionX("h11"); 
        
        TCanvas *h12c = (TCanvas*)f12->Get("tnpTree"+(sf12.Contains("JPsi") ?TString("JPsi") : TString("Z"))+"/Eff1/fit_eff_plots/pt_abseta_PLOT_pair_drM2_bin0");
        TH2F *h12_2D = (TH2F*)h12c->FindObject("pt_abseta_PLOT_pair_drM2_bin0");
        TH1F *h12 = (TH1F*)h12_2D->ProjectionX("h12");                 
         
         h1->SetTitle(title.c_str()); 

        setupHisto(h1,8,8);
        setupHisto(h2,8,8);
        setupHisto(h3,1,23);
        setupHisto(h4,1,23);
        setupHisto(h5,6,21);
        setupHisto(h6,6,21);
        setupHisto(h7,7,22);
        setupHisto(h8,7,22);
        setupHisto(h9,9,29);
        setupHisto(h10,9,29);
        setupHisto(h11,2,30);
        setupHisto(h12,2,30);
        TLegend *leg = getDefaultLegend(IsEta);
        leg->AddEntry(h1,leg(sf1),"p");
        leg->AddEntry(h3,leg(sf3),"p");
        leg->AddEntry(h5,leg(sf5),"p");
        leg->AddEntry(h7,leg(sf7),"p");
        leg->AddEntry(h9,leg(sf9),"p");
        leg->AddEntry(h11,leg(sf11),"p");
        TCanvas * c1 = drawHistos(h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11,h12,IsEta);
        leg->Draw();
        TLatex *t = new TLatex(0.4966443,0.9597902,"CMS Preliminary #sqrt{s} =7TeV");   
        t->SetNDC(true);t->SetTextSize(0.03496503); t->Draw();    
        c1->Print(out.c_str());
 }
