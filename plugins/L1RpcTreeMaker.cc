#include "L1RpcTreeMaker.h"
#include <vector>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"


#include "UserCode/L1RpcTriggerAnalysis/interface/BestMuonFinder.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetHitCompatibleCollector.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjMaker.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackToL1ObjMatcher.h"
#include "DataFormats/RPCDigi/interface/RPCRawSynchro.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/ConverterRPCRawSynchroSynchroCountsObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TriggerMenuResultObj.h"

#include "TFile.h"
#include "TTree.h"

template <class T> T sqr( T t) {return t*t;}

L1RpcTreeMaker::L1RpcTreeMaker(const edm::ParameterSet& cfg)
  : theConfig(cfg), theTree(0), event(0), muon(0), track(0), 
    bitsL1(0), bitsHLT(0),
    counts(0), 
    l1ObjColl(0), 
    theCounter(0),
    theBestMuonFinder(cfg.getParameter<edm::ParameterSet>("bestMuonFinder")),
    theDetHitCollector(cfg.getParameter<edm::ParameterSet>("detHitCollector")),
    theSynchroGrabber(cfg.getParameter<edm::ParameterSet>("linkSynchroGrabber")),
    theL1ObjMaker(cfg.getParameter<edm::ParameterSet>("l1ObjMaker")),
    theMenuInspector(edm::ParameterSet())
{ }

void L1RpcTreeMaker::beginRun(const edm::Run &ru, const edm::EventSetup &es)
{
  std::cout <<" L1RpcTreeMaker::beginRun CALLED" << std::endl; 
  theMenuInspector.checkRun(ru,es);
}

void L1RpcTreeMaker::beginJob()
{
  theFile = new TFile(theConfig.getParameter<std::string>("treeFileName").c_str(),"RECREATE");
  theTree = new TTree("tL1Rpc","L1RpcEfficciency");

  theTree->Branch("event","EventObj",&event,32000,99);
  theTree->Branch("muon","MuonObj",&muon,32000,99);
  theTree->Branch("track", "TrackObj",&track,32000,99);

  theTree->Branch("bitsL1" ,"TriggerMenuResultObj",&bitsL1 ,32000,99);
  theTree->Branch("bitsHLT","TriggerMenuResultObj",&bitsHLT,32000,99);

  theTree->Branch("counts",&counts);
  theTree->Branch("detsCrossedByMuon",&detsCrossedByMuon);
  theTree->Branch("detsCrossedByMuonDeepInside",&detsCrossedByMuonDeepInside);
  theTree->Branch("detsHitsCompatibleWithMuon",&detsHitsCompatibleWithMuon);
  theTree->Branch("detsSIMU",&detsSIMU);

  theTree->Branch("l1ObjColl","L1ObjColl",&l1ObjColl,32000,99);

  theHelper.SetOwner();
  theBestMuonFinder.initHistos(theHelper);
  theDetHitCollector.initHistos(theHelper);
  theSynchroGrabber.initHistos(theHelper);
}

void L1RpcTreeMaker::endJob()
{
  theFile->Write();
  delete theFile;

  std::string helperFile = theConfig.getParameter<std::string>("histoFileName");
  TFile f(helperFile.c_str(),"RECREATE");
  theHelper.Write();
  f.Close();
}

L1RpcTreeMaker::~L1RpcTreeMaker()
{
  std::cout <<"L1RpcTreeMaker: Event counter is: "<<theCounter<<std::endl;
}

void L1RpcTreeMaker::analyze(const edm::Event &ev, const edm::EventSetup &es)
{
  //
  // check reference muon
  //
  const reco::Muon * theMuon = theBestMuonFinder.result(ev,es);
  if (theConfig.getParameter<bool>("onlyBestMuEvents") && (!theMuon) ) return;
  theCounter++;

  //
  // fill event information
  //
  event = new EventObj;
  event->bx = ev.bunchCrossing();
  event->orbit = ev.orbitNumber();
  event->time = ev.time().value();
  event->id = ev.id().event();
  event->run = ev.run();
  event->lumi = ev.luminosityBlock();

  //
  // create other objects structure
  //
  muon = new MuonObj();
  track = new TrackObj();

  bitsL1 = new TriggerMenuResultObj();
  bitsHLT = new TriggerMenuResultObj();

  counts = std::vector<SynchroCountsObj>();
  detsCrossedByMuon = std::vector<uint32_t>();
  detsCrossedByMuonDeepInside = std::vector<uint32_t>();
  detsHitsCompatibleWithMuon = std::vector<DetCluDigiObj>();
  detsSIMU = std::vector<uint32_t>();

  l1ObjColl = new L1ObjColl;

  //
  // fill muon info
  //
  muon->isUnique = theBestMuonFinder.isUnique(ev,es);
  muon->nAllMuons = theBestMuonFinder.numberOfAllMuons(ev,es);
  muon->nRPCHits = theBestMuonFinder.numberOfValidMuonRPCHits();
  muon->nDTHits  = theBestMuonFinder.numberOfValidMuonDTHits();
  muon->nCSCHits = theBestMuonFinder.numberOfValidMuonCSCHits();
  muon->nTrackerHits = theBestMuonFinder.numberOfValidTrackerHits();
  if (theMuon) {
    muon->setKine(theMuon->bestTrack()->pt(), theMuon->bestTrack()->eta(), theMuon->bestTrack()->phi(), theMuon->bestTrack()->charge());
    muon->setBits(theMuon->isGlobalMuon(), theMuon->isTrackerMuon(), theMuon->isStandAloneMuon(), theMuon->isCaloMuon(), theMuon->isMatchesValid());
    muon->nMatchedStations = theMuon->numberOfMatchedStations();
  }

  //
  // fill algoBits info
  //
  static edm::RunNumber_t lastRun = 0;
  if (ev.run() != lastRun) {
    lastRun = ev.run();
    bitsL1->names  = theMenuInspector.namesAlgoL1();
    bitsHLT->names = theMenuInspector.namesAlgoHLT();
  }
  bitsL1->firedAlgos = theMenuInspector.firedAlgosL1(ev,es);
  bitsHLT->firedAlgos = theMenuInspector.firedAlgosHLT(ev,es);
  

  //
  // hits and detectors compatible with muon track
  //
  if (theMuon &&  muon->pt() > 10. && theMuon->isGlobalMuon()) {
    detsHitsCompatibleWithMuon = theDetHitCollector.compatibleHits( theMuon, ev, es);
    detsCrossedByMuon = theDetHitCollector.compatibleDets( theMuon, ev, es, false); 
    detsCrossedByMuonDeepInside = theDetHitCollector.compatibleDets( theMuon, ev, es, true); 
    if (theConfig.getParameter<bool>("checkDestSIMU")) detsSIMU = theDetHitCollector.compatibleSIMU( theMuon, ev, es);
  }

  
  //
  // fill LinkSynchroAnalysis data
  //
  if (theMuon && theMuon->isGlobalMuon()) {
    theSynchroGrabber.setMuon(theMuon);
    RPCRawSynchro::ProdItem rawCounts  = theSynchroGrabber.counts(ev,es);
    counts = ConverterRPCRawSynchroSynchroCountsObj::toSynchroObj(rawCounts);
  }
  

  //
  // fill L1 info
  //
  std::vector<L1Obj> l1Obj = theL1ObjMaker(ev);
  l1ObjColl->set( l1Obj);
  std::vector<bool> matching(l1Obj.size(), false);
  std::vector<double> deltaR(l1Obj.size(), 999.);
  TrackToL1ObjMatcher matcher(theConfig.getParameter<edm::ParameterSet>("matcherPSet"));
  if (theMuon && theMuon->isGlobalMuon()){
    for(unsigned int i=0; i< l1Obj.size(); ++i) {
      if (matcher(l1Obj[i].eta, l1Obj[i].phi, theMuon, ev,es)) matching[i]=true;
      TrackToL1ObjMatcher::LastResult result = matcher.lastResult();
      deltaR[i] = sqrt( sqr(result.deltaEta) + sqr(result.deltaPhi) );
    }
  }
  l1ObjColl->set( matching );
  l1ObjColl->set( deltaR );
//  std::cout <<"----------"<<std::endl;
//  std::cout <<*l1ObjColl << std::endl;

  //
  // fill ntuple + cleanup
  //
  //std::cout <<" THIS event written!" << std::endl;
  theTree->Fill();
  delete event; event = 0;
  delete muon;  muon = 0;
  delete track; track = 0;
  delete bitsL1;  bitsL1= 0;
  delete bitsHLT;  bitsHLT= 0;
  delete l1ObjColl; l1ObjColl = 0;
}
				
