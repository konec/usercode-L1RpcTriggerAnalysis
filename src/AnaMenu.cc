#include "UserCode/L1RpcTriggerAnalysis/interface/AnaMenu.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraphErrors.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TriggerMenuResultObj.h"
#include <sstream>
#include <iostream>
#include <cmath>

namespace { 
 TH1D *hMuonPt_MEN, *hMuonEta_MEN, *hMuonPhi_MEN;
} 


void  AnaMenu::updateMenu(const std::vector<std::string> & menuL1, const std::vector<std::string> & menuHLT)
{
  if (menuL1.size() != 0) theMenuL1 = menuL1;
  if (menuHLT.size() != 0) theMenuHLT = menuHLT;
}

bool AnaMenu::filter( const EventObj* ev, const MuonObj* muon,
                      const TriggerMenuResultObj *bitsL1,
                      const TriggerMenuResultObj *bitsHLT)

{
  const std::vector<unsigned int> & algosL1 = bitsL1->firedAlgos;
  const std::vector<unsigned int> & algosHLT = bitsHLT->firedAlgos;

  typedef std::vector<unsigned int>::const_iterator CIT;


  bool okL1 = false;
  std::vector<std::string> acceptL1_Names = theConfig.exists("acceptL1_Names") ?  theConfig.getParameter<std::vector<std::string> >("acceptL1_Names") : std::vector<std::string>();
  for (CIT it=algosL1.begin(); it != algosL1.end(); ++it) {
    std::string nameAlgo = theMenuL1[*it];
    if (theAlgosL1.find(nameAlgo) == theAlgosL1.end()) theAlgosL1[nameAlgo]=0;    
    bool isMu = ( nameAlgo.find("Mu") != std::string::npos);
    if (theConfig.getParameter<bool>("acceptL1_OtherThanMu") && !isMu ) okL1 = true;
    if (theConfig.getParameter<bool>("acceptL1_Mu") &&  isMu) okL1 = true;
    for ( std::vector<std::string>::const_iterator is=acceptL1_Names.begin(); is != acceptL1_Names.end(); ++is) if (nameAlgo==(*is)) okL1 = true;
  }

  bool okHLT = false;
  for (CIT it=algosHLT.begin(); it != algosHLT.end(); ++it) {
    std::string nameAlgo = theMenuHLT[*it];
    if (theAlgosHLT.find(nameAlgo) == theAlgosHLT.end()) theAlgosHLT[nameAlgo]=0;    
    bool isMu = (    (    (nameAlgo.find("Mu") != std::string::npos   ) 
                       && (nameAlgo.find("Multi") == std::string::npos) ) 
                  || (     nameAlgo.find("muon") != std::string::npos   )  );
    if ( theConfig.getParameter<bool>("acceptHLT_OtherThanMuPhysicsAlCa") 
         && !isMu 
         && (nameAlgo.find("Physics") == std::string::npos) 
         && (nameAlgo.find("AlCa") == std::string::npos) ) okHLT = true;
    if (theConfig.getParameter<bool>("acceptHLT_Mu") && isMu) okHLT = true; 
    if (theConfig.getParameter<bool>("acceptHLT_L1")       && (nameAlgo.find("HLT_L1")   != std::string::npos) ) okHLT = true;
    if (theConfig.getParameter<bool>("acceptHLT_Physics")  && (nameAlgo.find("Physics")  != std::string::npos) ) okHLT = true;
    if (theConfig.getParameter<bool>("acceptHLT_ZeroBias") && (nameAlgo.find("ZeroBias") != std::string::npos) ) okHLT = true;
  }

  if (okL1  && okHLT) {
    for (CIT it=algosL1.begin();  it != algosL1.end();  ++it)  theAlgosL1[ theMenuL1[*it] ]++; 
    for (CIT it=algosHLT.begin(); it != algosHLT.end(); ++it) theAlgosHLT[ theMenuHLT[*it] ]++;
    if (hMuonPt_MEN)  hMuonPt_MEN->Fill(muon->pt());
    if (hMuonEta_MEN) hMuonEta_MEN->Fill(muon->eta());
    if (hMuonPhi_MEN) hMuonPhi_MEN->Fill(muon->phi());
    return true; 
  } else return  false;

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
//    std::cout <<" BIN "<<ibin<<" LABEL: "<<(*it).first.c_str()<<" ENTRIES:"<<(*it).second<<std::endl;
  }
  ibin = 0;
  for (CIM it=theAlgosHLT.begin(); it != theAlgosHLT.end(); ++it) {
    ibin++;
    hMenuAlgosHLT->GetXaxis()->SetBinLabel(ibin, (*it).first.c_str());
    hMenuAlgosHLT->SetBinContent(ibin, (*it).second);
//    std::cout <<" BIN "<<ibin<<" LABEL: "<<(*it).first.c_str()<<" ENTRIES:"<<(*it).second<<std::endl;
  }
}


void AnaMenu::init(TObjArray& histos)
{
  hMuonPt_MEN  = new TH1D("hMuonPt_MEN","All global muons Pt;Glb.muon p_{T} [GeV];Muons / bin",L1PtScale::nPtBins,L1PtScale::ptBins); histos.Add(hMuonPt_MEN);
  hMuonEta_MEN = new TH1D("hMuonEta_MEN","All global muons Eta;Glb.muon #eta;Muons / bin",96, -2.4, 2.4);  histos.Add(hMuonEta_MEN);
  hMuonPhi_MEN = new TH1D("hMuonPhi_MEN","All global muons Phi;Glb.muon #phi [rad];Muons / bin",90,-M_PI,M_PI);  histos.Add(hMuonPhi_MEN);
}
