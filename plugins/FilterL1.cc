#include "FilterL1.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <set>

using namespace edm;
using namespace std;

FilterL1::FilterL1(const edm::ParameterSet& cfg)
  : theCounter(0), theAllCounter(0), 
    thePtCut(cfg.getParameter<double>("ptCut"))
{ 
 theL1MuReadout_Tag= consumes<L1MuGMTReadoutCollection>(cfg.getParameter<edm::InputTag>("l1MuReadout"));
}

FilterL1::~FilterL1()
{
  std::cout <<"FilterL1, passed: "<<theCounter<<" of "<< theAllCounter << std::endl;
}

bool FilterL1::filter(edm::Event&ev, const edm::EventSetup&es)
{

  bool goodEvent = false;
  theAllCounter++;
      
  edm::Handle<L1MuGMTReadoutCollection> pCollection;
  ev.getByToken(theL1MuReadout_Tag,pCollection);

  L1MuGMTReadoutCollection const* gmtrc = pCollection.product();

  if (!gmtrc) return goodEvent;

  vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  vector<L1MuGMTReadoutRecord>::const_iterator RRItr;
  bool brlRPC = false;
  bool fwdRPC = false;
  bool CSC = false;
  bool DT = false;
  vector<L1MuRegionalCand> Cands;
  typedef vector<L1MuRegionalCand>::const_iterator ITC;

  std::ostringstream str;

  for( RRItr = gmt_records.begin() ; RRItr != gmt_records.end() ; RRItr++ ) {
    Cands = RRItr->getCSCCands();
    for(ITC it = Cands.begin() ; it != Cands.end() ; ++it ) {
      if (it->empty()) continue;
      if (fabs(it->etaValue()) < 1.6 && it->ptValue() >= thePtCut ) CSC = true;
      str <<"HAS  CSC cand "
          <<" pt: "<<it->ptValue()
          <<" eta: "<<it->etaValue()
          <<" phi: "<<it->phiValue()
          <<" Q:" <<it->quality()
          <<" bx: " <<it->bx()
          <<std::endl;
    }
    Cands = RRItr->getDTBXCands();
    for(ITC it = Cands.begin() ; it != Cands.end() ; ++it ) {
      if (it->empty()) continue;
      if (it->ptValue() >= thePtCut) DT= true;
      str <<"HAS   DT cand "
          <<" pt: "<<it->ptValue()
          <<" eta: "<<it->etaValue()
          <<" phi: "<<it->phiValue()
          <<" Q:" <<it->quality()
          <<" bx: " <<it->bx()
          <<std::endl;
    }
    Cands = RRItr->getBrlRPCCands();
    for(ITC it = Cands.begin() ; it != Cands.end() ; ++it ) {
      if (it->empty()) continue;
      if (it->ptValue() >= thePtCut) brlRPC = true;
      str <<"HAS RPCB cand "
          <<" pt: "<<it->ptValue()
          <<" eta: "<<it->etaValue()
          <<" phi: "<<it->phiValue()
          <<" Q:" <<it->quality()
          <<" bx: " <<it->bx()
          <<std::endl;
    }
    Cands = RRItr->getFwdRPCCands();
    for(ITC it = Cands.begin() ; it != Cands.end() ; ++it ) {
      if (it->empty()) continue;
      if (it->ptValue() >= thePtCut) fwdRPC = true;
      str <<"HAS RPCF cand "
          <<" pt: "<<it->ptValue()
          <<" eta: "<<it->etaValue()
          <<" phi: "<<it->phiValue()
          <<" Q:" <<it->quality()
          <<" bx: " <<it->bx()
          <<std::endl;
    }
  }
  if (brlRPC || fwdRPC || CSC || DT) goodEvent = true;
  if (goodEvent) theCounter++;
  if (goodEvent) std::cout << str.str() << std::endl;
  return goodEvent;
}

