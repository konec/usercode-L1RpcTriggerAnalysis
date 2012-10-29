#include "UserCode/L1RpcTriggerAnalysis/interface/AnaMenu.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include <sstream>
#include <iostream>

namespace { 
} 


bool AnaMenu::filter( const EventObj* ev, const MuonObj* muon,
                  const  std::vector<std::string> & namesL1,
                  const std::vector<unsigned int> & algosL1,
                  const std::vector<std::string> &  namesHLT,
                  const std::vector<unsigned int> & algosHLT)
{
  typedef std::vector<unsigned int>::const_iterator CIT;

//  static int count = 0;
//  std::cout <<"----------"<<std::endl;
  bool hasNoL1Mu= false;
  for (CIT it=algosL1.begin(); it != algosL1.end(); ++it) {
    std::string nameAlgo = namesL1[*it];
//    std::cout <<nameAlgo << std::endl;
    if (theAlgosL1.find(nameAlgo) == theAlgosL1.end()) theAlgosL1[nameAlgo]=0;    
    bool isMu = ( nameAlgo.find("Mu") != std::string::npos);
    if (!isMu) hasNoL1Mu = true;
  }
// if (hasNoL1Mu) std::cout <<" HAS NO L1 MUON!: " <<++count<< std::endl;

  bool hasNoHLTMu= false;
  for (CIT it=algosHLT.begin(); it != algosHLT.end(); ++it) {
    std::string nameAlgo = namesHLT[*it];
//    std::cout <<nameAlgo << std::endl;
    if (theAlgosHLT.find(nameAlgo) == theAlgosHLT.end()) theAlgosHLT[nameAlgo]=0;    
    if (nameAlgo.find("Physic") != std::string::npos) continue;
    bool isMu = ( (nameAlgo.find("Mu") != std::string::npos) && (nameAlgo.find("Multi") == std::string::npos) );
    if (!isMu) hasNoHLTMu = true;
  }
//  if (hasNoHLTMu) std::cout <<" HAS NO HLT MUON!" << std::endl;

  if (hasNoL1Mu && hasNoHLTMu) {
//  if (hasNoL1Mu ) {
    for (CIT it=algosL1.begin();  it != algosL1.end();  ++it)  theAlgosL1[ namesL1[*it] ]++; 
    for (CIT it=algosHLT.begin(); it != algosHLT.end(); ++it) theAlgosHLT[ namesHLT[*it] ]++;
    return true;
  }

  return  false;
}

void AnaMenu::resume(TObjArray& histos)
{
  unsigned int sizeL1  = theAlgosL1.size();
  unsigned int sizeHLT = theAlgosHLT.size();
  TH1D *hMenuAlgosL1  = new TH1D( "hMenuAlgosL1", "hMenuAlgosL1", sizeL1, 1., 1.+sizeL1);  histos.Add(hMenuAlgosL1); 
  TH1D *hMenuAlgosHLT = new TH1D("hMenuAlgosHLT","hMenuAlgosHLT",sizeHLT, 1., 1.+sizeHLT); histos.Add(hMenuAlgosHLT); 
  unsigned int ibin = 0;
  typedef std::map< std::string, unsigned int>::const_iterator CIM;
  for (CIM it=theAlgosL1.begin(); it != theAlgosL1.end(); ++it) {
    ibin++;
    hMenuAlgosL1->GetXaxis()->SetBinLabel(ibin, (*it).first.c_str());
    hMenuAlgosL1->SetBinContent(ibin, (*it).second);
    std::cout <<" BIN "<<ibin<<" LABEL: "<<(*it).first.c_str()<<" ENTRIES:"<<(*it).second<<std::endl;
  }
  ibin = 0;
  for (CIM it=theAlgosHLT.begin(); it != theAlgosHLT.end(); ++it) {
    ibin++;
    hMenuAlgosHLT->GetXaxis()->SetBinLabel(ibin, (*it).first.c_str());
    hMenuAlgosHLT->SetBinContent(ibin, (*it).second);
    std::cout <<" BIN "<<ibin<<" LABEL: "<<(*it).first.c_str()<<" ENTRIES:"<<(*it).second<<std::endl;
  }
}


void AnaMenu::init(TObjArray& histos)
{
/*
 hClu_SizeDigi = new TH1D( "hClu_SizeDigi", "hClu_SizeDigi", 97, -0.5, 96.5);  histos.Add( hClu_SizeDigi);
 hClu_SizeRHit = new TH1D( "hClu_SizeRHit", "hClu_SizeRHit", 97, -0.5, 96.5);  histos.Add( hClu_SizeRHit);
 hClu_DigiRHit = new TH2D( "hClu_DigiRHit","hClu_DigiRHit", 97, -0.5, 96.5, 97, -0.5, 96.5);  histos.Add(hClu_DigiRHit);
*/
}
