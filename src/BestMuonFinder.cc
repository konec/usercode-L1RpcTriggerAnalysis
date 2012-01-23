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
#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"

//TH2D* hMuHitsCSCvsEta;

BestMuonFinder::BestMuonFinder(const edm::ParameterSet& cfg)
  : lastEvent(0), lastRun(0), theConfig(cfg), theUnique(true), theMuon(0),
    hMuChi2Tk(0), hMuChi2Gl(0), hMuNHitsTk(0), 
    hMuPtVsEta(0), hMuHitsRPCvsCSC(0), hMuHitsRPCvsDT(0)
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
  
  for (reco::MuonCollection::const_iterator im = muons->begin(); im != muons->end(); ++im) {

    if (!im->isTrackerMuon() || !im->innerTrack().isNonnull()) continue;
    if (hMuPtVsEta) hMuPtVsEta->Fill(im->innerTrack()->eta(), im->innerTrack()->pt());
    if (im->innerTrack()->pt() < theConfig.getParameter<double>("minPt")) continue;
    if (fabs(im->innerTrack()->eta()) >  theConfig.getParameter<double>("maxEta")) continue;
    if (im->innerTrack()->dxy(reference) >  theConfig.getParameter<double>("maxTIP")) continue;
    if (hMuChi2Tk) hMuChi2Tk->Fill(im->innerTrack()->normalizedChi2());
    if (im->innerTrack()->normalizedChi2() >  theConfig.getParameter<double>("maxChi2Tk")) continue;
    if (hMuChi2Gl && im->isGlobalMuon()) hMuChi2Gl->Fill(im->combinedMuon()->normalizedChi2());
    if (    theConfig.getParameter<bool>("requireOuterTrack")){ 
        if(!im->isStandAloneMuon() || !im->outerTrack().isNonnull())continue;
	  if(im->standAloneMuon()->normalizedChi2() >  theConfig.getParameter<double>("maxChi2Sa")) continue;
    }
    if ( theConfig.getParameter<bool>("requireGlobalTrack")) { 
	  if(!im->isGlobalMuon() ||  !im->globalTrack().isNonnull()) continue;  
	  if(im->combinedMuon()->normalizedChi2() >  theConfig.getParameter<double>("maxChi2Mu")) continue;
    }

    if (im->numberOfMatchedStations() <  theConfig.getParameter<int>("minNumberOfMatchedStations")) continue;
    if (! (im->track()->hitPattern().numberOfValidPixelHits() > 0)) continue;

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
    if(im->isTrackerMuon()) {
      const reco::HitPattern& hp = (im->innerTrack())->hitPattern();
      nTrackerHits = hp.numberOfValidTrackerHits();
      if (hMuNHitsTk) hMuNHitsTk->Fill(fabs(nTrackerHits)+1.e-3);
    }
    if(im->isGlobalMuon()){
      const reco::HitPattern& hp = (im->combinedMuon())->hitPattern();
      nRPCHits = hp.numberOfValidMuonRPCHits();
      nDTHits = hp.numberOfValidMuonDTHits();
      nCSCHits = hp.numberOfValidMuonCSCHits();
    if (nDTHits==0 && hMuHitsRPCvsCSC) hMuHitsRPCvsCSC->Fill(nCSCHits,nRPCHits);
    if (nCSCHits==0 && hMuHitsRPCvsDT) hMuHitsRPCvsDT->Fill(nDTHits,nRPCHits);
    }

    if (nTrackerHits< theConfig.getParameter<int>("minNumberTrackerHits")) continue;
    if ( nRPCHits < theConfig.getParameter<int>("minNumberRpcHits")) continue;
    if ( nDTHits + nCSCHits < theConfig.getParameter<int>("minNumberDtCscHits")  ) continue;

    if (theMuon) theUnique = false;
    if (!theMuon || (im->track()->pt() > theMuon->track()->pt()) ) theMuon = &(*im);
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
}
