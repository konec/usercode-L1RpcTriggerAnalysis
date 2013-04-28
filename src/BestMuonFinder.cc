#include "UserCode/L1RpcTriggerAnalysis/interface/BestMuonFinder.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"

//TH2D* hMuHitsCSCvsEta;

BestMuonFinder::BestMuonFinder(const edm::ParameterSet& cfg)
  : lastEvent(0), lastRun(0), theConfig(cfg), theUnique(true), theAllMuons(0), theMuon(0),
    theTrackerHits(0), theRPCHits(0), theDTHits(0), theCSCHits(0),
    hMuChi2Tk(0), hMuChi2Gl(0), hMuNHitsTk(0), 
    hMuPtVsEta(0), hMuHitsRPCvsCSC(0), hMuHitsRPCvsDT(0),
    hMuonPt_BMF(0),hMuonEta_BMF (0),hMuonPhi_BMF(0)
{}

bool BestMuonFinder::run(const edm::Event &ev, const edm::EventSetup &es)
{

  if (lastEvent==ev.id().event() && lastRun==ev.run()) return false;
  lastEvent = ev.id().event() ;
  lastRun = ev.run();
  theMuon = 0;
  theUnique = true;


  //getBeamSpot
  math::XYZPoint reference(0.,0.,0.);
  edm::InputTag beamSpot =  theConfig.getParameter<edm::InputTag>("beamSpot");
  edm::Handle<reco::BeamSpot> bsHandle;
  ev.getByLabel( beamSpot, bsHandle);
  if (bsHandle.isValid()) reference = math::XYZPoint(bsHandle->x0(), bsHandle->y0(), bsHandle->z0());

  //get Muon
  edm::Handle<reco::MuonCollection> muons;
  ev.getByLabel( theConfig.getParameter<std::string>("muonColl"), muons);
  theAllMuons = muons->size();
  
  for (reco::MuonCollection::const_iterator im = muons->begin(); im != muons->end(); ++im) {

/*
if (ev.id().event() == 352597514) {
    std::cout << "HERE Muon: matched stations:"<<im->numberOfMatchedStations() <<std::endl;
    if (im->isGlobalMuon()) {
       std::cout <<"  GlobalMuon: "
                 <<" pt="<<im->combinedMuon()->pt()
                 <<" eta="<<im->combinedMuon()->eta()
                 <<" phi="<<im->combinedMuon()->phi()
                 <<" chi2: "<<im->combinedMuon()->normalizedChi2()<<std::endl;
     }
    if (im->isTrackerMuon() && im->innerTrack().isNonnull()) { 
      std::cout <<" TrackerMuon: " 
                <<" pt="<<im->innerTrack()->pt()
                <<" eta="<<im->innerTrack()->eta()
                <<" phi="<<im->innerTrack()->phi()
              << std::endl;
    }
    if (im->isStandAloneMuon())  std::cout <<" StandaloneMuon"<<std::endl;
}
*/
    if (im->bestTrack()->eta() >  theConfig.getParameter<double>("maxEta")) continue;
    if (im->bestTrack()->pt() < theConfig.getParameter<double>("minPt")) continue;
    if (im->numberOfMatchedStations() <  theConfig.getParameter<int>("minNumberOfMatchedStations")) continue;

    if (    theConfig.getParameter<bool>("requireInnerTrack")) {
      if (!im->isTrackerMuon() || !im->innerTrack().isNonnull()) continue;
      if (im->innerTrack()->dxy(reference) >  theConfig.getParameter<double>("maxTIP")) continue;
      if (im->innerTrack()->normalizedChi2() >  theConfig.getParameter<double>("maxChi2Tk")) continue;
      if (hMuonPt_BMF)   hMuonPt_BMF->Fill( im->innerTrack()->pt());
      if (hMuonEta_BMF) hMuonEta_BMF->Fill( im->innerTrack()->eta());
      if (hMuonPhi_BMF) hMuonPhi_BMF->Fill( im->innerTrack()->phi());
      if (hMuChi2Tk) hMuChi2Tk->Fill(im->innerTrack()->normalizedChi2());
      if (hMuPtVsEta) hMuPtVsEta->Fill(im->innerTrack()->eta(), im->innerTrack()->pt());
    }
    if (    theConfig.getParameter<bool>("requireOuterTrack")){ 
       if(!im->isStandAloneMuon() || !im->outerTrack().isNonnull())continue;
	 if(im->standAloneMuon()->normalizedChi2() >  theConfig.getParameter<double>("maxChi2Sa")) continue;
    }
    if ( theConfig.getParameter<bool>("requireGlobalTrack")) { 
	  if(!im->isGlobalMuon() ||  !im->globalTrack().isNonnull()) continue;  
	  if(im->combinedMuon()->normalizedChi2() >  theConfig.getParameter<double>("maxChi2Mu")) continue;
    }

    if (hMuChi2Gl && im->isGlobalMuon()) hMuChi2Gl->Fill(im->combinedMuon()->normalizedChi2());

//
// TMP TIGHT SELECTION FOR IVAN
//  if (! im->isGlobalMuon()) continue;
//  if (! (im->globalTrack()->normalizedChi2() < 10)) continue;
//  if (! (im->globalTrack()->hitPattern().numberOfValidMuonHits() > 0)) continue;
//  if (! (im->numberOfMatchedStations() > 1)) continue;
//  if (! (fabs(im->innerTrack()->dxy(reference)) < 0.2)) continue; 
//  if (! (im->track()->hitPattern().numberOfValidPixelHits() > 0)) continue;
//  if (! (im->track()->hitPattern().numberOfValidTrackerHits() > 10)) continue;
// TMP END OF TIGHT MUON SELECTION FROM IVAN

    //remove muons without valid hits in tk and mu system
    int nTrackerHits =0;
    int nRPCHits =0;
    int nDTHits =0;
    int nCSCHits =0;

    if (im->isGlobalMuon()) {
      const reco::HitPattern& hp = (im->combinedMuon())->hitPattern();
      nTrackerHits = hp.numberOfValidTrackerHits();
      nRPCHits = hp.numberOfValidMuonRPCHits();
      nDTHits = hp.numberOfValidMuonDTHits();
      nCSCHits = hp.numberOfValidMuonCSCHits();
      if (hMuNHitsTk) hMuNHitsTk->Fill(fabs(nTrackerHits)+1.e-3);
      if (nDTHits==0 && hMuHitsRPCvsCSC) hMuHitsRPCvsCSC->Fill(nCSCHits,nRPCHits);
      if (nCSCHits==0 && hMuHitsRPCvsDT) hMuHitsRPCvsDT->Fill(nDTHits,nRPCHits);
    } else {
      if(im->isTrackerMuon()) {
        const reco::HitPattern& hp = (im->innerTrack())->hitPattern();
        nTrackerHits = hp.numberOfValidTrackerHits();
      }
      if (im->isStandAloneMuon()) {
        const reco::HitPattern& hp = (im->standAloneMuon())->hitPattern();
        nRPCHits = hp.numberOfValidMuonRPCHits();
        nDTHits = hp.numberOfValidMuonDTHits();
        nCSCHits = hp.numberOfValidMuonCSCHits();
      }
    }
    
    if (nTrackerHits< theConfig.getParameter<int>("minNumberTrackerHits")) continue;
    if ( nRPCHits < theConfig.getParameter<int>("minNumberRpcHits")) continue;
    if ( nDTHits + nCSCHits < theConfig.getParameter<int>("minNumberDtCscHits")  ) continue;

    if (!theMuon || (im->bestTrack()->pt() > theMuon->bestTrack()->pt()) ) {
      theMuon = &(*im);
      theTrackerHits = nTrackerHits;
      theRPCHits = nRPCHits;
      theDTHits = nDTHits;
      theCSCHits = nCSCHits;
    }
  }

  //
  // check if muon is unigue
  //
  if (theMuon) {
    double muonEta = theMuon->bestTrack()->eta();
    double muonPhi = theMuon->bestTrack()->phi();
    for (reco::MuonCollection::const_iterator im = muons->begin(); im != muons->end(); ++im) {
      if (&(*im) == theMuon) continue;
      if ( fabs(reco::deltaPhi(muonPhi, im->bestTrack()->phi())) > theConfig.getParameter<double>("deltaPhiUnique")) continue; 
      if ( fabs(muonEta-im->bestTrack()->eta()) > theConfig.getParameter<double>("deltaEtaUnique")) continue;
      theUnique = false;
    }
  }
  return true;
}

void BestMuonFinder::initHistos( TObjArray & histos)
{
  hMuChi2Tk = new TH1D("hMuChi2Tk","hMuChi2Tk",100.,0.,10.); histos.Add(hMuChi2Tk);
  hMuChi2Gl = new TH1D("hMuChi2Gl","hMuChi2Gl",100.,0.,10.); histos.Add(hMuChi2Gl);
  hMuNHitsTk = new TH1D("hMuNHitsTk","hMuNHitsTk",30,0.,30.); histos.Add(hMuNHitsTk);
  hMuPtVsEta = new TH2D("hMuPtVsEta","hMuPtVsEta", L1RpcEtaScale::nEtaBins, L1RpcEtaScale::etaBins, L1PtScale::nPtBins, L1PtScale::ptBins); histos.Add(hMuPtVsEta);
  hMuHitsRPCvsCSC = new TH2D("hMuHitsRPCvsCSC","hMuHitsRPCvsCSC", 40,0.,40., 6,0.,6.); histos.Add(hMuHitsRPCvsCSC);
  hMuHitsRPCvsDT  = new TH2D("hMuHitsRPCvsDT","hMuHitsRPCvsDT", 30,0.,60., 6,0.,6.); histos.Add(hMuHitsRPCvsDT);
//  hMuHitsCSCvsEta = new TH2D("hMuHitsCSCvsEta","hMuHitsCSCvsEta",17, 0.8,2.5, 40,0.,40); histos.Add(hMuHitsCSCvsEta);
  hMuonPt_BMF  = new TH1D("hMuonPt_BMF","All global muons Pt;Glb.muon p_{T} [GeV];Muons / bin",L1PtScale::nPtBins,L1PtScale::ptBins);  histos.Add(hMuonPt_BMF);
  hMuonEta_BMF = new TH1D("hMuonEta_BMF","All global muons Eta;Glb.muon #eta;Muons / bin",84, -2.1, 2.1);  histos.Add(hMuonEta_BMF);
  hMuonPhi_BMF = new TH1D("hMuonPhi_BMF","All global muons Phi;Glb.muon #phi [rad];Muons / bin",90,-M_PI,M_PI);  histos.Add(hMuonPhi_BMF);

}
