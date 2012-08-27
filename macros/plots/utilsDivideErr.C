#ifndef utilsDivideErr_C
#define utilsDivideErr_C

#include <cstring>
#include <iostream>
#include <cmath>
#include "TH1D.h"

TH1D * DivideErr(TH1D * h1, 
                 TH1D * h2, 
                 const char * name="DivideErr", 
                 const char * optErr ="")
{
//
// return h1/h2 with recalculated errors for "B"
//
  if (!h1) std::cout <<"DivideErr called, but histogram (h1) pointer is:"<<h1<<std::endl;
  if (!h2) std::cout <<"DivideErr called, but histogram (h2) pointer is:"<<h1<<std::endl;
  if (!h1 || !h2) return 0;
  TH1D * hout = new TH1D( *h1);
  hout->Reset(); 
  hout->SetName(name);
//  hout->SetTitleOffset(gStyle->GetTitleXOffset(),"x");
//  hout->SetTitleOffset(gStyle->GetTitleYOffset(),"y");
  hout->Divide(h1,h2,1.,1.,optErr);

  if (strcmp(optErr,"B")==0  || strcmp(optErr,"b")==0) {
    for (int i = 0; i<=hout->GetNbinsX()+1;i++) {
      Float_t tot   = h2->GetBinContent(i) ;
      Float_t tot_e = h2->GetBinError(i);
      Float_t eff = hout->GetBinContent(i) ;
      Float_t Err = 0.;
      if (tot > 0) { 
        if (eff == 1.) eff = (tot-1)/tot; //modify efficiency as one even in numerator not fired
        Err = tot_e / tot * sqrt( eff* (1-eff) );
      }
      hout->SetBinError(i, Err);
    }
  } else {
    std::cout << "** Fig--DivideErr ** unknown option ---"<<optErr<<"---"<<std::endl;
  }

  return hout;
}

#endif
