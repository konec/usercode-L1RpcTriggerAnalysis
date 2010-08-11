#include "TROOT.h"
#include "TH1.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TArrayD.h"
#include "TFile.h"
#include "TString.h"
#include "TAxis.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <vector>
//#include <math.h>
#define minim(A,B) ((A)<(B)?(A):(B))


int doit(int argc, const char *argv[])
{
  if(argc!=5 && argc!=7) {
    std::cout << "Usage: " 
	      << argv[0] << "  <FIRST_root_file>  <SECOND_root_file>  <COMMON_HIST_name_file>  <COMMON_DIR_name>" << std::endl
	      << "or:" << std::endl
	      << argv[0] << "  <FIRST_root_file>  <SECOND_root_file>  <FIRST_HIST_name_file>  <SECOND_HIST_name_file>  <FIRST_DIR_name>  <SECOND_DIR_name> " << std::endl;

    exit(0);
  }
  std::cout << setiosflags(std::ios::fixed | std::ios::showpoint);

  gROOT->SetBatch(kTRUE);
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(0);

  TString file1, file2, dir1, dir2, fhist[2];
  if(argc==7) {
    file1 = argv[1];
    file2 = argv[2];
    fhist[0]=argv[3];
    fhist[1]=argv[4];
    dir1 = argv[5];
    dir2 = argv[6];
  } else {
    file1 = argv[1];
    file2 = argv[2];
    fhist[0]=argv[3];
    fhist[1]=argv[3];
    dir1 = argv[4];
    dir2 = argv[4];

    std::cout << file1 << std::endl 
	      << file2 << std::endl 
	      << fhist[0] << std::endl 
	      << fhist[1] << std::endl 
	      << dir1 << std::endl 
	      << dir2 << std::endl;  
  }
  
  fstream *fh = NULL;
  std::vector<TString> hnames[2];

  //____________________
  //READ HISTOGRAM NAMES
  for(int i=0; i<2; i++) {
    std::cout<<"Reading histogram names from file "<<fhist[i]<<"..."<<std::endl;  
    fh=new std::fstream(fhist[i],std::ios::in);
    if(!fh) {
      std::cout << "#### ERROR #### : Wrong file with histogram names: " << fhist[i] << std::endl;
      exit(0);
    }
    while(!fh->eof()) {
      TString name("");
      (*fh) >> name;
      if( name!="" && 
	  find(hnames[i].begin(), hnames[i].end(), name)==hnames[i].end() ) {
	hnames[i].push_back(name);
      }
    };
    fh->close();
    if (hnames[i].size()==0) {
      std::cout<< "#### ERROR #### : Can't read histogram names!" << std::endl;    
      exit(-1);
    }
    std::cout << "Read " << hnames[i].size() << " histogram names" << std::endl;
  }

  //__________________
  //COMPARE HISTOGRAMS
  TFile   *f0 = new TFile(file1, "old");
  TFile   *f1 = new TFile(file2, "old");

  if(!f0 || !f1 || !f0->IsOpen() || !f1->IsOpen() ) {
    std::cout<< "#### ERROR #### : Can't open files!" << std::endl;
    exit(-2);
  }

  const int nhist=minim(hnames[0].size(), hnames[1].size());
  //__________________________
  // LOOP THRU HISTOGRAM NAMES
  for(int i=0; i<nhist; i++) {

    bool err=false;

    TH1 *h0=NULL,    *h1=NULL;
    TCanvas *c0=NULL, *c1=NULL;
    TAxis *xaxis[2], *yaxis[2];
    int ndim[2];
    int index;

    index=0;
    //_______________________________________
    // CHECK IF THIS IS A HISTOGRAM OR CANVAS
    f0->cd(dir1);
    c0 = (TCanvas*)gROOT->FindObject(hnames[0][i].Data());
    //c0 = (TCanvas*)f0->Get(hnames[0][i].Data());
    if(c0) {
      //std::cout << "TCANVAS" << std::endl;
      TObjLink *lnk=c0->GetListOfPrimitives()->FirstLink();
      while (lnk) {
	if( (TString)lnk->GetObject()->ClassName()=="TH1F" ||
	    (TString)lnk->GetObject()->ClassName()=="TH2F" ||
	    (TString)lnk->GetObject()->ClassName()=="TH1D" ||
	    (TString)lnk->GetObject()->ClassName()=="TH2D" ||
	    (TString)lnk->GetObject()->ClassName()=="TProfile" ||
	    (TString)lnk->GetObject()->ClassName()=="TProfile2D" ) {
	  h0 = (TH1*)lnk->GetObject();
	  //std::cout << "name=" << ( h0 ? h0->GetName() : "NULL" ) << std::endl;
	  break;
	} else {
	  h0 = NULL;
	}
	lnk = lnk->Next();
      }
      //h0 = (TH1*)c0->GetPrimitive(hnames[0][i].Data());
      c0->Clear();
    } else {
      //std::cout << "TH1" << std::endl;
      h0 = (TH1*)gROOT->FindObject(hnames[0][i].Data());
      //h0 = (TH1*)f0->Get(hnames[0][i].Data());
    }
    //h0 = (TH1*)gROOT->FindObject(hnames[0][i].Data());
    if(!h0) {
      std::cout<<"#### ERROR #### : Can't find histogram named "<<hnames[0][i]<<" in file #1" << std::endl;
      err=true;
    }
    if(err) continue;
    ndim[index] = h0->GetDimension();
    xaxis[index]= h0->GetXaxis();
    if(ndim[index]==2) yaxis[index]=h0->GetYaxis();

    index=1;
    f1->cd(dir2);
    c1 = (TCanvas*)gROOT->FindObject(hnames[1][i].Data());
    //c1 = (TCanvas*)f1->Get(hnames[1][i].Data());
    if(c1) {
      //std::cout << "TCANVAS" << std::endl;
      //c1->ls();
      TObjLink *lnk=c1->GetListOfPrimitives()->FirstLink();
      while (lnk) {
	if( (TString)lnk->GetObject()->ClassName()=="TH1F" ||
	    (TString)lnk->GetObject()->ClassName()=="TH2F" ||
	    (TString)lnk->GetObject()->ClassName()=="TH1D" ||
	    (TString)lnk->GetObject()->ClassName()=="TH2D" ||
	    (TString)lnk->GetObject()->ClassName()=="TProfile" ||
	    (TString)lnk->GetObject()->ClassName()=="TProfile2D" ) {
	  h1 = (TH1*)lnk->GetObject();
	  //std::cout << "name=" << ( h1 ? h1->GetName() : "NULL" ) << std::endl;
	  break;
	} else {
	  h1 = NULL;
	}
	lnk = lnk->Next();
      }
      //h1 = (TH1*)c1->GetPrimitive(hnames[1][i].Data());
      c1->Clear();
    } else {
      //std::cout << "TH1" << std::endl;
      h1 = (TH1*)gROOT->FindObject(hnames[1][i].Data());
      //h1 = (TH1*)f1->Get(hnames[1][i].Data());
    }
    //h1 = (TH1*)gROOT->FindObject(hnames[1][i].Data());
    if(!h1) {
      std::cout<<"#### ERROR #### : Can't find histogram named "<<hnames[1][i]<<" in file #2" << std::endl;
      err=true;
    }
    if(err) continue;
    ndim[index] = h1->GetDimension();
    xaxis[index]= h1->GetXaxis();
    if(ndim[index]==2) yaxis[index]=h1->GetYaxis();

    //_________________________________
    // BIN BOUNDARIES CONSISTENCY CHECK
    
    std::cout << "------" << std::endl
	      << "Comparing: H1=" << dir1 << "/" << hnames[0][i] 
	      << "  \"" << h0->GetTitle() << "\"" << std::endl
	      << "        FILE1=" << file1 << std::endl
	      << "      with H2=" << dir2 << "/" << hnames[1][i] 
	      << "  \"" << h1->GetTitle() << "\"" << std::endl
	      << "        FILE2=" << file2 << std::endl;

    if( ndim[0] != ndim[1] ) {
      std::cout << std::endl 
		<< "#### ERROR #### : Different dimensions!" << std::endl;
      err=true;
    }
    if(err) continue;
    if( xaxis[0]->GetNbins() != xaxis[1]->GetNbins() ) {
      std::cout << std::endl 
		<< "#### ERROR #### : Different number of bins in X!  (" 
		<< xaxis[0]->GetNbins() << " and " << xaxis[1]->GetNbins() 
		<< ")" << std::endl;
      err=true;
    }
    if(err) continue;
    for(int ix=1; ix<=xaxis[0]->GetNbins(); ix++) {
      if(err) break;
      if( fabs(xaxis[0]->GetBinCenter(ix)-xaxis[1]->GetBinCenter(ix))>1e-5 ) {
	std::cout << std::endl 
		  << "#### ERROR #### : Different bin center in X!  (bin=" 
		  << ix << ", x1=" 
		  << xaxis[0]->GetBinCenter(ix) 
		  << ", x2="
		  << xaxis[1]->GetBinCenter(ix) << ")" << std::endl;
	err=true;
	break;
      }
    }
    if(err) continue;
    if( ndim[0]==2 ) {
      if ( yaxis[0]->GetNbins() != yaxis[1]->GetNbins() ) {
	std::cout << std::endl  
		  << "#### ERROR #### : Different number of bins in Y!" << std::endl;
        err=true;
      }
    }
    if(err) continue;
    if( ndim[0]==2 ) {
      for(int iy=1; iy<=yaxis[0]->GetNbins(); iy++) {
	if(err) break;
	if( fabs(yaxis[0]->GetBinCenter(iy)-yaxis[1]->GetBinCenter(iy))>1e-5 ) {
	  std::cout << std::endl 
		    << "#### ERROR #### : Different bin center in Y!  (bin=" 
		    << iy << ", y1=" 
		    << yaxis[0]->GetBinCenter(iy) 
		    << ", y2="
		    << yaxis[1]->GetBinCenter(iy) << ")" << std::endl;
	  err=true;
	  break;
	}
      }
    }
    if(err) continue;

    double integral1=h0->Integral();
    double integral2=h1->Integral();
    bool   is1_profile   = (dynamic_cast<TProfile *>(h0))!=NULL;
    bool   is1_profile2d = (dynamic_cast<TProfile2D *>(h0))!=NULL;
    bool   is2_profile   = (dynamic_cast<TProfile *>(h1))!=NULL;
    bool   is2_profile2d = (dynamic_cast<TProfile2D *>(h1))!=NULL;
    //    std::cout << "HIST=" << h0->GetName() << ": is_prof=" << is1_profile << ", is_prof2d=" << is1_profile2d << std::endl;
    //    std::cout << "HIST=" << h1->GetName() << ": is_prof=" << is2_profile << ", is_prof2d=" << is2_profile2d << std::endl;

    //___________________
    // CHI2, MEAN AND RMS
    int NDIM=ndim[0];
  
    // chi2
    int npoints=0;
    double chi2=0.0;
    switch(NDIM) {
    case 1: {
      if(!is1_profile && integral1) h0->Scale(1.0/integral1);
      if(!is2_profile && integral2) h1->Scale(1.0/integral2);
      int NBINSX=xaxis[0]->GetNbins();
      for(int ix=1; ix<=NBINSX; ix++) {
	double data0 = h0->GetBinContent(ix);
	double data1 = h1->GetBinContent(ix);
	double err0 = h0->GetBinError(ix);
	double err1 = h1->GetBinError(ix);
	if( err0!=0.0 && err1!=0.0 ) {
	  npoints++;
	  chi2 += pow( (data0-data1)/sqrt(err0*err0+err1*err1), 2);
	}
      }
      break;
    }
    case 2: {
      if(!is1_profile2d && integral1) h0->Scale(1.0/integral1);
      if(!is2_profile2d && integral2) h1->Scale(1.0/integral2);
      int NBINSX=xaxis[0]->GetNbins();
      int NBINSY=yaxis[0]->GetNbins();
      for(int ix=1; ix<=NBINSX; ix++) {
	for(int iy=1; iy<=NBINSY; iy++) {
	  double data0 = h0->GetBinContent(ix, iy);
	  double data1 = h1->GetBinContent(ix, iy);
	  double err0 = h0->GetBinError(ix, iy);
	  double err1 = h1->GetBinError(ix, iy);
	  if( err0!=0.0 && err1!=0.0 ) {
	    npoints++;
	    chi2 += pow( (data0-data1)/sqrt(err0*err0+err1*err1), 2);
	  }
	}
      }
      break;
    }
    };
    std::cout << std::scientific;
    std::cout << "CHI2/NPOINTS            = " << std::setw(13) 
	      << ( npoints > 0 ? chi2/npoints : 0.0 )
	      << "    CHI2 = " << std::setw(13) << chi2 
	      << "    NPOINTS = " << npoints << std::endl;
    
    // mean X
    double Xmean0 = h0->GetMean(1);
    double Xmean0_err = h0->GetMeanError(1);
    double Xmean1 = h1->GetMean(1);
    double Xmean1_err = h1->GetMeanError(1);
    double DeltaXmean = Xmean0-Xmean1;
    double DeltaXmean_err = sqrt( Xmean0_err*Xmean0_err + Xmean1_err*Xmean1_err );

    // rms Y
    double Xrms0 = h0->GetRMS(1);
    double Xrms0_err = h0->GetRMSError(1);
    double Xrms1 = h1->GetRMS(1);
    double Xrms1_err = h1->GetRMSError(1);
    double DeltaXrms = Xrms0-Xrms1;
    double DeltaXrms_err = sqrt( Xrms0_err*Xrms0_err + Xrms1_err*Xrms1_err );

    // shift of mean X
    std::cout << "Mean(X1)-Mean(X2)       = " << std::setw(13) << DeltaXmean
	      << "     Err = " <<std::setw(13) << DeltaXmean_err << std::endl;
    // pull of mean X
    std::cout << "Pull(Mean(X1)-Mean(X2)) = " << std::setw(13) 
	      << ( DeltaXmean_err>0.0 ? DeltaXmean/DeltaXmean_err : 
                   (DeltaXmean==0.0 ? 0.0 : 1e30 ) ) << std::endl;
    // shift of rms X
    std::cout << "Rms(X1)-Rms(X2)         = " << std::setw(13) << DeltaXrms
	      << "     Err = " <<std::setw(13) << DeltaXrms_err << std::endl;    
    // pull of rms X
    std::cout << "Pull(Rms(X1)-Rms(X2))   = " << std::setw(13) 
	      << ( DeltaXrms_err>0.0 ? DeltaXrms/DeltaXrms_err : 
                   (DeltaXrms==0.0 ? 0.0 : 1e30 ) ) << std::endl;

    if( NDIM==2 || (is1_profile && is2_profile) ) {

      // mean Y
      double Ymean0 = h0->GetMean(2);
      double Ymean0_err = h0->GetMeanError(2);
      double Ymean1 = h1->GetMean(2);
      double Ymean1_err = h1->GetMeanError(2);
      double DeltaYmean = Ymean0-Ymean1;
      double DeltaYmean_err = sqrt( Ymean0_err*Ymean0_err + Ymean1_err*Ymean1_err );
      
      // rms Y
      double Yrms0 = h0->GetRMS(2);
      double Yrms0_err = h0->GetRMSError(2);
      double Yrms1 = h1->GetRMS(2);
      double Yrms1_err = h1->GetRMSError(2);
      double DeltaYrms = Yrms0-Yrms1;
      double DeltaYrms_err = sqrt( Yrms0_err*Yrms0_err + Yrms1_err*Yrms1_err );
      
    // shift of mean Y
    std::cout << "Mean(Y1)-Mean(Y2)       = " << std::setw(13) << DeltaYmean
	      << "     Err = " <<std::setw(13) << DeltaYmean_err << std::endl;
    // pull of mean Y
    std::cout << "Pull(Mean(Y1)-Mean(Y2)) = " << std::setw(13) 
	      << ( DeltaYmean_err>0.0 ? DeltaYmean/DeltaYmean_err : 
                   (DeltaYmean==0.0 ? 0.0 : 1e30 ) ) << std::endl;

    // shift of rms Y
    std::cout << "Rms(Y1)-Rms(Y2)         = " << std::setw(13) << DeltaYrms
	      << "     Err = " <<std::setw(13) << DeltaYrms_err << std::endl;
    // pull of rms Y
    std::cout << "Pull(Rms(Y1)-Rms(Y2))   = " << std::setw(13) 
	      << ( DeltaYrms_err>0.0 ? DeltaYrms/DeltaYrms_err : 
                   (DeltaYrms==0.0 ? 0.0 : 1e30 ) ) << std::endl;
    }  

    std::cout << std::endl;
  }
  f0->Close();
  f1->Close();
 
  return 0;
}
void calc_chi2_for_2_files(const char *FIRST_root_file,
			   const char *SECOND_root_file,
			   const char *COMMON_HIST_name_file,
			   const char *COMMON_DIR_name) {
    const char *argv[5] = {
      "calc_chi2_for_2_files",
      FIRST_root_file, 
      SECOND_root_file, 
      COMMON_HIST_name_file,
      COMMON_DIR_name         };
    doit(5, argv); 
}
void calc_chi2_for_2_files(const char *FIRST_root_file,
			   const char *SECOND_root_file,
			   const char *FIRST_HIST_name_file,
			   const char *SECOND_HIST_name_file,
			   const char *FIRST_DIR_name,
			   const char *SECOND_DIR_name) {
    const char *argv[7] = {
      "calc_chi2_for_2_files",
      FIRST_root_file,
      SECOND_root_file,
      FIRST_HIST_name_file,
      SECOND_HIST_name_file,
      FIRST_DIR_name,
      SECOND_DIR_name };
    doit(7, argv); 
}

