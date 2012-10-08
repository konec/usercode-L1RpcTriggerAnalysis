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
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjMakerRpcEmu.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackToL1ObjMatcher.h"
#include "DataFormats/RPCDigi/interface/RPCRawSynchro.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/ConverterRPCRawSynchroSynchroCountsObj.h"


#include "TFile.h"
#include "TTree.h"

template <class T> T sqr( T t) {return t*t;}


L1RpcTreeMaker::L1RpcTreeMaker(const edm::ParameterSet& cfg)
  : theConfig(cfg), theTree(0), event(0), muon(0), track(0), counts(0), 
    l1RpcColl(0) , l1OtherColl(0), l1RpcCollEmu(0),
    theCounter(0),
    theBestMuonFinder(cfg.getParameter<edm::ParameterSet>("bestMuonFinder")),
    theDetHitCollector(cfg.getParameter<edm::ParameterSet>("detHitCollector")),
    theSynchroGrabber(cfg.getParameter<edm::ParameterSet>("linkSynchroGrabber"))
{ }

void L1RpcTreeMaker::beginRun( const edm::Run &ru, const edm::EventSetup &es)
{
}

void L1RpcTreeMaker::beginJob()
{
  theFile = new TFile(theConfig.getParameter<std::string>("treeFileName").c_str(),"RECREATE");
  theTree = new TTree("tL1Rpc","L1RpcEfficciency");

  theTree->Branch("event","EventObj",&event,32000,99);
  theTree->Branch("muon","MuonObj",&muon,32000,99);
  theTree->Branch("track", "TrackObj",&track,32000,99);

  theTree->Branch("counts",&counts);
  theTree->Branch("detsCrossedByMuon",&detsCrossedByMuon);
  theTree->Branch("detsCrossedByMuonDeepInside",&detsCrossedByMuonDeepInside);
  theTree->Branch("detsHitsCompatibleWithMuon",&detsHitsCompatibleWithMuon);
  theTree->Branch("detsSIMU",&detsSIMU);
  theTree->Branch("nDigisCompDets",&nDigisCompDets);
  theTree->Branch("clSizeCompDets",&clSizeCompDets);

  theTree->Branch("l1RpcColl","L1ObjColl",&l1RpcColl,32000,99);
  theTree->Branch("l1RpcCollEmu","L1ObjColl",&l1RpcCollEmu,32000,99);
  theTree->Branch("l1OtherColl","L1ObjColl",&l1OtherColl,32000,99);

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
  if (!theMuon || !theBestMuonFinder.isUnique(ev,es) ) return;
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
//  counts = new SynchroCountsObjVect;
  counts = std::vector<SynchroCountsObj>();
  detsCrossedByMuon = std::vector<uint32_t>();
  detsCrossedByMuonDeepInside = std::vector<uint32_t>();
  detsHitsCompatibleWithMuon = std::vector<uint32_t>();
  detsSIMU = std::vector<uint32_t>();
  nDigisCompDets = std::vector<uint32_t>();
  clSizeCompDets = std::vector<uint32_t>();
  l1RpcColl = new L1ObjColl;
  l1OtherColl = new L1ObjColl;
  l1RpcCollEmu = new L1ObjColl;

  //
  // fill muon info
  //
  muon->setKine(theMuon->track()->pt(), theMuon->track()->eta(), theMuon->track()->phi(), theMuon->track()->charge());
  muon->setBits(theMuon->isGlobalMuon(), theMuon->isTrackerMuon(), theMuon->isStandAloneMuon(), theMuon->isCaloMuon(), theMuon->isMatchesValid());
  muon->nMatchedStations = theMuon->numberOfMatchedStations();
  if (theMuon->isGlobalMuon()) {
    const reco::HitPattern& hp = (theMuon->combinedMuon())->hitPattern();
    muon->nRPCHits = hp.numberOfValidMuonRPCHits();
    muon->nDTHits  = hp.numberOfValidMuonDTHits();
    muon->nCSCHits = hp.numberOfValidMuonCSCHits();
  } else  muon->nRPCHits = muon->nDTHits = muon->nCSCHits = 0;
  muon->nTrackerHits = theMuon->isTrackerMuon() ? (theMuon->innerTrack())->hitPattern().numberOfValidTrackerHits() : 0;


  //
  // hits and detectors compatible with muon track
  //
  if ( muon->pt() > 10.) {
    detsHitsCompatibleWithMuon = theDetHitCollector.compatibleHits( theMuon, ev, es); 
    detsCrossedByMuon = theDetHitCollector.compatibleDets( theMuon, ev, es, false); 
    detsCrossedByMuonDeepInside = theDetHitCollector.compatibleDets( theMuon, ev, es, true); 
    if (theConfig.getParameter<bool>("checkDestSIMU")) detsSIMU = theDetHitCollector.compatibleSIMU( theMuon, ev, es);
    nDigisCompDets = theDetHitCollector.nDigisCompDets(detsHitsCompatibleWithMuon, ev, es);
    clSizeCompDets = theDetHitCollector.clSizeCompDets(detsHitsCompatibleWithMuon, ev, es);

    for (uint32_t i=0; i< nDigisCompDets.size(); i++) {
      if (clSizeCompDets[i] > nDigisCompDets[i]) std::cout <<" PROBLEM, event: "<<theCounter<<" cl:"<<clSizeCompDets[i]<<" nDigis:"<<nDigisCompDets[i]<<std::endl;
    }
  }

  
  //
  // fill LinkSynchroAnalysis data
  //
  theSynchroGrabber.setMuon(theMuon);
  RPCRawSynchro::ProdItem rawCounts  = theSynchroGrabber.counts(ev,es);
  counts = ConverterRPCRawSynchroSynchroCountsObj::toSynchroObj(rawCounts);
  

  //
  // fill L1 RPCemu
  //
  if (theConfig.exists("l1RpcEmu") ) {
    TrackToL1ObjMatcher matcher(theConfig.getParameter<edm::ParameterSet>("matcherPSet"));
    L1ObjMakerRpcEmu l1RpcsFromEmu( theConfig.getParameter<edm::InputTag>("l1RpcEmu"), ev);
    std::vector<L1Obj> l1RpcsEmu = l1RpcsFromEmu();
    std::vector<bool> l1RpcsEmuMatching(l1RpcsEmu.size(), false);
    for(unsigned int i=0; i< l1RpcsEmu.size(); ++i) if (matcher(l1RpcsEmu[i].eta, l1RpcsEmu[i].phi, theMuon, ev,es)) l1RpcsEmuMatching[i]=true; 
    l1RpcCollEmu->set(l1RpcsEmu);
    l1RpcCollEmu->set(l1RpcsEmuMatching);
  }

  //
  // fill L1 objects (Rpc,Oth)
  //
  L1ObjMaker l1( theConfig.getParameter<edm::InputTag>("l1MuReadout"), ev);
  TrackToL1ObjMatcher matcher(theConfig.getParameter<edm::ParameterSet>("matcherPSet"));
  std::vector<L1Obj> l1Rpcs = l1(L1ObjMaker::RPCB,L1ObjMaker::RPCF);
  std::vector<L1Obj> l1Others = l1(L1ObjMaker::DT,L1ObjMaker::CSC);
  std::vector<bool> l1RpcsMatching(l1Rpcs.size(), false);
  std::vector<bool> l1OthersMatching(l1Others.size(), false);
  for(unsigned int i=0; i< l1Rpcs.size(); ++i) if (matcher(l1Rpcs[i].eta, l1Rpcs[i].phi, theMuon, ev,es)) l1RpcsMatching[i]=true; 
  for(unsigned int i=0; i< l1Others.size(); ++i) if (matcher(l1Others[i].eta, l1Others[i].phi, theMuon, ev,es)) l1OthersMatching[i]=true; 
  l1RpcColl->set(l1Rpcs);
  l1RpcColl->set(l1RpcsMatching);
  l1OtherColl->set(l1Others);
  l1OtherColl->set(l1OthersMatching);

//  std::cout <<"RPCColl:"<<std::endl; l1RpcColl->print();
//  std::cout <<"RPCCollEmu:"<<std::endl; l1RpcCollEmu->print();

  //
  // fill ntuple + cleanup
  //
  //std::cout <<" THIS event written!" << std::endl;
  theTree->Fill();
  delete event; event = 0;
  delete muon;  muon = 0;
  delete track; track = 0;
  delete l1RpcColl; l1RpcColl = 0;
  delete l1OtherColl; l1OtherColl = 0;
  delete l1RpcCollEmu; l1RpcCollEmu = 0;
}
				
