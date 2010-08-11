{
  cout <<"Define MyDefaultStyle...";
  TStyle * s = new TStyle("MyDefaultStyle","Little margins");

// StyleOptions
  s->SetCanvasColor(0);
  s->SetCanvasBorderMode(0);
  s->SetFrameBorderMode(0);
  s->SetFrameBorderSize(0);
  s->SetFrameLineWidth(2);
  s->SetFrameFillColor(0);  
  s->SetFrameFillStyle(1000);  

// format
  s->SetStripDecimals(kFALSE); 
// TGaxis::SetMaxDigits(1);

// histo option
  s->SetHistLineWidth(3);

// options
  s->SetOptFit();
  s->SetOptStat(0);
  s->SetOptTitle(0);
  s->SetMarkerSize(1.0);
  s->SetPalette(14);

// stat options
  s->SetStatBorderSize(1);
  s->SetStatTextColor(4);
//  s->SetStatFontSize(0.03);
  s->SetStatColor(0);

// fit option
  s->SetFuncColor(2);
  s->SetFuncWidth(3);

// PadAttributes
  s->SetPadBorderMode(0);
  s->SetPadBorderSize(0);
  s->SetPadLeftMargin(0.155);
  s->SetPadRightMargin(0.11);
  s->SetPadTopMargin(0.065);
  s->SetPadBottomMargin(0.13);
  s->SetPadTickX(1);
  s->SetPadTickY(1);
  
// to be overwritten by histogram attributes
  s->SetLabelSize(0.05,"x");
  s->SetLabelSize(0.05,"y");
  s->SetLabelOffset(0.008,"x");
  s->SetLabelOffset(0.015,"y");

  s->SetTitleSize(0.05,"x");
  s->SetTitleSize(0.05,"y");
  s->SetTitleXOffset(1.1);
  s->SetTitleYOffset(1.4);

//

  cout << "done"<<endl;
}
