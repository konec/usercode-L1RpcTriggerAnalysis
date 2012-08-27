#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <TROOT.h>
#include <TFile.h>
#include <TText.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TProfile.h>
#include <TLine.h>
#include <TLatex.h>
#include "Style.C"  // "/afs/cern.ch/cms/L1/rpc/plots/macros/Style.C"
using namespace std;
//
// Extracts sector number from LB name
//
Int_t getSector(const char *lbname) {
  if(lbname) {
    Int_t sec;
    if ( (sscanf(lbname,"LB_RB%*2d_S%2d", &sec)==1 ||
	  sscanf(lbname,"LB_RE%*2d_S%2d", &sec)==1   ) && 
         sec>=1 && sec<=12) {
      cout << "getSector: " << lbname << " --> " << sec << endl;
      return sec;
    }
  }
  cout << "getSector: " << lbname << " --> BAD" << endl;
  return -1;
}
Int_t getSector(TString &lbname) {
  return getSector(lbname.Data());
}
//
// Counts how many LBs from the list belongs to a given sector
//
Int_t getNumLbPerSector(const Int_t sec, vector<TString> &list) {
  Int_t counter=0;
  if( sec>=1 && sec<=12 && list.size()>0 ) {
    for(vector<TString>::iterator it=list.begin(); it!=list.end(); it++) {
      if(sec==getSector(*it)) counter++;
    }
  }
  return counter;
}
//
// Parses delays.txt file created by synchroMerger and plots: 
// - distribution of BX average for LBs 
// - distribution of BX rms for LBs 
// - cumulative distribution of LBs vs window size around L1A
//   that contains a given fraction of LB hits.
//
// version w/ blacklisted LBs
void synchro(// a text file with BX histograms for all links
	       TString datafile,
	       std::vector<TString> &blacklist,
	       TString outfile="",    
	       TString runrange="",
	       TString datasettype="",
	       TString geolocation="") {

  const Float_t legendFontSize = 0.030;
  const Float_t legendOffset   = 0.030;

  //gROOT->Reset();
  setTDRStyle();
  TStyle *tdrStyle=(TStyle*)(gROOT->FindObject("tdrStyle"));
  tdrStyle->SetOptStat(0);
  tdrStyle->SetOptLogy(1);
  tdrStyle->SetOptTitle(0);//off title
  tdrStyle->SetPadGridY(1);
  tdrStyle->SetPadRightMargin(0.02);
  tdrStyle->SetPalette(1);
  tdrStyle->SetOptTitle(1);
  gROOT->ForceStyle();

  if(outfile=="") outfile="drawSynchro";
  TFile f( (outfile+".root").Data(), "RECREATE");
  f.mkdir("Synchro");
  f.cd("Synchro");
  f.ls();
  //tdrStyle->Write();
  //gStyle->Write();

  cout << "DrawSynchro:"<<endl 
       << " Input text file      = " << datafile << endl
       << " # of blacklisted LBs = " << blacklist.size() << endl
       << " Output file prefix   = " << outfile
       << endl;

  // fraction of hits in a window to be studied
  vector<Double_t> frac;
  frac.push_back(1.00); // 100% of hits within given window
  frac.push_back(0.99); // 99%  of hits within given window
  frac.push_back(0.95); // 95%  of hits within given window
  frac.push_back(0.90); // 90%  of hits within given window

  // map LBid BX
  const Int_t PEAK_BIN = 3;    // L1A position (BX=0) in the input text file
  const Int_t MAX_OFFPEAK = 3; // maximum +- 3BX off L1A position (BX=0)

  char label[200];
  char title[200];
  // prepare text labels for BX window categories
  vector<TString> category;
  for(Int_t nbx=0; nbx<=MAX_OFFPEAK; nbx++) {
    if(nbx==0) sprintf(label, "BX=0");
    else       sprintf(label, "#pm%d BX", nbx);
    category.push_back(label);
  }
  // prepare text labels for Sector names
  vector<TString> sector;
  for(Int_t sec=1; sec<=12; sec++) {
    sprintf(label, "%d", sec);
    sector.push_back(label);
  }

  Int_t nbinsX, nbinsY;
  Float_t widthX, widthY;
  //
  // Histograms that depend on a given threshold for fraction of events
  //
  // 1D : Number of LBs per window category (text bin labels)
  vector<TH1F*> hLB_win;
  for(UInt_t ifrac=0; ifrac<frac.size(); ifrac++) {
    sprintf(title,"LBs with %.0f%% of hits in a given window;"
	          "Window wrt L1A with %.0f%% of hits;Link Boards / category", 
	          frac[ifrac]*100, frac[ifrac]*100);
    sprintf(label,"hLB_win%.0f", frac[ifrac]*100);
    TH1F *h=new TH1F(label, title, MAX_OFFPEAK+1, 0, MAX_OFFPEAK+1);
    for(Int_t nbx=0; nbx<=MAX_OFFPEAK; nbx++) {
      h->GetXaxis()->SetBinLabel(nbx+1,category[nbx]);
    }
    h->Sumw2();
    h->StatOverflows(true); // to include underflows/overflows in mean/rms
    hLB_win.push_back(h);
  }

  //
  // Common histograms (do not depend on fraction of events).
  //


  // 1D : Hit arrival time 
  sprintf(title,"RPC hit synchronization;Chamber hit timing wrt L1A [BX];Hits / bin");
  widthX=1.0 ; // bin width of 1 BX
  nbinsX=7;    // nbins for [-3 BX, +3 BX] range
  TH1F *hLB_Bx=new TH1F("hLB_Bx", title, 
                        nbinsX, 
			0.0-(Float_t)nbinsX/2*widthX, 
			0.0+(Float_t)nbinsX/2*widthX);
  hLB_Bx->Sumw2();
  hLB_Bx->StatOverflows(true); // to include underflows/overflows in mean/rms


  // 1D : Mean BX (1 entry=1 LB)
  sprintf(title,"Average signal arrival per Link Board;"
                "Avg signal arrival wrt L1A per Link Board [BX];"
	        "Link Boards / bin");
  widthX=1.0/50 ;                 // bin width of 1/25 BX = 1ns
  nbinsX=2*(Int_t)(3.0/widthX)+1; // nbins for [-3 BX, +3 BX] range
  TH1F *hLB_Mean=new TH1F("hLB_Mean", title, 
			  nbinsX, 
			  0.0-(Int_t)(nbinsX/2)*widthX, 
			  0.0+(Int_t)(nbinsX/2)*widthX);
  hLB_Mean->Sumw2();
  hLB_Mean->StatOverflows(true); // to include underflows/overflows in mean/rms


  // 1D : Rms BX (1 entry=1 LB)
  sprintf(title,"Average signal spread per Link Board;"
	  "Avg signal spread per Link Board [BX];"
	  "Link Boards / bin");
  widthX=1.0/50 ;             // bin width of 1/25 BX = 1ns
  nbinsX=(Int_t)(7.0/widthX); // nbins for [0 BX, 7 BX[ range
  TH1F *hLB_Rms=new TH1F("hLB_Rms", title,
			  nbinsX, 
			  0.0,
			  0.0+nbinsX*widthX);
  hLB_Rms->StatOverflows(true); // to include underflows/overflows in mean/rms
  hLB_Rms->Sumw2();


  // 2D : Mean BX vs Rms BX (1 entry=1 LB)
  sprintf(title,"Average signal spread vs average signal arrival per Link Board;"
	  "Avg signal arrival wrt L1A per Link Board [BX];"
	  "Avg signal spread per Link Board [BX];"
	  "Link Boards / bin");
  widthX=1.0/50 ;                   // bin width of 1/25 BX = 1ns
  nbinsX=2*(Int_t)(3.0/widthX)+1; // nbins for [-3 BX, +3 BX] range
  widthY=1.0/50 ;                   // bin width of 1/25 BX = 1ns
  nbinsY=(Int_t)(7.0/widthY);     // nbins for [0 BX, 7 BX[ range
  TH2F *hLB_Rms_V_Mean=new TH2F("hLB_Rms_V_Mean", title, 
				//70*5, -3., 4., 70*5, 0., 7.);
				nbinsX, 
				0.0-(Int_t)(nbinsX/2)*widthX, 
				0.0+(Int_t)(nbinsX/2)*widthX,
				nbinsY,
				0.0,
				0.0+nbinsY*widthY);


  hLB_Rms_V_Mean->StatOverflows(true); // to include underflows/overflows in mean/rms
  hLB_Rms_V_Mean->Sumw2();


  // 1D : Profile of average hit arrival times per LB vs Sector
  sprintf(title,"Average signal arrival time per Link Board vs Sector;"
	        "Sector;"
	        "Avg signal arrival wrt L1A per Link Board [BX]");
  TProfile *pLB_Mean_V_Sector = new TProfile("pLB_Mean_V_Sector", title, 12, -0.5, 12.5, -3.5, 3.5);
  for(Int_t sec=1; sec<=12; sec++) {
    sprintf(label, "%d", sec);
    pLB_Mean_V_Sector->GetXaxis()->SetBinLabel(sec, label);
  }
  pLB_Mean_V_Sector->Sumw2();
  pLB_Mean_V_Sector->StatOverflows(true); // to include underflows/overflows in mean/rms


  // 1D : Profile of hit arrival times vs Sector
  sprintf(title,"Average signal arrival time vs Sector;"
	        "Sector;"
	        "Avg signal arrival wrt L1A [BX]");
  TProfile *pLB_Bx_V_Sector = new TProfile("pLB_Bx_V_Sector", 
					   title, 12, -0.5, 12.5, -3.5, 3.5);
  for(Int_t sec=1; sec<=12; sec++) {
    sprintf(label, "%d", sec);
    pLB_Bx_V_Sector->GetXaxis()->SetBinLabel(sec, label);
  }
  pLB_Bx_V_Sector->Sumw2();
  pLB_Bx_V_Sector->StatOverflows(true); // to include underflows/overflows in mean/rms


  // 1D : Number of hits vs Sector
  sprintf(title,"Occupancy per Sector;"
	        "Sector;"
	        "Number of events");
  TH1F *hLB_Evts_V_Sector = new TH1F("hLB_Evts_V_Sector", title, 12, -0.5, 12.5);
  for(Int_t sec=1; sec<=12; sec++) {
    sprintf(label, "%d", sec);
    hLB_Evts_V_Sector->GetXaxis()->SetBinLabel(sec, label);
  }
  hLB_Evts_V_Sector->Sumw2();
  hLB_Evts_V_Sector->StatOverflows(true); // to include underflows/overflows in mean/rms
  
  ////////////////////////////////////////////
  ////////////////////////////////////////////

  vector<TString> LB_alive;
  vector<TString> LB_dead;
  vector<TString> LB_blacklisted;

  Int_t nLB_alive = 0;        // LBs with non-empty hit BX distribution
  Int_t nLB_dead  = 0;        // LBs with empty hit BX distribution 
  Int_t nLB_blacklisted  = 0; // problematic LBs to be excluded
  Int_t nLB_total = 0;        // total LBs (empty+alive+excluded) 
  
  // parses input text file, 
  // computes hits fraction for a given window wrt L1A bunch crossing (BX=0),
  // computes mean and rms of BX hit distribution. 
  ifstream file;
  file.open(datafile.Data());
  file.is_open();
  if (file.fail()) {
    cout << "ERROR: Can't open input text file: "<<datafile<<endl; 
    return;
  }
  while (!file.eof()) {
    string line;
    getline(file, line);
    //cout<<"RAW="<<line<<endl;
    size_t beg,end;
    if( (beg = line.find("counts:"))!=string::npos &&
	(end = line.find("paths:"))!=string::npos    ) {
      Int_t bin[7]={0, 0, 0, 0, 0, 0, 0};
      string sel=line.substr(beg, end-beg);
      char lbname[100];

      if(sscanf(sel.c_str(),
		"counts:%5d%5d%5d%5d%5d%5d%5d", 
		&(bin[0]), 
		&(bin[1]),
		&(bin[2]),
		&(bin[3]),
		&(bin[4]),
		&(bin[5]),
		&(bin[6]) )==7 &&
	 sscanf(line.c_str(),"%20s", lbname)==1 ) {

	// is excluded?
       	if( find(blacklist.begin(), blacklist.end(), lbname)
	    !=blacklist.end() ) {
	  // check for duplicates
	  if(find(LB_blacklisted.begin(), LB_blacklisted.end(), lbname)
	     ==LB_blacklisted.end()) {
	    LB_blacklisted.push_back( lbname );
	    //nLB_alive++;
	  }
	  continue;
	}
	  
	Double_t sumw=0.;
	Double_t sumw2=0.;
	Double_t mean=0.;
	Double_t mean2=0.;
	Double_t rms=0.;
	for(Int_t i=0; i<7; i++) {
	  if(bin[i]<0) {
	    cout<<"ERROR: Number of hit counts must not be < 0"<<endl;
	    return;
	  }
	  if(bin[i]>0) {
/* from ROOT manual:
 * By default, for each bin, the sum of weights is computed at fill time. One can also call TH1::Sumw2 to force the storage and computation of the sum of the square of weights per bin. If Sumw2 has been called, the error per bin is computed as the sqrt(sum of squares of weights), otherwise the error is set equal to the sqrt(bin content).
 */
	    for(Long_t j=0; j<bin[i]; j++) {
	      hLB_Bx->Fill(i-PEAK_BIN, 1.0); // bin[i]);
	    }
	  }
	  sumw  += bin[i];
	  sumw2 += bin[i]*bin[i];
	  mean  += (i-PEAK_BIN) * bin[i];
	  mean2 += (i-PEAK_BIN)*(i-PEAK_BIN) * bin[i];
	}
	if(sumw>0.) {
	  mean  /= sumw; 
	  mean2 /= sumw;
	  rms   = sqrt(mean2-mean*mean);

	  // fill mean, rms histograms
	  hLB_Mean->Fill(mean);
	  hLB_Rms->Fill(rms);
	  hLB_Rms_V_Mean->Fill(mean, rms);
	  Int_t sec=getSector(lbname);
	  if(sec>0) {
	    pLB_Mean_V_Sector->Fill(sector[sec-1], mean);
/* from ROOT manual:
 * By default, for each bin, the sum of weights is computed at fill time. One can also call TH1::Sumw2 to force the storage and computation of the sum of the square of weights per bin. If Sumw2 has been called, the error per bin is computed as the sqrt(sum of squares of weights), otherwise the error is set equal to the sqrt(bin content).
 */
	    for(Long_t j=0; j<sumw; j++) {
	      hLB_Evts_V_Sector->Fill(sector[sec-1], 1.0);
	    }
	    for(Int_t i=0; i<7; i++) if (bin[i]>0) {
	      for(Long_t j=0; j<bin[i]; j++) {
		pLB_Bx_V_Sector->Fill(sector[sec-1], i-PEAK_BIN, 1.0 );//bin[i]);
	      } 
	    }
	  }

	  // count # of alive LBs
	  // check for duplicates
	  if(find(LB_alive.begin(), LB_alive.end(), lbname)==LB_alive.end()) {
	    LB_alive.push_back( lbname );
	    //nLB_alive++;
	  }

	} else {
	  // count # of dead LBs
	  // check for duplicates
	  if(find(LB_dead.begin(), LB_dead.end(), lbname)==LB_dead.end()) {
	    LB_dead.push_back( lbname );
	    //nLB_dead++;
	  }
	}

	cout << lbname << " :  mean="<<mean<<", rms="<<rms<<endl;
	//cout<<"FOUND="<<bin[0]<<" "<<bin[1]<<" "<<bin[2]<<" "
	//  <<bin[3]<<" "<<bin[4]<<" "<<bin[5]<<" "<<bin[6]<<endl;

	// loop over windows sizes and fill LB distribution histo
	for(Int_t nbx=0; nbx<=MAX_OFFPEAK; nbx++) {
	  Int_t sum_window=0;           // LB sum of hits for category
	  Int_t sum_total=0;            // LB total sum of hits
	  for(Int_t i=0; i<7; i++) {
	    sum_total += bin[i];
	    Int_t bx_offset=i-PEAK_BIN; // offset from L1A peak in BX
	    if ( fabs(bx_offset)<=nbx ) { 
	      sum_window += bin[i];              
	    }
	  }
	  // fraction of hits in a given window
	  Double_t fr=(sum_total>0 ? 
                       (Double_t)sum_window/(Double_t)sum_total : 0.);
	  // fill the histogram only if fraction of hits is sufficient 
	  cout << lbname << " :  size=[" << -nbx << "," <<nbx << "]"
	       << ", frac=" << fr
	       << ", inside=" << sum_window 
	       << ", total=" << sum_total << endl;
	  for(Int_t ifrac=0; ifrac<frac.size(); ifrac++) {
	    if( fr >= frac[ifrac] ) {
	      cout << "Filling: fr="<<fr
		   << " ifrac="<<ifrac
		   << " frac[ifrac]="<<frac[ifrac]
		   << " nbx="<<nbx
		   << " category[nbx]="<<category[nbx]<<endl; 
	      hLB_win[ifrac]->Fill( category[nbx], 1.0 );
	      cout << lbname << " : filling category "<<nbx<<endl;
	    }
	  } // end of threshold loop	
	} // end of windows size loop

      }

    }
  }
  file.close();

  // Total LBs
  nLB_alive=LB_alive.size();
  nLB_dead=LB_dead.size();
  nLB_blacklisted=LB_blacklisted.size();
  nLB_total = nLB_alive + nLB_dead + nLB_blacklisted;
  cout << "Number of Link Boards: total="<<nLB_total
       << ", non-empty="<<nLB_alive
       << ", empty="<<nLB_dead
       << ", blacklisted="<<nLB_blacklisted<<endl;
  if(nLB_blacklisted>0) {
    cout << "Excluded LBs that exist in the input file:"<<endl;
    for(Int_t i=0; i<nLB_blacklisted; i++) {
      cout << LB_blacklisted[i] << endl;
    }
  }

  // temporary pointers
  TH1F *h;
  TH2F *h2;
  TProfile *p;
  TCanvas *c1;
  TLatex *t;//TLatex *t;
  TLine *l;
  // position of the legend
  Float_t xpos, ypos;
  Int_t ioff;
  Float_t rangemin,rangemax;

  Double_t meanX, meanY, rmsX, rmsY;
  // NOTE: Using SetRangeUser(), SetRange() causes ROOT
  // recompute MEAN/RMS using bin center values in the
  // viewable range! In order to get true MEAN/RMS values
  // one has to do GetXaxis()->UnZoom() after drawing the
  // canvas and before calling GetMean(), GetRms().

  ////////////////////////////////////////////////////////////////

  h=hLB_Bx;
  h->Scale(1.0/h->Integral()); // normalize to 1
  h->SetYTitle("Fraction of hits");
  // Draw: Normalized distribution of hit BX
  //  setTDRStyle();
  tdrStyle->SetOptStat(0);  
  tdrStyle->SetOptLogy(1);
  tdrStyle->SetOptTitle(0);//off title
  tdrStyle->SetPadGridY(1);
  tdrStyle->SetPadRightMargin(0.02);
  //tdrStyle->SetPaintTextFormat("5.5f");  // for TEXT bin values
  tdrStyle->SetPalette(1);
  tdrStyle->SetOptStat(000111100);
  tdrStyle->SetStatY(0.9);                
  tdrStyle->SetStatX(0.9);                
  tdrStyle->SetStatW(0.2);                
  tdrStyle->SetStatH(0.05);                
  gROOT->ForceStyle();
  c1 = new TCanvas( outfile + "_" + h->GetName(),
		    h->GetTitle(),
		    0,0,600,600);
  c1->SetTitle( h->GetTitle() );
  c1->Draw();
  gPad->SetLeftMargin(0.17);
  gPad->SetBottomMargin(0.15);
  h->SetTitleSize(0.045,"Y");
  h->SetTitleSize(0.045,"X");
  h->SetTitleOffset(1.5,"Y");
  h->SetTitleOffset(1.2,"X");
  //gPad->SetLeftMargin(0.12);
  //h->SetTitleOffset(.95,"Y");
  //h->GetYaxis()->SetTitle("Fraction of hits");
  //h->GetXaxis()->SetTitle("Chamber hits timing vs L1A [BX]");
  h->SetMarkerSize(1.7);  // for TEXT bin values
  h->SetMarkerColor(4);   // for TEXT bin valued
  h->SetLineStyle(1);
  h->SetMarkerStyle(1);
  h->SetStats(false);

  // get statistics for full range
  h->GetXaxis()->UnZoom();
  meanX = h->GetMean();
  rmsX = h->GetRMS();

  h->SetFillColor(4); h->SetFillStyle(3003);
  h->DrawCopy("E1 E0");
  h->DrawCopy("HIST,SAME");
  gPad->Update();

  // header
  //t = new TLatex(0.3,0.94, "RPC Hit Synchronization ");ry #sqrt{s}
  //t->SetNDC(true);t->DrawClone();

  xpos=0.20;
  ypos=0.85;
  ioff=0;

  //t = new TLatex(.45,.95,"CMS Preliminary    #sqrt{s} = 8TeV");
  //t->SetNDC(true);t->SetTextSize(0.032); t->DrawClone();

  // run range
  if(geolocation!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, geolocation); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(runrange!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, runrange); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(datasettype!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, datasettype); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  // statistics
  // NOTE: Using SetRangeUser(), SetRange() causes ROOT
  // recompute MEAN/RMS using bin center values in the
  // viewable range! In order to get true MEAN/RMS values
  // one has to do GetXaxis()->UnZoom() after drawing the
  // canvas and before calling GetMean(), GetRms().
  //h->GetXaxis()->UnZoom();
  sprintf(label,"Avg time = %6.4f BX", meanX);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(2); t->DrawClone();

  sprintf(label,"Rms = %6.4f BX", rmsX);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(2); t->DrawClone();

  // Hit count
  t = new TLatex(xpos, ypos-ioff*legendOffset, "Number of Link Boards:"); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - total",nLB_total);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - w/o hits",nLB_dead);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  if(nLB_blacklisted>0) {
    sprintf(label,"%d - bad, excluded",nLB_blacklisted);
    t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();
  }
    
  //h->Write();
  c1->Write();
  c1->Print( outfile + "_" + h->GetName() + ".png", "png" );
  c1->Print( outfile + "_" + h->GetName() + ".C", "cxx" );
  c1->Print( outfile + "_" + h->GetName() + ".pdf", "pdf" );
  c1->Print( outfile + "_" + h->GetName() + ".eps", "eps");

  ////////////////////////////////////////////////////////////////


  h=hLB_Mean;
  // Draw: Distribution of LB mean BX
  //  setTDRStyle();
  tdrStyle->SetOptStat(0);  
  tdrStyle->SetOptLogy(1);
  tdrStyle->SetOptTitle(0);//off title
  tdrStyle->SetPadGridY(1);
  tdrStyle->SetPadRightMargin(0.02);
  //tdrStyle->SetPaintTextFormat("5.5f");  // for TEXT bin values
  tdrStyle->SetPalette(1);
  tdrStyle->SetOptStat(000111100);
  tdrStyle->SetStatY(0.9);                
  tdrStyle->SetStatX(0.9);                
  tdrStyle->SetStatW(0.2);                
  tdrStyle->SetStatH(0.05);                
  gROOT->ForceStyle();
  c1 = new TCanvas( outfile + "_" + h->GetName(),
		    h->GetTitle(),
		    0,0,600,600);
  c1->Draw();
  gPad->SetLeftMargin(0.17);
  gPad->SetBottomMargin(0.15);
  h->SetTitleSize(0.045,"Y");
  h->SetTitleSize(0.045,"X");
  h->SetTitleOffset(1.5,"Y");
  h->SetTitleOffset(1.2,"X");
  //gPad->SetLeftMargin(0.12);
  //h->SetTitleOffset(.95,"Y");
  //h->GetYaxis()->SetTitle("Fraction of hits");
  //h->GetXaxis()->SetTitle("Chamber hits timing vs L1A [BX]");
  h->SetMarkerSize(1.7);  // for TEXT bin values
  h->SetMarkerColor(4);   // for TEXT bin valued
  h->SetLineStyle(1);
  h->SetMarkerStyle(1);
  h->SetStats(false);

  // get statistics for full range
  meanX = h->GetMean();
  rmsY = h->GetRMS();
  h->GetXaxis()->SetRangeUser(-0.11, 0.11);

  h->GetXaxis()->SetNdivisions(410);
  h->SetFillColor(4); h->SetFillStyle(3003);
  h->DrawCopy("E1");
  h->DrawCopy("HIST,SAME"); 
  gPad->Update();

  // header
  //t = new TLatex(0.3,0.94, "RPC LB Synchronization ");
  //t->SetNDC(true);t->DrawClone();
  //t = new TLatex(.45,.95,"CMS Preliminary    #sqrt{s} = 8TeV");
  //t->SetNDC(true);t->SetTextSize(0.032); t->DrawClone();

  xpos=0.20;
  ypos=0.85;
  ioff=0;
  if(geolocation!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, geolocation); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  // run range
  if(runrange!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, runrange); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(datasettype!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, datasettype); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  // statistics
  // NOTE: Using SetRangeUser(), SetRange() causes ROOT
  // recompute MEAN/RMS using bin center values in the
  // viewable range! In order to get true MEAN/RMS values
  // one has to do GetXaxis()->UnZoom() after drawing the
  // canvas and before calling GetMean(), GetRms().
  // h->GetXaxis()->UnZoom();
  sprintf(label,"Avg time = %6.4f BX",meanX);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(2); t->DrawClone();

  // LB count
  t = new TLatex(xpos, ypos-ioff*legendOffset, "Number of Link Boards:"); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - total",nLB_total);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - w/o hits",nLB_dead);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();
  if(nLB_blacklisted>0) {
    sprintf(label,"%d - bad, excluded",nLB_blacklisted);
    t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();
  }
  
  //h->Write();
  c1->Write();
  c1->Print( outfile + "_" + h->GetName() + ".png", "png" );
  c1->Print( outfile + "_" + h->GetName() + ".C", "cxx" );
  c1->Print( outfile + "_" + h->GetName() + ".pdf", "pdf" );
  c1->Print( outfile + "_" + h->GetName() + ".eps", "eps");

  ////////////////////////////////////////////////////////////////


  h=hLB_Rms;
  // Draw: Distribution of LB rms BX
  //  setTDRStyle();
  tdrStyle->SetOptStat(0);  
  tdrStyle->SetOptLogy(1);
  tdrStyle->SetOptTitle(0);//off title
  tdrStyle->SetPadGridY(1);
  tdrStyle->SetPadRightMargin(0.02);
  //tdrStyle->SetPaintTextFormat("5.5f");  // for TEXT bin values
  tdrStyle->SetPalette(1);
  tdrStyle->SetOptStat(000111100);
  tdrStyle->SetStatY(0.9);                
  tdrStyle->SetStatX(0.9);                
  tdrStyle->SetStatW(0.2);                
  tdrStyle->SetStatH(0.05);                
  gROOT->ForceStyle();
  c1 = new TCanvas( outfile + "_" + h->GetName(),
		    h->GetTitle(),
		    0,0,600,600);
  c1->Draw();
  gPad->SetLeftMargin(0.17);
  gPad->SetBottomMargin(0.15);
  h->SetTitleSize(0.045,"Y");
  h->SetTitleSize(0.045,"X");
  h->SetTitleOffset(1.5,"Y");
  h->SetTitleOffset(1.2,"X");
  //gPad->SetLeftMargin(0.12);
  //h->SetTitleOffset(.95,"Y");
  //h->GetYaxis()->SetTitle("Fraction of hits");
  //h->GetXaxis()->SetTitle("Chamber hits timing vs L1A [BX]");
  h->SetMarkerSize(1.7);  // for TEXT bin values
  h->SetMarkerColor(4);   // for TEXT bin valued
  h->SetLineStyle(1);
  h->SetMarkerStyle(1);
  h->SetStats(false);

  // get statistics for full range
  h->GetXaxis()->UnZoom();
  meanX = h->GetMean();
  rmsX = h->GetRMS();
  h->GetXaxis()->SetRangeUser(0.0, 0.451);

  h->GetXaxis()->SetNdivisions(410);
  h->SetFillColor(4); h->SetFillStyle(3003);
  h->DrawCopy("E1");
  h->DrawCopy("HIST,SAME");
  gPad->Update();

  // header
  //t = new TLatex(0.3,0.94, "RPC LB Synchronization ");
  //t->SetNDC(true);t->DrawClone();
  //t = new TLatex(.45,.95,"CMS Preliminary    #sqrt{s} = 8TeV");
  //t->SetNDC(true);t->SetTextSize(0.032); t->DrawClone();

  // run range
  xpos=0.55;
  ypos=0.85;
  ioff=0;
  if(geolocation!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, geolocation); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(runrange!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, runrange); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(datasettype!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, datasettype); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  // statistics
  // NOTE: Using SetRangeUser(), SetRange() causes ROOT
  // recompute MEAN/RMS using bin center values in the
  // viewable range! In order to get true MEAN/RMS values
  // one has to do GetXaxis()->UnZoom() after drawing the
  // canvas and before calling GetMean(), GetRms().
  // h->GetXaxis()->UnZoom();
  sprintf(label,"Avg spread = %6.4f BX",meanX);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(2); t->DrawClone();

  // LB count
  t = new TLatex(xpos, ypos-ioff*legendOffset, "Number of Link Boards:"); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - total",nLB_total);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - w/o hits",nLB_dead);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  if(nLB_blacklisted>0) {
    sprintf(label,"%d - bad, excluded",nLB_blacklisted);
    t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();
  }
    
  //h->Write();
  c1->Write();
  c1->Print( outfile + "_" + h->GetName() + ".png", "png" );
  c1->Print( outfile + "_" + h->GetName() + ".C", "cxx" );
  c1->Print( outfile + "_" + h->GetName() + ".pdf", "pdf" );
  c1->Print( outfile + "_" + h->GetName() + ".eps", "eps");

  ////////////////////////////////////////////////////////////////

  h2=hLB_Rms_V_Mean;
  // Draw: 2D distribution of LB rms vs LB timing
  //  setTDRStyle();
  tdrStyle->SetOptStat(0);  
  tdrStyle->SetOptLogx(0);
  tdrStyle->SetOptLogy(0);
  tdrStyle->SetOptLogz(1);
  tdrStyle->SetOptTitle(0);//off title
  tdrStyle->SetPadGridX(1);
  tdrStyle->SetPadGridY(1);
  tdrStyle->SetPadRightMargin(0.2);
  //tdrStyle->SetPaintTextFormat("5.5f");  // for TEXT bin values
  tdrStyle->SetPalette(1);
  tdrStyle->SetOptStat(000111100);
  tdrStyle->SetStatY(0.9);                
  tdrStyle->SetStatX(0.9);                
  tdrStyle->SetStatW(0.2);                
  tdrStyle->SetStatH(0.05);     
  //tdrStyle->SetLabelOffset(1.0,"X");
  gROOT->ForceStyle();
  c1 = new TCanvas( outfile + "_" + h2->GetName(),
		    h2->GetTitle(),
		    0,0,600,600);
  c1->Draw();
  gPad->SetLeftMargin(0.17);
  gPad->SetBottomMargin(0.15);
  h2->SetTitleSize(0.045,"Y");
  h2->SetTitleSize(0.045,"X");
  h2->SetTitleSize(0.045,"Z");
  h2->SetTitleOffset(1.5,"Y");
  h2->SetTitleOffset(1.2,"X");
  h2->SetTitleOffset(1.2,"Z");
  h2->SetMaximum(nLB_total);
  h2->GetXaxis()->CenterTitle(true);
  //gPad->SetLeftMargin(0.12);
  //h2->SetTitleOffset(.95,"Y");
  //h2->GetYaxis()->SetTitle("Fraction of hits");
  //h2->GetXaxis()->SetTitle("Chamber hits timing vs L1A [BX]");
  h2->SetMarkerSize(1.0);  // for TEXT bin values
  h2->SetMarkerColor(1);   // for TEXT bin valued
  h2->SetLineStyle(1);
  h2->SetMarkerStyle(1);
  h2->SetStats(false);
  
  // get statistics for full range
  h2->GetXaxis()->UnZoom();
  h2->GetYaxis()->UnZoom();
  meanX = h2->GetMean(1);
  meanY = h2->GetMean(2);
  h2->GetXaxis()->SetRangeUser(-0.11, 0.11);
  h2->GetYaxis()->SetRangeUser(0.0, 0.45);

  h2->GetXaxis()->SetNdivisions(410);
  h2->GetYaxis()->SetNdivisions(410);
  h2->Draw("COLZ");
  h2->Draw("TEXT0,SAME");
  gPad->Update();
  c1->Modified();
  c1->Update();

  // header
  //t = new TLatex(0.3,0.94, "RPC LB Synchronization ");
  //t->SetNDC(true);t->DrawClone();
  //t = new TLatex(.45,.95,"CMS Preliminary    #sqrt{s} = 8TeV");
  //t->SetNDC(true);t->SetTextSize(0.032); t->DrawClone();

  //xpos=0.50;
  //ypos=0.40;
  xpos=0.20;
  ypos=0.85;
  ioff=0;
  if(geolocation!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, geolocation); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  // run range
  if(runrange!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, runrange); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(datasettype!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, datasettype); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  // statistics
  // NOTE: Using SetRangeUser(), SetRange() causes ROOT
  // recompute MEAN/RMS using bin center values in the
  // viewable range! In order to get true MEAN/RMS values
  // one has to do GetXaxis()->UnZoom() after drawing the
  // canvas and before calling GetMean(), GetRms().
  // h2->GetXaxis()->UnZoom();
  // h2->GetYaxis()->UnZoom();
  sprintf(label,"Avg time = %6.4f BX",meanX);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(2); t->DrawClone();

  sprintf(label,"Avg spread = %6.4f BX",meanY);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(2); t->DrawClone();

  // LB count
  t = new TLatex(xpos,ypos-ioff*legendOffset, "Number of Link Boards:"); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - total",nLB_total);
  t = new TLatex(xpos,ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - w/o hits",nLB_dead);
  t = new TLatex(xpos,ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  if(nLB_blacklisted>0) {
    sprintf(label,"%d - bad, excluded",nLB_blacklisted);
    t = new TLatex(xpos,ypos-ioff*legendOffset, label); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();
  }

  //h2->Write();
  c1->Write();
  c1->Print( outfile + "_" + h2->GetName() + ".png", "png" );
  c1->Print( outfile + "_" + h2->GetName() + ".C", "cxx" );
  c1->Print( outfile + "_" + h2->GetName() + ".pdf", "pdf" );
  c1->Print( outfile + "_" + h2->GetName() + ".eps", "eps");

  ////////////////////////////////////////////////////////////////

  for(int ifrac=0; ifrac<frac.size(); ifrac++) {

    h=hLB_win[ifrac];
    // Draw: Number of Link Boards: vs BX window size
    //    setTDRStyle();
    tdrStyle->SetOptStat(0);  
    tdrStyle->SetOptLogy(0);
    tdrStyle->SetOptTitle(0);//off title
    tdrStyle->SetPadGridY(1);
    tdrStyle->SetPadRightMargin(0.02);
    //  tdrStyle->SetPaintTextFormat(".0d");  // for TEXT bin values
    tdrStyle->SetPalette(1);
    gROOT->ForceStyle();
    c1 = new TCanvas( outfile + "_" + h->GetName(),
		      h->GetTitle(),
		      0,0,600,600);
    c1->Draw();
    gPad->SetLeftMargin(0.17);
    gPad->SetBottomMargin(0.15);
    h->SetTitleSize(0.045,"Y");
    h->SetTitleSize(0.045,"X");
    h->SetTitleOffset(1.5,"Y");
    h->SetTitleOffset(1.2,"X");
    //h->GetYaxis()->SetTitle("Fraction of hits");
    //h->GetXaxis()->SetTitle("Chamber hits timing vs L1A [BX]");
    h->SetMarkerSize(1.7);  // for TEXT bin values
    h->SetMarkerColor(4);   // for TEXT bin valued
    h->GetXaxis()->SetLabelSize(0.06);
    h->SetStats(false);
    h->SetFillColor(4); h->SetFillStyle(3003);
    h->DrawCopy("HIST,TEXT0");
    gPad->Update();

    // header
    //t = new TLatex(0.3,0.94, "RPC LB Synchronization ");
    //t->SetNDC(true);t->DrawClone();
    //t = new TLatex(.45,.95,"CMS Preliminary    #sqrt{s} = 8TeV");
    //t->SetNDC(true);t->SetTextSize(0.032); t->DrawClone();
    
    //xpos=0.65;
    //ypos=0.5;
    xpos=0.55;
    ypos=0.35;
    ioff=0;
    // run range
    if(geolocation!="") {
      t = new TLatex(xpos, ypos-ioff*legendOffset, geolocation); ioff++;
      t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
    }
    if(runrange!="") {
      t = new TLatex(xpos, ypos-ioff*legendOffset, runrange); ioff++;
      t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
    }
    if(datasettype!="") {
      t = new TLatex(xpos, ypos-ioff*legendOffset, datasettype); ioff++;
      t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
    }
    // statistics
    // NOTE: Using SetRangeUser(), SetRange() causes ROOT
    // recompute MEAN/RMS using bin center values in the
    // viewable range! In order to get true MEAN/RMS values
    // one has to do GetXaxis()->UnZoom() after drawing the
    // canvas and before calling GetMean(), GetRms().
    //h->GetXaxis()->UnZoom();

    // LB count
    t = new TLatex(xpos, ypos-ioff*legendOffset, "Number of Link Boards:"); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();
    
    sprintf(label,"%d - total",nLB_total);
    t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();
    
    sprintf(label,"%d - w/o hits",nLB_dead);
    t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();
    
    if(nLB_blacklisted>0) {
      sprintf(label,"%d - bad, excluded",nLB_blacklisted);
      t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
      t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();
    }
    
    //h->Write();
    c1->Write();
    c1->Print( outfile + "_" + h->GetName() + ".png", "png" );
    c1->Print( outfile + "_" + h->GetName() + ".C", "cxx" );
    c1->Print( outfile + "_" + h->GetName() + ".pdf", "pdf" );
    c1->Print( outfile + "_" + h->GetName() + ".eps", "eps");

  } // end of threshold loop

  ////////////////////////////////////////////////////////////////

  p=pLB_Mean_V_Sector;
  // Draw: Distribution of LB timing vs Sector
  // setTDRStyle();
  tdrStyle->SetOptStat(0);  
  tdrStyle->SetOptLogy(0);
  tdrStyle->SetOptTitle(0);//off title
  tdrStyle->SetPadGridY(1);
  tdrStyle->SetPadRightMargin(0.02);
  //tdrStyle->SetPaintTextFormat("5.5f");  // for TEXT bin values
  tdrStyle->SetPalette(1);
  tdrStyle->SetOptStat(000111100);
  tdrStyle->SetStatY(0.9);                
  tdrStyle->SetStatX(0.9);                
  tdrStyle->SetStatW(0.2);                
  tdrStyle->SetStatH(0.05);                
  gROOT->ForceStyle();
  c1 = new TCanvas( outfile + "_" + p->GetName(),
		    p->GetTitle(),
		    0,0,600,600);
  c1->Draw();
  gPad->SetLeftMargin(0.17);
  gPad->SetBottomMargin(0.15);
  p->SetTitleSize(0.045,"Y");
  p->SetTitleSize(0.045,"X");
  p->SetTitleOffset(1.8,"Y");
  p->SetTitleOffset(1.2,"X");
  //gPad->SetLeftMargin(0.12);
  //p->SetTitleOffset(.95,"Y");
  //p->GetYaxis()->SetTitle("Fraction of hits");
  //p->GetXaxis()->SetTitle("Chamber hits timing vs L1A [BX]");
  p->SetMarkerSize(1.5);  // for TEXT bin values
  p->SetMarkerColor(4);   // for TEXT bin valued
  p->SetLineStyle(1);
  p->SetMarkerStyle(20);
  p->SetStats(false);
  p->SetFillStyle(0);
  rangemax=p->GetBinContent(p->GetMaximumBin())+p->GetBinError(p->GetMaximumBin());
  rangemin=p->GetBinContent(p->GetMinimumBin())-p->GetBinError(p->GetMinimumBin());
  
  // get statistics for full region
  p->GetYaxis()->UnZoom();
  meanY = p->GetMean(2);  
  p->GetYaxis()->SetRangeUser(rangemin-0.4*(rangemax-rangemin), rangemax);

  p->GetYaxis()->SetNdivisions(410);
  p->GetXaxis()->SetNdivisions(-12);
  p->GetXaxis()->SetLabelSize(0.06);
  p->LabelsOption("h","X");
  //p->SetFillColor(4); p->SetFillStyle(3003);
  p->DrawCopy("E1 P");
  p->DrawCopy("E0 P SAME");
  //p->DrawCopy("HIST,SAME");
  gPad->Update();

  // header
  //t = new TLatex(0.3,0.94, "RPC LB Synchronization ");
  //t->SetNDC(true);t->DrawClone();
  //t = new TLatex(.45,.95,"CMS Preliminary    #sqrt{s} = 8TeV");
  //t->SetNDC(true);t->SetTextSize(0.032); t->DrawClone();

  // run range
  xpos=0.55;
  ypos=0.35;
  ioff=0;
  if(geolocation!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, geolocation); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(runrange!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, runrange); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(datasettype!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, datasettype); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  // statistics
  // NOTE: Using SetRangeUser(), SetRange() causes ROOT
  // recompute MEAN/RMS using bin center values in the
  // viewable range! In order to get true MEAN/RMS values
  // one has to do GetXaxis()->UnZoom() after drawing the
  // canvas and before calling GetMean(), GetRms().
  sprintf(label,"Avg time = %6.4f BX",meanY);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(2); t->DrawClone();
  l = new TLine(0, meanY, 12, meanY);
  l->SetLineColor(2); l->SetLineStyle(9); l->SetLineWidth(2); l->DrawClone();

  // LB count
  t = new TLatex(xpos, ypos-ioff*legendOffset, "Number of Link Boards:"); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - total", nLB_total);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - w/o hits",nLB_dead);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  if(nLB_blacklisted>0) {
    sprintf(label,"%d - bad, excluded",nLB_blacklisted);
    t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();
  }
    
  //p->Write();
  c1->Write();
  c1->Print( outfile + "_" + p->GetName() + ".png", "png" );
  c1->Print( outfile + "_" + p->GetName() + ".C", "cxx" );
  c1->Print( outfile + "_" + p->GetName() + ".pdf", "pdf" );
  c1->Print( outfile + "_" + p->GetName() + ".eps", "eps");

  ////////////////////////////////////////////////////////////////

  h=hLB_Evts_V_Sector;
  // Draw: Distribution of hit numbers vs Sector
  // setTDRStyle();
  tdrStyle->SetOptStat(0);  
  tdrStyle->SetOptLogy(0);
  tdrStyle->SetOptTitle(0);//off title
  tdrStyle->SetPadGridY(1);
  tdrStyle->SetPadRightMargin(0.02);
  //tdrStyle->SetPaintTextFormat("5.5f");  // for TEXT bin values
  tdrStyle->SetPalette(1);
  tdrStyle->SetOptStat(000111100);
  tdrStyle->SetStatY(0.9);                
  tdrStyle->SetStatX(0.9);                
  tdrStyle->SetStatW(0.2);                
  tdrStyle->SetStatH(0.05);                
  gROOT->ForceStyle();
  c1 = new TCanvas( outfile + "_" + h->GetName(),
		    h->GetTitle(),
		    0,0,600,600);
  c1->Draw();
  gPad->SetLeftMargin(0.17);
  gPad->SetBottomMargin(0.15);
  h->SetTitleSize(0.045,"Y");
  h->SetTitleSize(0.045,"X");
  h->SetTitleOffset(1.8,"Y");
  h->SetTitleOffset(1.2,"X");
  //gPad->SetLeftMargin(0.12);
  //h->SetTitleOffset(.95,"Y");
  //h->GetYaxis()->SetTitle("Fraction of hits");
  //h->GetXaxis()->SetTitle("Chamber hits timing vs L1A [BX]");
  h->SetMarkerSize(1.5);  // for TEXT bin values
  h->SetMarkerColor(4);   // for TEXT bin valued
  h->SetLineStyle(1);
  h->SetMarkerStyle(20);
  h->SetStats(false);
  h->SetFillStyle(0);
  h->GetXaxis()->SetNdivisions(-12);
  h->GetXaxis()->SetLabelSize(0.06);
  h->LabelsOption("h","X");
  rangemax=h->GetBinContent(h->GetMaximumBin())+h->GetBinError(h->GetMaximumBin());
  rangemin=h->GetBinContent(h->GetMinimumBin())-h->GetBinError(h->GetMinimumBin());

  // get statistics for full range
  h->GetYaxis()->UnZoom();
  meanY = h->GetMean(2);
  h->GetYaxis()->SetRangeUser(rangemin-0.4*(rangemax-rangemin), rangemax);

  //h->SetMinimum( 0.5*h->GetMaximum() );
  //h->SetFillColor(4); h->SetFillStyle(3003);
  h->DrawCopy("E1 P");
  h->DrawCopy("E0 P SAME");
  //h->DrawCopy("HIST,SAME");
  gPad->Update();

  // header
  //t = new TLatex(0.3,0.94, "RPC LB Synchronization ");
  //t->SetNDC(true);t->DrawClone();
  //t = new TLatex(.45,.95,"CMS Preliminary    #sqrt{s} = 8TeV");
  //t->SetNDC(true);t->SetTextSize(0.032); t->DrawClone();

  // run range
  xpos=0.55;
  ypos=0.35;
  ioff=0;
  if(geolocation!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, geolocation); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(runrange!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, runrange); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(datasettype!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, datasettype); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }

  // LB count
  t = new TLatex(xpos, ypos-ioff*legendOffset, "Number of Link Boards:"); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - total", nLB_total);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - w/o hits",nLB_dead);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  if(nLB_blacklisted>0) {
    sprintf(label,"%d - bad, excluded",nLB_blacklisted);
    t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();
  }
    
  //h->Write();
  c1->Write();
  c1->Print( outfile + "_" + h->GetName() + ".png", "png" );
  c1->Print( outfile + "_" + h->GetName() + ".C", "cxx" );
  c1->Print( outfile + "_" + h->GetName() + ".pdf", "pdf" );
  c1->Print( outfile + "_" + h->GetName() + ".eps", "eps");

  ////////////////////////////////////////////////////////////////

  p=pLB_Bx_V_Sector;
  // Draw: Distribution of hit timing vs Sector
  // setTDRStyle();
  tdrStyle->SetOptStat(0);  
  tdrStyle->SetOptLogy(0);
  tdrStyle->SetOptTitle(0);//off title
  tdrStyle->SetPadGridY(1);
  tdrStyle->SetPadRightMargin(0.02);
  //tdrStyle->SetPaintTextFormat("5.5f");  // for TEXT bin values
  tdrStyle->SetPalette(1);
  tdrStyle->SetOptStat(000111100);
  tdrStyle->SetStatY(0.9);                
  tdrStyle->SetStatX(0.9);                
  tdrStyle->SetStatW(0.2);                
  tdrStyle->SetStatH(0.05);                
  gROOT->ForceStyle();
  c1 = new TCanvas( outfile + "_" + p->GetName(),
		    p->GetTitle(),
		    0,0,600,600);
  c1->Draw();
  gPad->SetLeftMargin(0.17);
  gPad->SetBottomMargin(0.15);
  p->SetTitleSize(0.045,"Y");
  p->SetTitleSize(0.045,"X");
  p->SetTitleOffset(1.8,"Y");
  p->SetTitleOffset(1.2,"X");
  //gPad->SetLeftMargin(0.12);
  //p->SetTitleOffset(.95,"Y");
  //p->GetYaxis()->SetTitle("Fraction of hits");
  //p->GetXaxis()->SetTitle("Chamber hits timing vs L1A [BX]");
  p->SetMarkerSize(1.5);  // for TEXT bin values
  p->SetMarkerColor(4);   // for TEXT bin valued
  p->SetLineStyle(1);
  p->SetMarkerStyle(20);
  p->SetStats(false);
  p->SetFillStyle(0);
  rangemax=p->GetBinContent(p->GetMaximumBin())+p->GetBinError(p->GetMaximumBin());
  rangemin=p->GetBinContent(p->GetMinimumBin())-p->GetBinError(p->GetMinimumBin()); 

  // get statistics for full range
  p->GetYaxis()->UnZoom();
  meanY = p->GetMean(2);
  p->GetYaxis()->SetRangeUser(rangemin-0.4*(rangemax-rangemin), rangemax);

  p->GetYaxis()->SetNdivisions(410);
  p->GetXaxis()->SetNdivisions(-12);
  p->GetXaxis()->SetLabelSize(0.06);
  p->LabelsOption("h","X");
  //p->SetFillColor(4); p->SetFillStyle(3003);
  p->DrawCopy("E1 P");
  p->DrawCopy("E0 P SAME");
  //p->DrawCopy("HIST,SAME");
  gPad->Update();

  // header
  //t = new TLatex(0.3,0.94, "RPC LB Synchronization ");
  //t->SetNDC(true);t->DrawClone();
  //t = new TLatex(.45,.95,"CMS Preliminary    #sqrt{s} = 8TeV");
  //t->SetNDC(true);t->SetTextSize(0.032); t->DrawClone();

  // run range
  xpos=0.55;
  ypos=0.35;
  ioff=0;
  if(geolocation!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, geolocation); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(runrange!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, runrange); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(datasettype!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, datasettype); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  // statistics
  // NOTE: Using SetRangeUser(), SetRange() causes ROOT
  // recompute MEAN/RMS using bin center values in the
  // viewable range! In order to get true MEAN/RMS values
  // one has to do GetXaxis()->UnZoom() after drawing the
  // canvas and before calling GetMean(), GetRms().
  // p->GetYaxis()->UnZoom();
  sprintf(label,"Avg time = %6.4f BX",meanY);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(2); t->DrawClone();
  l = new TLine(0, meanY, 12, meanY);
  l->SetLineColor(2); l->SetLineStyle(9); l->SetLineWidth(2); l->DrawClone();

  // LB count
  t = new TLatex(xpos, ypos-ioff*legendOffset, "Number of Link Boards:"); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - total", nLB_total);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  sprintf(label,"%d - w/o hits",nLB_dead);
  t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
  t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();

  if(nLB_blacklisted>0) {
    sprintf(label,"%d - bad, excluded",nLB_blacklisted);
    t = new TLatex(xpos, ypos-ioff*legendOffset, label); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(4); t->DrawClone();
  }
    
  //p->Write();
  c1->Write();
  c1->Print( outfile + "_" + p->GetName() + ".png", "png" );
  c1->Print( outfile + "_" + p->GetName() + ".C", "cxx" );
  c1->Print( outfile + "_" + p->GetName() + ".pdf", "pdf" );
  c1->Print( outfile + "_" + p->GetName() + ".eps", "eps");

  ////////////////////////////////////////////////////////////////

  f.Close();
}

//=================================================

// version w/o blacklisted LBs
void synchro(TString datafile,
	     TString outfile="",
	     TString runrange="",
	     TString datasettype="",
	     TString geolocation="") {
  vector<TString> blacklist;
  synchro(datafile, blacklist, outfile, runrange, datasettype, geolocation); 
}


//=================================================
void synchro_history(TString h_name, TString p_name = "",Int_t log=1)
{
  if (p_name=="") p_name = h_name ;

  //gROOT->Reset();
  setTDRStyle();
  gROOT->ForceStyle();
  TStyle *tdrStyle=(TStyle*)(gROOT->FindObject("tdrStyle"));

  tdrStyle->SetOptStat(0);
  tdrStyle->SetOptLogy(log);
  tdrStyle->SetOptTitle(0);//off title
  tdrStyle->SetPadGridY(1);
  tdrStyle->SetPadRightMargin(0.02);
  tdrStyle->SetPalette(1);

  TFile *f1 = new TFile("/afs/cern.ch/cms/L1/rpc/plots/marcin/LBTimingSettings/all/2010_04_22_delays_2/merge.root");  
  TH1F * h1= (TH1F*)f1->Get(h_name);
  TFile *f2 = new TFile("/afs/cern.ch/cms/L1/rpc/plots/marcin/LBTimingSettings/all/2010_05_12_delays/merge.root");    
  TH1F * h2= (TH1F*)f2->Get(h_name);
  //TFile *f3 = new TFile("/afs/cern.ch/user/c/cwiok/scratch0/eff_synchro.06072010/results/3synch_ExpressPhys_noHLTFilter.run138560-138751/merge.root");    
  TFile *f3 = new TFile("/afs/cern.ch/user/c/cwiok/scratch0/eff_synchro.06072010/results/3synch_ExpressPhys_noHLTFilter.run138560-139458/merge.root");    
  TH1F * h3= (TH1F*)f3->Get(h_name);
  //TFile *f4 = new TFile("/afs/cern.ch/user/c/cwiok/scratch0/eff_synchro.06072010/results/3synch_ExpressPhys_noHLTFilter.run138919-139458/merge.root");    
  //TH1F * h4= (TH1F*)f4->Get(h_name);

  TCanvas *c1 = new TCanvas("c1","",0,0,600,600);
  c1->Draw();
  c1->cd(1);
  gPad->SetLeftMargin(0.12);

  h1->SetLineColor(8);  h1->SetMarkerColor(8);  h1->SetFillColor(8);  h1->SetFillStyle(0);     h1->SetLineWidth(2);
  h2->SetLineColor(2);  h2->SetMarkerColor(2);  h2->SetFillColor(2);  h2->SetFillStyle(0);     h2->SetLineWidth(2);
  h3->SetLineColor(4);  h3->SetMarkerColor(4);  h3->SetFillColor(4);  h3->SetFillStyle(3003);  h3->SetLineWidth(2);
  //h3->SetLineStyle(2);
  //h4->SetLineColor(1);  h4->SetFillColor(1);  h4->SetLineWidth(2); 
  //h4->SetLineStyle(1);  h4->SetFillStyle(3003); 

  h1->Sumw2();
  h2->Sumw2();
  h3->Sumw2();
  Double_t scale1 = 1/h1->Integral();h1->Scale(scale1);
  Double_t scale2 = 1/h2->Integral();h2->Scale(scale2);
  Double_t scale3 = 1/h3->Integral();h3->Scale(scale3);
  //Double_t scale4 = 1/h4->Integral();h4->Scale(scale4);
 
  int i=4;
  float bxh1 = h1->GetBinContent(i)*100;
  float bxh2 = h2->GetBinContent(i)*100;
  float bxh3 = h3->GetBinContent(i)*100;
  //float bxh4 = h4->GetBinContent(i)*100;

  char s[100];
  sprintf(s, "%5.2f", bxh1);  TString bx1(s);
  sprintf(s, "%5.2f", bxh2);  TString bx2(s);
  sprintf(s, "%5.2f", bxh3);  TString bx3(s);
  //sprintf(s, "%5.2f", bxh4);  TString bx4(s);

  cout<<" "<<bxh1<< " "<<bx1<<endl;
  cout<<" "<<bxh2<< " "<<bx2<<endl;
  cout<<" "<<bxh3<< " "<<bx3<<endl;
  //cout<<" "<<bxh4<< " "<<bx4<<endl;

  /////////////////////////////////////////
  // when plotting 2-3 histograms at once
  h1->SetTitleOffset(1.2,"x");
  h1->SetTitleOffset(1.5,"y");
  h1->GetYaxis()->SetTitle("Fraction of hits");
  h1->GetXaxis()->SetTitle("Chamber hits timing vs L1A [BX]");
  h1->GetYaxis()->SetTitleSize(0.04);
  h1->GetXaxis()->SetTitleSize(0.04);

  h1->SetMaximum(1.5);
  h1->SetMinimum(5.e-6);
  h1->SetAxisRange(-3.5,3);

  h1->DrawCopy("E1 HIST");
  h2->DrawCopy("E1 HIST SAME");
  h3->DrawCopy("E1 HIST SAME");
  //h4->DrawCopy("same");
/* 
  ///////////////////////////////////////////
  // when h3 is plotted alone
  h3->SetTitleOffset(1.2,"x");
  h3->SetTitleOffset(1.5,"y");
  h3->GetYaxis()->SetTitle("Fraction of hits");
  h3->GetXaxis()->SetTitle("Chamber hits timing vs L1A [BX]");
  h3->GetYaxis()->SetTitleSize(0.04);
  h3->GetXaxis()->SetTitleSize(0.04);

  h3->SetMaximum(1.5);
  h3->SetMinimum(5.e-6);
  h3->SetAxisRange(-3.5,3);
  h3->DrawCopy("E1 HIST");
  //////////////////////////////////////////
*/
  gPad->Update();

  TLatex *t = new TLatex(0.17,0.35,"CMS Preliminary 2012 @ 8TeV"); 
  t->SetNDC(true);t->SetTextSize(0.035);t->SetTextAngle(90); t->SetTextColor(11); t->DrawClone();

  t = new TLatex(0.3,0.94, "RPC Hit Synchronization ");
  t->SetNDC(true);t->DrawClone();

  t = new TLatex(0.47,0.78, "BX=0:");t->SetTextAngle(90);
  t->SetNDC(true);t->SetTextSize(0.035); t->SetTextColor(1); t->DrawClone();

  i=0;
  t = new TLatex(0.50,0.84-i*0.05, bx1+"%"); i++;
  t->SetNDC(true);t->SetTextSize(0.035); t->SetTextColor(8); t->DrawClone();
  t = new TLatex(0.50,0.84-i*0.05, bx2+"%"); i++;
  t->SetNDC(true);t->SetTextSize(0.035); t->SetTextColor(2); t->DrawClone();
  t = new TLatex(0.50,0.84-i*0.05, bx3+"%"); i++;
  t->SetNDC(true);t->SetTextSize(0.035); t->SetTextColor(4); t->DrawClone();
  //t = new TLatex(0.50,0.84-i*0.05, bx4+"%"); i++;
  //t->SetNDC(true);t->SetTextSize(0.035); t->SetTextColor(1); t->DrawClone();


  t = new TLatex(0.23,0.78, "Date:");t->SetTextAngle(90);
  t->SetNDC(true);t->SetTextSize(0.035); t->SetTextColor(1); t->DrawClone();

  i=0;
  t = new TLatex(0.24,0.84-i*0.05, "2010.04.22"); i++;
  t->SetNDC(true);t->SetTextSize(0.025); t->SetTextColor(8); t->DrawClone();
  t = new TLatex(0.24,0.84-i*0.05, "2010.05.12"); i++;
  t->SetNDC(true);t->SetTextSize(0.025); t->SetTextColor(2); t->DrawClone();
  //t = new TLatex(0.24,0.84-i*0.05, "2010.07.01"); i++;
  t = new TLatex(0.24,0.84-i*0.05, "2010.07.06"); i++;// up to 139458 on 6 Jul 2010
  t->SetNDC(true);t->SetTextSize(0.025); t->SetTextColor(4); t->DrawClone();
  //t = new TLatex(0.24,0.84-i*0.05, "2010.07.04"); i++;
  //t->SetNDC(true);t->SetTextSize(0.025); t->SetTextColor(1); t->DrawClone();


  t = new TLatex(0.73,0.78, "Runs:");t->SetTextAngle(90);
  t->SetNDC(true);t->SetTextSize(0.035); t->SetTextColor(1); t->DrawClone();

  i=0;
  t = new TLatex(0.74,0.84-i*0.05, "132440-133483"); i++;
  t->SetNDC(true);t->SetTextSize(0.035); t->SetTextColor(8); t->DrawClone();
  t = new TLatex(0.74,0.84-i*0.05, "133876-134472"); i++;
  t->SetNDC(true);t->SetTextSize(0.035); t->SetTextColor(2); t->DrawClone();
  //t = new TLatex(0.74,0.84-i*0.05, "138560-138751"); i++;
  t = new TLatex(0.74,0.84-i*0.05, "138560-139458"); i++;
  t->SetNDC(true);t->SetTextSize(0.035); t->SetTextColor(4); t->DrawClone();
  //t = new TLatex(0.74,0.84-i*0.05, "138919-139458"); i++;
  //t->SetNDC(true);t->SetTextSize(0.035); t->SetTextColor(1); t->DrawClone();

  c1->Print( p_name + ".png", "png" );
  c1->Print( p_name + ".C", "cxx" );
  c1->Print( p_name + ".pdf", "pdf" );
  c1->Print( p_name + ".eps", "eps");

  //  c1->Print(p_name+".png");
  //c1->Print(p_name+".eps");
  //  c1->Close();
  c1->Draw();
}

//
// plots BX distribution for top 10 occupancy and top 10 spread LBs
//
void top10(TString datafile,
	   TString outfile="",    
	   TString runrange="",
	   TString datasettype="",
	   TString geolocation="") {

  const Float_t legendFontSize = 0.030;
  const Float_t legendOffset   = 0.030;
  const Int_t PEAK_BIN = 3;    // L1A position (BX=0) in the input text file

  //gROOT->Reset();
  setTDRStyle();
  TStyle *tdrStyle=(TStyle*)(gROOT->FindObject("tdrStyle"));
  tdrStyle->SetOptStat(0);  
  tdrStyle->SetOptLogy(0);
  tdrStyle->SetOptTitle(1);
  tdrStyle->SetPadGridY(0);
  tdrStyle->SetPadRightMargin(0.02);
  //tdrStyle->SetPaintTextFormat("5.0f");  // for TEXT bin values
  tdrStyle->SetPalette(1);
  gROOT->ForceStyle();

  if(outfile=="") outfile="top10";
  TFile f( (outfile+".root").Data(), "RECREATE");
  f.mkdir("Synchro");
  f.cd("Synchro");
  f.ls();

  cout << "Top10:"<<endl 
       << " Input ROOT file      = " << datafile << endl
       << " Output file prefix   = " << outfile
       << endl;

  TFile fin( datafile.Data(), "OLD");
  fin.cd();
  fin.ls();

  TH2D *h;
  TCanvas *c1;
  Float_t xpos,ypos;
  Int_t ioff;
  TLatex *t;

  ////////////////////////////////////////////////////////////////

  h=(TH2D*)(gROOT->FindObject("topOccup"));
  for(Int_t nbx=0; nbx<h->GetNbinsX(); nbx++) {
    char label[5];
    sprintf(label,"%d",nbx-PEAK_BIN);
    h->GetXaxis()->SetBinLabel(nbx+1,label);
  }
  h->SetTitle("Top 10 occupancy LBs");
  h->GetXaxis()->SetTitle("Chamber hit timing wrt L1 [BX]");
  h->GetXaxis()->SetRange(1,7);
  c1 = new TCanvas( outfile + "_" + h->GetName(),
		    h->GetTitle(), 600, 600);
  c1->Draw();
  gPad->SetLeftMargin(0.3);
  //gPad->SetBottomMargin(0.15);
  //h->SetTitleSize(0.045,"Y");
  //h->SetTitleSize(0.045,"X");
  //h->SetTitleOffset(1.8,"Y");
  //h->SetTitleOffset(1.2,"X");
  h->SetStats(0);
  h->SetFillStyle(0);
  h->SetMarkerSize(1.5);
  h->SetMarkerColor(4);
  //h->GetXaxis()->SetLabelSize(0.06);
  h->SetTitleSize(0.04,"X");
  h->SetTitleOffset(1.05,"X");
  h->LabelsOption("h","X");
  h->DrawCopy("TEXT0");
  gPad->Update();

  // header
  //t = new TLatex(.45,.95,"CMS Preliminary    #sqrt{s} = 8TeV");
  //t->SetNDC(true);t->SetTextSize(0.032); t->DrawClone();

  // run range
  xpos=0.32;
  ypos=0.20;
  ioff=0;
  if(geolocation!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, geolocation); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(runrange!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, runrange); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(datasettype!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, datasettype); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }

  f.cd("Synchro");
  //h->Write();
  c1->Write();
  c1->Print( outfile + "occup" + ".png", "png" );
  c1->Print( outfile + "occup" + ".C", "cxx" );
  c1->Print( outfile + "occup" + ".pdf", "pdf" );
  c1->Print( outfile + "occup" + ".eps", "eps");

  fin.cd();

  ////////////////////////////////////////////////////////////////

  h=(TH2D*)(gROOT->FindObject("topSpread"));
  for(Int_t nbx=0; nbx<h->GetNbinsX(); nbx++) {
    char label[5];
    sprintf(label,"%d",nbx-PEAK_BIN);
    h->GetXaxis()->SetBinLabel(nbx+1,label);
  }
  h->SetTitle("Top 10 spread LBs");
  h->GetXaxis()->SetTitle("Chamber hit timing wrt L1 [BX]");
  h->GetXaxis()->SetRange(1,7);
  c1 = new TCanvas( outfile + "_" + h->GetName(),
		    h->GetTitle(), 600, 600);
  c1->Draw();
  gPad->SetLeftMargin(0.3);
  //gPad->SetBottomMargin(0.15);
  //h->SetTitleSize(0.045,"Y");
  //h->SetTitleSize(0.045,"X");
  //h->SetTitleOffset(1.8,"Y");
  //h->SetTitleOffset(1.2,"X");
  h->SetStats(0);
  h->SetFillStyle(0);
  h->SetMarkerSize(1.5);
  h->SetMarkerColor(4);
  //h->GetXaxis()->SetLabelSize(0.06);
  h->SetTitleSize(0.04,"X");
  h->SetTitleOffset(1.05,"X");
  h->LabelsOption("h","X");
  h->DrawCopy("TEXT0");
  gPad->Update();

  // header
  //t = new TLatex(.45,.95,"CMS Preliminary    #sqrt{s} = 8TeV");
  //t->SetNDC(true);t->SetTextSize(0.032); t->DrawClone();

  // run range
  xpos=0.32;
  ypos=0.20;
  ioff=0;
  if(geolocation!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, geolocation); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(runrange!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, runrange); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }
  if(datasettype!="") {
    t = new TLatex(xpos, ypos-ioff*legendOffset, datasettype); ioff++;
    t->SetNDC(true);t->SetTextSize(legendFontSize); t->SetTextColor(1); t->DrawClone();
  }

  f.cd("Synchro");
  //h->Write();
  c1->Write();
  c1->Print( outfile + "spread" + ".png", "png" );
  c1->Print( outfile + "spread" + ".C", "cxx" );
  c1->Print( outfile + "spread" + ".pdf", "pdf" );
  c1->Print( outfile + "spread" + ".eps", "eps");

  fin.cd();

  ////////////////////////////////////////////////////////////////

  f.Close();
  fin.Close();

}

//========== MAIN ==============

void drawSynchro(const TString dir="./", 
	         const TString runrange="",
	         const TString dataset="",
	         const TString prefix="synchro") {

  gROOT->Reset();
  cout << dir << endl;
  cout << runrange << endl;
  cout << dataset << endl;
  cout << prefix << endl;
/*  
  // Superimposes several synchronizations on the same plot
  // Requires merge.root files produced by synchroMerger.py
  // NOTE: File names are hardcoded in the function body.
  synchro_history("delaySummary");
*/

  // Draws several statistics plots per Link Board.
  // Also produces normalized distribution of hit arrival times.
  // Requires delays.txt file (not merge.root) produced by synchroMerger.py

  // All LBs
  //synchro("/afs/cern.ch/user/c/cwiok/scratch0/submit/ExpressPhys_noHLTFilter/runno_139096-140180_ZBunpr_noMu/delays.txt",
  synchro(dir + 
          "/delays.txt",
	  prefix + "_Barrel+Endcap", 
	  runrange, // "139096-140180, no #mu",// "Data corresponds"
	  dataset,  // "ExpressPhysics",       // "  to XX nb^{-1}"
	  "Barrel+Endcap");

  /*
  // Excluded LBs from the top 10 BX spread list
  vector<TString> blacklist;
  blacklist.push_back("LB_RE-3_S3_EN33_CH0");
  blacklist.push_back("LB_RB+2_S8_BP2D_CH0");
  blacklist.push_back("LB_RB-2_S10_BN2A_CH0");
  blacklist.push_back("LB_RB0_S9_BP0E_CH0");
  blacklist.push_back("LB_RE+3_S3_EP32_CH2");
  blacklist.push_back("LB_RB+1_S6_BP1E_CH0");
  blacklist.push_back("LB_RB-2_S7_BN2B_CH1");
  blacklist.push_back("LB_RB-2_S10_BN2B_CH2");
  blacklist.push_back("LB_RB-2_S10_BN2E_CH0");
  blacklist.push_back("LB_RE-1_S11_EN22_CH1");

  //synchro("/afs/cern.ch/user/c/cwiok/scratch0/submit/ExpressPhys_noHLTFilter/runno_139096-140180_ZBunpr_noMu/delays.txt", 
  synchro(dir +
          "/delays.txt",
	  blacklist,
	  prefix + "_excl_Barrel+Endcap", 
	  runrange.Data(), // "139096-140180, no #mu",// "Data corresponds"
	  dataset.Data(),  // "ExpressPhysics",       // "  to XX nb^{-1}"
	  "Barrel+Endcap");
  */

  // BARREL & ENDCAP
  vector<TString> geolocation;
  geolocation.push_back("Barrel");
  geolocation.push_back("Endcap");
  geolocation.push_back("Endcap_Pos");
  geolocation.push_back("Endcap_Neg");
  for(Int_t igeo=0; igeo<geolocation.size(); igeo++) {
    //synchro("/afs/cern.ch/user/c/cwiok/scratch0/submit/ExpressPhys_noHLTFilter/runno_139096-140180_ZBunpr_noMu/"
    synchro(dir + 
	    "/delays_" + geolocation[igeo] + ".txt", 
	    prefix + "_" + geolocation[igeo], 
	    runrange, // "139096-140180, no #mu",// "Data corresponds"
	    dataset,  // "ExpressPhysics",       // "  to XX nb^{-1}"
	    geolocation[igeo]);
  } 

  // BARREL per WHEEL
  geolocation.resize(0);
  geolocation.push_back("W-2");
  geolocation.push_back("W-1");
  geolocation.push_back("W0");
  geolocation.push_back("W+1");
  geolocation.push_back("W+2");
  for(Int_t igeo=0; igeo<geolocation.size(); igeo++) {
    //synchro("/afs/cern.ch/user/c/cwiok/scratch0/submit/ExpressPhys_noHLTFilter/runno_139096-140180_ZBunpr_noMu/"
    synchro(dir +
	    "/delays_Barrel_" + geolocation[igeo] + ".txt", 
	    prefix + "_Barrel_" + geolocation[igeo], 
	    runrange, // "139096-140180, no #mu",// "Data corresponds"
	    dataset,  // "ExpressPhysics",       // "  to XX nb^{-1}"
	    "Wheel " + geolocation[igeo]);
  } 

  // ENDCAP per DISK
  geolocation.resize(0);
  geolocation.push_back("RE+1");
  geolocation.push_back("RE-1");
  geolocation.push_back("RE+2");
  geolocation.push_back("RE-2");
  geolocation.push_back("RE+3");
  geolocation.push_back("RE-3");
  //geolocation.push_back("RE+4");
  //geolocation.push_back("RE-4");
  for(Int_t igeo=0; igeo<geolocation.size(); igeo++) {
    //synchro("/afs/cern.ch/user/c/cwiok/scratch0/submit/ExpressPhys_noHLTFilter/runno_139096-140180_ZBunpr_noMu/"
    synchro(dir +
	    "/delays_Endcap_" + geolocation[igeo] + ".txt", 
	    prefix + "_Endcap_" + geolocation[igeo], 
	    runrange, // "139096-140180, no #mu",// "Data corresponds"
	    dataset,  // "ExpressPhysics",       // "  to XX nb^{-1}"
	    "Endcap disk " + geolocation[igeo]);
  } 

  // BARREL per STATION
  geolocation.resize(0);
  geolocation.push_back("RB1");
  geolocation.push_back("RB2");
  geolocation.push_back("RB3");
  geolocation.push_back("RB4");
  for(Int_t igeo=0; igeo<geolocation.size(); igeo++) {
    //synchro("/afs/cern.ch/user/c/cwiok/scratch0/submit/ExpressPhys_noHLTFilter/runno_139096-140180_ZBunpr_noMu/"
    synchro(dir +
	    "/delays_Barrel_" + geolocation[igeo] + ".txt", 
	    prefix + "_Barrel_" + geolocation[igeo], 
	    runrange, // "139096-140180, no #mu",// "Data corresponds"
	    dataset,  // "ExpressPhysics",       // "  to XX nb^{-1}"
	    "Barrel station " + geolocation[igeo]);
  } 

  // ENDCAP per STATTION
  geolocation.resize(0);
  geolocation.push_back("RE1");
  geolocation.push_back("RE2");
  geolocation.push_back("RE3");
  //geolocation.push_back("RE4");
  for(Int_t igeo=0; igeo<geolocation.size(); igeo++) {
    //synchro("/afs/cern.ch/user/c/cwiok/scratch0/submit/ExpressPhys_noHLTFilter/runno_139096-140180_ZBunpr_noMu/"
    synchro(dir +
	    "/delays_Endcap_" + geolocation[igeo] + ".txt", 
	    prefix + "_Endcap_" + geolocation[igeo], 
	    runrange, // "139096-140180, no #mu",// "Data corresponds"
	    dataset,  // "ExpressPhysics",       // "  to XX nb^{-1}"
	    "Endcap station " + geolocation[igeo]);
  } 

  top10(dir + 
	"/merge.root",
	prefix + "_Barrel+Endcap_top10",
	runrange,
	dataset,
	"Barrel+Endcap");

  return;
}
