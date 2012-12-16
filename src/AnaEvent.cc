#include "UserCode/L1RpcTriggerAnalysis/interface/AnaEvent.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObjBXExtra.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>

namespace { 
 TH1D *hEvent_BX, *hEvent_BX_val, *hEvent_BX_nm2, *hEvent_BX_np2;
} 

AnaEvent::AnaEvent(const edm::ParameterSet& cfg) 
  : debug(false),
    theUseValidBX_ONLY(false),
    theUseValidBX_NoBxMinus2_ONLY(false),
    theUseValidBX_NoBxPlus2_ONLY(false)
{

  theSkipRuns = cfg.getParameter<std::vector<unsigned int> >("skipRuns");
  std::sort(theSkipRuns.begin(), theSkipRuns.end());

  if (cfg.exists("useValidBX_ONLY"))                       theUseValidBX_ONLY =  cfg.getParameter<bool>("useValidBX_ONLY");
  if (cfg.exists("useValidBX_NoBxMinus2_ONLY")) theUseValidBX_NoBxMinus2_ONLY =  cfg.getParameter<bool>("useValidBX_NoBxMinus2_ONLY");
  if (cfg.exists("useValidBX_NoBxPlus2_ONLY"))   theUseValidBX_NoBxPlus2_ONLY =  cfg.getParameter<bool>("useValidBX_NoBxPlus2_ONLY");

  std::ifstream infile( cfg.getParameter<std::string>("validBXes") );
  while (infile.good() && infile.peek() != EOF) {
    char c = infile.peek();
    if ( c=='#') infile.ignore(256,'\n');  //comment
    else if ( c==',' || c==' ' || c=='\n') infile.ignore();
    else if ( (c >= '0') && (c <= '9') ) {int bx; infile >>bx;  theValidBX.set(bx);}
    else { std::cout <<"Unexpected input file formatting, line is>>>"<<c<<"<<<"<<std::endl;infile.ignore(); }
  }
  infile.close();
}

bool AnaEvent::filter(EventObj* ev)

{
  // skip if wrong run
  if ( binary_search(theSkipRuns.begin(), theSkipRuns.end(), ev->run ) ) return  false;
/*
  if (ev->run == 200466) {
   std::cout <<"RUN IS: "<< ev->run <<std::endl;
   std::cout <<" result of search: "<<  binary_search(theSkipRuns.begin(), theSkipRuns.end(), ev->run ) << std:endl;
   std::cout <<"runs are: "; for 
   exit(7);
  }
*/

  // skip if event duplication
  if ( theRunEvent.find(std::make_pair(ev->run, ev->id) ) == theRunEvent.end()) theRunEvent[std::make_pair(ev->run, ev->id)] = ev->time;
  else return false; 

  // filter BXes
  bool validBXm2 = ( (int)ev->bx-2>=0 && theValidBX[ev->bx-2] );
  bool validBXp2 = ( ev->bx+2<=3563   && theValidBX[ev->bx+2] );

  //control histograms
  hEvent_BX->Fill(ev->bx);

  if (theUseValidBX_ONLY && !theValidBX[ev->bx]) return false; 
  if (theUseValidBX_NoBxMinus2_ONLY && validBXm2) return false;
  if (theUseValidBX_NoBxPlus2_ONLY  && validBXp2) return false; 

  if (theValidBX[ev->bx]) hEvent_BX_val->Fill(ev->bx);
  if (!validBXm2) hEvent_BX_nm2->Fill(ev->bx);
  if (!validBXp2) hEvent_BX_np2->Fill(ev->bx);

  //fill eventExtra structure
  if ( EventObjBXExtra* evbx = dynamic_cast<EventObjBXExtra*>(ev) ) {
    evbx->hasValidBX = theValidBX[evbx->bx];
    evbx->hasValidBX_Minus2 = validBXm2;
    evbx->hasValidBX_Plus2 = validBXp2;
  }
  
  return true;
}

void AnaEvent::resume(TObjArray& histos)
{
}

void AnaEvent::init(TObjArray& histos)
{
  hEvent_BX = new TH1D("hEvent_BX","hEvent_BX",3564,0., 3564.);  histos.Add( hEvent_BX);
  hEvent_BX_val = new TH1D("hEvent_BX_val","hEvent_BX_val",3564,0., 3564.);  histos.Add( hEvent_BX_val);
  hEvent_BX_nm2 = new TH1D("hEvent_BX_nm2","hEvent_BX_nm2",3564,0., 3564.);  histos.Add( hEvent_BX_nm2);
  hEvent_BX_np2 = new TH1D("hEvent_BX_np2","hEvent_BX_np2",3564,0., 3564.);  histos.Add( hEvent_BX_np2);
}
