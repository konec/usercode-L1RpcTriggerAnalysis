TH1D * DivideErr(TH1D * h1, 
                 TH1D *h2, 
                 char * name="DivideErr", 
                 char * optErr ="")
{
//
// return h1/h2 with recalculated errors for "B"
//
  TH1D * hout = new TH1D( *h1);
  hout->Reset(); 
  hout->SetName("DivideErr");
  hout->SetTitleOffset(gStyle->GetTitleXOffset(),"x");
  hout->SetTitleOffset(gStyle->GetTitleYOffset(),"y");
  hout->Divide(h1,h2,1.,1.,optErr);

  if (strcmp(optErr,"B")==0  || strcmp(optErr,"b")==0) {
    for (int i = 0; i<=hout->GetNbinsX()+1;i++) {
      Float_t tot   = h2->GetBinContent(i) ;
      Float_t tot_e = h2->GetBinError(i);
      Float_t eff = hout->GetBinContent(i) ;
      Float_t Err = 0.;
      if (tot > 0) Err = tot_e / tot * sqrt( eff* (1-eff) );
      if (eff == 1.) Err=1.e-3;
      hout->SetBinError(i, Err);
    }
  } else if (optErr == "" || optErr == " ") {
      ;
  } else {
    cout << "** Fig--DivideErr ** unknown option ---"<<optErr<<"---"<<endl;
  }

  return hout;
}

