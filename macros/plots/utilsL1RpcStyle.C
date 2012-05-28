#ifndef utilsL1RpcStyle_C
#define utilsL1RpcStyle_C

#include "TStyle.h"
#include <iostream>

void utilsL1RpcStyle() 
{ 
  TStyle *l1RpcStyle = new TStyle("L1RpcStyle","L1Rpc Style for common plots");

  std::cout <<"Define l1RpcStyle...";

//set the background color to white
  l1RpcStyle->SetFillColor(0);
  l1RpcStyle->SetFrameFillColor(0);
  l1RpcStyle->SetCanvasColor(0);
  l1RpcStyle->SetPadColor(0);
  l1RpcStyle->SetTitleFillColor(0);

//dont put a colored frame around the plots
  l1RpcStyle->SetFrameBorderMode(0);
  l1RpcStyle->SetCanvasBorderMode(0);
  l1RpcStyle->SetPadBorderMode(0);
  l1RpcStyle->SetLegendBorderSize(0);

// StyleOptions
  l1RpcStyle->SetFrameBorderSize(0);
  l1RpcStyle->SetFrameLineWidth(1);
  l1RpcStyle->SetFrameFillStyle(4000); //transparent  

// format
  l1RpcStyle->SetStripDecimals(kFALSE); 
//  TGaxis::SetMaxDigits(4);

// histo option
  l1RpcStyle->SetHistLineWidth(1);

// options
  l1RpcStyle->SetOptFit();
  l1RpcStyle->SetOptStat(0);
  l1RpcStyle->SetOptTitle(1);
  l1RpcStyle->SetMarkerSize(1.0);
  l1RpcStyle->SetPalette(14);

// stat options
  l1RpcStyle->SetStatBorderSize(1);
  l1RpcStyle->SetStatTextColor(4);
  l1RpcStyle->SetStatFontSize(0.03);
  l1RpcStyle->SetStatColor(0);

// fit option
  l1RpcStyle->SetFuncColor(2);
  l1RpcStyle->SetFuncWidth(2);

// PadAttributes
  l1RpcStyle->SetPadBorderMode(0);
  l1RpcStyle->SetPadBorderSize(0);
  l1RpcStyle->SetPadLeftMargin(0.155);
  l1RpcStyle->SetPadRightMargin(0.05);
  l1RpcStyle->SetPadTopMargin(0.095);
  l1RpcStyle->SetPadBottomMargin(0.135);
  l1RpcStyle->SetPadTickX(1);
  l1RpcStyle->SetPadTickY(1);
  
// to be overwritten by histogram attributes
  l1RpcStyle->SetLabelSize(0.05,"x");
  l1RpcStyle->SetLabelSize(0.05,"y");
  l1RpcStyle->SetLabelOffset(0.004,"x");
  l1RpcStyle->SetLabelOffset(0.014,"y");

  l1RpcStyle->SetTitleSize(0.05,"x");
  l1RpcStyle->SetTitleSize(0.05,"y");
  l1RpcStyle->SetTitleXOffset(1.25);
  l1RpcStyle->SetTitleYOffset(1.4);

//  l1RpcStyle->SetTitleX(0.12); 
//  l1RpcStyle->SetTitleW(0.78); 
  std::cout <<"..done"<<std::endl;
}

#endif
