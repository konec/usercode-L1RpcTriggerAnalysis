#include "UserCode/L1RpcTriggerAnalysis/interface/AnaSynch.h"
#include "TProfile.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"

#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "CondFormats/RPCObjects/interface/RPCEMap.h"
#include "CondFormats/DataRecord/interface/RPCEMapRcd.h"
#include "CondFormats/RPCObjects/interface/RPCReadOutMapping.h"

#include "DQM/RPCMonitorClient/interface/RPCLinkSynchroHistoMaker.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include <fstream>

namespace {
  TH1F *hSynch_delaySummary;  
  TH2F *hSynch_delaySpread, *hSynch_topOccup, *hSynch_topSpread;
  TH2F *hSynch_notComplete[3];
}




AnaSynch::AnaSynch()
 : theSynchroStat(true)
{ }

void AnaSynch::init(TObjArray& histos)
{

  hSynch_delaySummary = new TH1F("hSynch_delaySummary","LinkDelaySummary",8,-3.5, 4.5); hSynch_delaySummary->SetStats(111); histos.Add(hSynch_delaySummary);

  double  widthX=1.0/50 ;                   // bin width of 1/25 BX = 1ns
  unsigned int  nbinsX=2*(Int_t)(3.0/widthX)+1; // nbins for [-3 BX, +3 BX] range
  double  widthY=1.0/50 ;                   // bin width of 1/25 BX = 1ns
  unsigned int nbinsY=(Int_t)(7.0/widthY);     // nbins for [0 BX, 7 BX[ range

  hSynch_delaySpread = new TH2F("hSynch_delaySpread",
        "Average signal spread vs average signal arrival per Link Board; Avg signal arrival wrt L1A per Link Board [BX]; Avg signal spread per Link Board [BX]; Link Boards / bin",
        nbinsX, 0.0-(int)(nbinsX/2)*widthX, 0.0+(int)(nbinsX/2)*widthX,
        nbinsY, 0.0, 0.0+nbinsY*widthY);
 //        71,-3.05, 4.05, 31,-0.05,3.05); 
  hSynch_delaySpread->Sumw2();
  //hSynch_delaySpread->SetStats(0); 
  histos.Add(hSynch_delaySpread);

  hSynch_notComplete[0] = new TH2F("hSynch_notComplete790","FED790: not All Paths hit",36,-0.5,35.5,18,-0.5,17.5); histos.Add(hSynch_notComplete[0]);
  hSynch_notComplete[1] = new TH2F("hSynch_notComplete791","FED791: not All Paths hit",36,-0.5,35.5,18,-0.5,17.5); histos.Add(hSynch_notComplete[1]);
  hSynch_notComplete[2] = new TH2F("hSynch_notComplete792","FED792: not All Paths hit",36,-0.5,35.5,18,-0.5,17.5); histos.Add(hSynch_notComplete[2]);
  for (unsigned int i=0;i<3;++i) {
    hSynch_notComplete[i]->GetXaxis()->SetNdivisions(512);
    hSynch_notComplete[i]->GetYaxis()->SetNdivisions(505);
    hSynch_notComplete[i]->SetXTitle("rmb");
    hSynch_notComplete[i]->SetYTitle("link");
    hSynch_notComplete[i]->SetStats(0);
  }
  hSynch_topOccup  = new TH2F("hSynch_topOccup","Top10 LinkBoard occupancy",8,-0.5,7.5, 10,0.,10.); histos.Add( hSynch_topOccup);
  hSynch_topSpread = new TH2F("hSynch_topSpread","Top10 LinkBoard delay spread",8,-0.5,7.5, 10,0.,10.); histos.Add( hSynch_topSpread);
  hSynch_topOccup->GetXaxis()->SetNdivisions(110);
  hSynch_topSpread->GetXaxis()->SetNdivisions(110);
  hSynch_topOccup->SetStats(0);
  hSynch_topSpread->SetStats(0);

 }

void AnaSynch::beginRun(const edm::Run&, const edm::EventSetup& es)
{
  edm::ESTransientHandle<RPCEMap> readoutMapping;
  es.get<RPCEMapRcd>().get(readoutMapping);
  const RPCReadOutMapping * cabling = readoutMapping->convert();
  std::cout <<" INITIALISING READOUT MAP, version: "<< cabling->version() << std::endl;
  theSynchroStat.init(cabling, true); // second for use of DetNames
  delete cabling;
}


void AnaSynch::run( const EventObj* event, const MuonObj* muon, const  RPCRawSynchro::ProdItem & synchro)
{
  std::vector<LinkBoardElectronicIndex> problems;
  theSynchroStat.add(synchro, problems);
  for (std::vector<LinkBoardElectronicIndex>::const_iterator it=problems.begin(); it != problems.end(); ++it) {
    hSynch_notComplete[it->dccId-790]->Fill(it->dccInputChannelNum,it->tbLinkInputNum);
  }
} 

void AnaSynch::endJob()
{
  RPCLinkSynchroHistoMaker hm(theSynchroStat);
  hm.fill(hSynch_delaySummary, hSynch_delaySpread, hSynch_topOccup, hSynch_topSpread);

  std::ofstream file("l1RpcDelays.txt");
  file << theSynchroStat.dumpDelays() << std::endl;
  file.close();
}
