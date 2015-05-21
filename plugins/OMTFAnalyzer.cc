#include "UserCode/L1RpcTriggerAnalysis/plugins/OMTFAnalyzer.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"

#include "L1Trigger/RPCTrigger/interface/RPCConst.h"

#include "SimDataFormats/Track/interface/SimTrack.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/BestSimulatedMuonFinder.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <strstream>

OMTFAnalyzer::OMTFAnalyzer(const edm::ParameterSet & cfg)
  : theConfig(cfg), theAnaEff(0){

  if (theConfig.exists("anaEff")) theAnaEff = new AnaEff( cfg.getParameter<edm::ParameterSet>("anaEff"));
  
  trigOMTFCandSrc = cfg.getParameter<edm::InputTag>("OMTFCandSrc");
  trigGMTCandSrc = cfg.getParameter<edm::InputTag>("GMTCandSrc");
  g4SimTrackSrc = cfg.getParameter<edm::InputTag>("g4SimTrackSrc");
  multiplyEvents = 1;
  if (theConfig.exists("multiplyEvents"))  multiplyEvents = cfg.getParameter<unsigned int>("multiplyEvents");

  inputOMTFToken = consumes<std::vector<L1MuRegionalCand> >(trigOMTFCandSrc);
  inputGMTToken = consumes<L1MuGMTReadoutCollection>(trigGMTCandSrc);
  consumes<edm::SimTrackContainer>(g4SimTrackSrc);

}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void OMTFAnalyzer::beginJob(){
  
  theHistos.SetOwner();

  if (theAnaEff) theAnaEff->init(theHistos);
  
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void OMTFAnalyzer::analyze(const edm::Event& ev, const edm::EventSetup& es){

  std::ostringstream myStr;

  ///Get the simulated muon parameters
  TrackObj* simu = new TrackObj();
  TrackObj* simu1 = new TrackObj();
  const SimTrack* aSimMuon1 = BestSimulatedMuonFinder().result(ev,es);
  const SimTrack* aSimMuon2 = BestSimulatedMuonFinder().result(ev,es,aSimMuon1);
  const SimTrack* aSimMuon = aSimMuon1;
  if (aSimMuon) { 
    int charge = (abs(aSimMuon->type()) == 13) ? aSimMuon->type()/-13 : 0;
    simu->setKine(aSimMuon->momentum().pt(), aSimMuon->momentum().eta(),aSimMuon->momentum().phi(), charge);
  }
  if (aSimMuon2) { 
    int charge = (abs(aSimMuon2->type()) == 13) ? aSimMuon2->type()/-13 : 0;
    simu1->setKine(aSimMuon2->momentum().pt(), aSimMuon2->momentum().eta(),aSimMuon2->momentum().phi(), charge);
  }

  ///Get the GMT and old subststems response
  std::vector<L1Obj> theL1Objs;
  //getGMTReadout( ev, theL1Objs,L1Obj::RPCb); 
  //getGMTReadout( ev, theL1Objs,L1Obj::RPCf);
  //getGMTReadout( ev, theL1Objs,L1Obj::CSC);
  //getGMTReadout( ev, theL1Objs,L1Obj::DT);
  getGMTReadout( ev, theL1Objs,L1Obj::GMT);

  ///Get the OMTF result  
  getOMTFCandidates(ev,theL1Objs);

  L1ObjColl myL1ObjColl;
  for(auto aObj:theL1Objs) myL1ObjColl.push_back(aObj, false, 0.); 

  for(unsigned int i=0;i<multiplyEvents;++i) if (theAnaEff) theAnaEff->run(simu, &myL1ObjColl,0,simu1);
 
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
bool OMTFAnalyzer::getGMTReadout(const edm::Event &iEvent,
				 std::vector<L1Obj> &result,
				 L1Obj::TYPE type){

  std::ostringstream myStr;

  edm::Handle<L1MuGMTReadoutCollection> gmtCandidates;
  iEvent.getByToken(inputGMTToken, gmtCandidates);

  L1MuGMTReadoutCollection const* gmtrc = gmtCandidates.product();
  if (!gmtrc) return false;

  std::vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  std::vector<L1MuGMTReadoutRecord>::const_iterator RRItr;
  for( RRItr = gmt_records.begin() ; RRItr != gmt_records.end() ; RRItr++ ) {
    std::vector<L1MuRegionalCand>    cands;
    std::vector<L1MuGMTExtendedCand> gmts;

    switch (type) {
      case L1Obj::RPCb: { cands = RRItr->getBrlRPCCands(); break; }  
      case L1Obj::RPCf: { cands = RRItr->getFwdRPCCands(); break; }
      case L1Obj::DT:   { cands = RRItr->getDTBXCands(); break; }
      case L1Obj::CSC:  { cands = RRItr->getCSCCands(); break; }
      case L1Obj::GMT:      { gmts  = RRItr->getGMTCands(); 

	myStr<<"GMT cands: "<<cands.size()<<std::endl;
	if(cands.size()) myStr<<" GMT pt code: "<<cands[0].pt_packed()<<std::endl;
	break; 
      }
      case L1Obj::GMT_emu:  { gmts  = RRItr->getGMTCands(); break; }
      default: break;
    };

    typedef std::vector<L1MuRegionalCand>::const_iterator ITC;
    for(ITC it = cands.begin() ; it != cands.end() ; ++it ) {
      if (it->empty()) continue;
      result.push_back( makeL1Obj( *it, type) );
    }

    typedef std::vector<L1MuGMTExtendedCand>::const_iterator ITG;
    for(ITG it = gmts.begin() ; it != gmts.end() ; ++it ) {
      if (it->empty()) continue;
      result.push_back( makeL1Obj( *it, type) );
    }
  }

  edm::LogInfo("OMTFAnalyzer")<<myStr.str();
  return true;
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
bool OMTFAnalyzer::getOMTFCandidates(const edm::Event &iEvent,
				     std::vector<L1Obj> &result){

  edm::Handle<std::vector<L1MuRegionalCand> > omtfCandidates; 
  iEvent.getByToken(inputOMTFToken, omtfCandidates);
  for (auto it: *omtfCandidates.product()) {
    if (it.empty()) continue;
    L1Obj obj;
    obj.eta = it.etaValue()/1000*4.0;
    obj.phi = it.phiValue();
    obj.pt  = RPCConst::ptFromIpt(it.pt_packed());
    obj.charge = it.chargeValue();
    obj.q   = it.getHitsWord();
    obj.refLayer   = it.quality();
    obj.disc   = it.getDiscVal();
    obj.bx  = it.bx();
    obj.type = L1Obj::OTF;
    result.push_back(obj);
  }
  return true;
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void OMTFAnalyzer::endJob(){

  std::cout <<"ENDJOB, summaries:"<<std::endl;
  if(theAnaEff) delete theAnaEff;
}
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
