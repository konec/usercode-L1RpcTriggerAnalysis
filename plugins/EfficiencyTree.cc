#include "EfficiencyTree.h"
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"


#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjMaker.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackToL1ObjMatcher.h"
#include "DataFormats/Math/interface/deltaR.h"


#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/RPCGeometry/interface/RPCGeomServ.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"
#include "DataFormats/DetId/interface/DetIdCollection.h"
#include "TH1F.h"

template <class T> T sqr( T t) {return t*t;}

EfficiencyTree::EfficiencyTree(const edm::ParameterSet& cfg)
  : theConfig(cfg), theTree(0), event(0), muon(0), track(0), l1RpcColl(0) , l1OtherColl(0)
{ }

void EfficiencyTree::beginJob()
{
  theFile = new TFile(theConfig.getParameter<std::string>("treeFileName").c_str(),"RECREATE");
  theTree = new TTree("tL1Rpc","L1RpcEfficciency");

  theTree->Branch("event","EventObj",&event,32000,99);

  theTree->Branch("muon","MuonObj",&muon,32000,99);
  theTree->Branch("track", "TrackObj",&track,32000,99);

  theTree->Branch("hitBarrel",&hitBarrel);
  theTree->Branch("hitEndcap",&hitEndcap);
  theTree->Branch("detBarrel",&detBarrel);
  theTree->Branch("detEndcap",&detEndcap);

//   theTree->Branch("validRPCHitsBarrel",&validRPCHitsBarrel);
//   theTree->Branch("validRPCHitsEndcap",&validRPCHitsEndcap);
//   theTree->Branch("validDTHitsBarrel",&validDTHitsBarrel);
//   theTree->Branch("validDTHitsEndcap",&validDTHitsEndcap);
//   theTree->Branch("validCSCHitsBarrel",&validCSCHitsBarrel);
//   theTree->Branch("validCSCHitsEndcap",&validCSCHitsEndcap);



  theTree->Branch("l1RpcColl","L1ObjColl",&l1RpcColl,32000,99);
  theTree->Branch("l1OtherColl","L1ObjColl",&l1OtherColl,32000,99);

  histos.SetOwner();
  hPullX_Sta = new TH1F("hPullX_Sta","PullX fro STA muon",100,-10.,10.);  histos.Add(hPullX_Sta);
  hPullY_Sta = new TH1F("hPullY_Sta","PullY fro STA muon",100,-10.,10.);  histos.Add(hPullY_Sta); 
  hPullX_Tk  = new TH1F("hPullX_Tk","PullX fro Tk muon ",100,-10.,10.); histos.Add(hPullX_Tk); 
  hPullY_Tk  = new TH1F("hPullY_Tk","PullY fro Tk muon",100,-10.,10.); histos.Add(hPullY_Tk);

  hDeltaR_Sta  = new TH1F("hDeltaR_Sta"," deltaR for Sta muon",100,0.,1.); histos.Add( hDeltaR_Sta);
  hDeltaR_TkMu = new TH1F("hDeltaR_TkMu"," deltaR for Tk  muon",100,0.,1.); histos.Add( hDeltaR_TkMu);
  hDeltaR_TkTk = new TH1F("hDeltaR_TkTk"," deltaR for Tk track ",100,0.,1.); histos.Add( hDeltaR_TkTk);
  
  hPropToDetDeltaR = new TH1F("hPropToDetDeltaR","DR good Mu-DU before propagation",100,0.,3.);  histos.Add(hPropToDetDeltaR);
}

void EfficiencyTree::endJob()
{
   theFile->Write();
   delete theFile;

  std::string histoFile = theConfig.getParameter<std::string>("histoFileName");
  TFile f(histoFile.c_str(),"RECREATE");
  histos.Write();
  f.Close();

}

void EfficiencyTree::analyze(const edm::Event &ev, const edm::EventSetup &es)
{
  //clean event
  event = new EventObj;
  event->bx = ev.bunchCrossing();
  event->orbit = ev.orbitNumber();
  event->time = ev.time().value();
  event->id = ev.id().event();
  event->run = ev.run();
  event->lumi = ev.luminosityBlock();

  muon = new MuonObj();
  track = new TrackObj();

  hitBarrel = std::vector<bool>(6,false);
  hitEndcap = std::vector<bool>(3,false);
  detBarrel = std::vector<unsigned int>(6,0);
  detEndcap = std::vector<unsigned int>(3,0);

  l1RpcColl = new L1ObjColl;
  l1OtherColl = new L1ObjColl;

  //getBeamSpot, 
  math::XYZPoint reference(0.,0.,0.);
  edm::InputTag beamSpot =  theConfig.getParameter<edm::InputTag>("beamSpot");
  edm::Handle<reco::BeamSpot> bsHandle;
  ev.getByLabel( beamSpot, bsHandle);
  if (bsHandle.isValid()) reference = math::XYZPoint(bsHandle->x0(), bsHandle->y0(), bsHandle->z0());

  //geometry, magfield
  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);

   //get Muon
  edm::Handle<reco::MuonCollection> muons;
  ev.getByLabel( theConfig.getParameter<std::string>("muonColl"), muons);
  const reco::Muon * theMuon = 0;
  for (reco::MuonCollection::const_iterator im = muons->begin(); im != muons->end(); ++im) {
   if (!im->isTrackerMuon()) continue;
   if (im->track()->pt() < theConfig.getParameter<double>("minPt")) continue;
   if (fabs(im->track()->eta()) >  theConfig.getParameter<double>("maxEta")) continue;
   if (im->track()->dxy(reference) >  theConfig.getParameter<double>("maxTIP")) continue;

   //remove muons without valid hits in tk and mu system
   int nTrackerHits =-99;
   int nRPCHits =-99;
   int nDTHits =-99;
   int nCSCHits =-99;
   if(im->isGlobalMuon()){
     const reco::HitPattern& hp = (im->globalTrack())->hitPattern();
     nTrackerHits = hp.numberOfValidTrackerHits();
   } 
   if(im->isStandAloneMuon()){
     const reco::HitPattern& hp = (im->standAloneMuon())->hitPattern();
     nRPCHits = hp.numberOfValidMuonRPCHits();
     nDTHits = hp.numberOfValidMuonDTHits();
     nCSCHits = hp.numberOfValidMuonCSCHits();
   }
   std::cout<<" PRZED spr"
	    <<" Hits:Tk "<< nTrackerHits
	    <<" RPC "<< nRPCHits
	    <<" DT "<< nDTHits
	    <<" CSC "<< nCSCHits
	    <<" pt,eta,phi "<< im->track()->pt()<<","<<im->track()->eta()<<","<<im->track()->phi()
	    <<" charge "<< im->track()->charge()
	    <<std::endl;

   std::cout <<"i-mu muon  (pt,eta,phi) "<<im->track()->pt()<<" "<< im->track()->eta() <<" "<< im->track()->phi() << std::endl;

   if(im->innerTrack().isNonnull())
     std::cout <<"i-mu inner (pt,p,eta,phi) "<<im->innerTrack()->pt()<<" "<<im->innerTrack()->p()
	       <<" "<<im->innerTrack()->eta()<<" "<<im->innerTrack()->phi()<<std::endl;
   else
     std::cout <<"i-mu inner brak "
	    <<" Hits:Tk "<< nTrackerHits
	    <<" RPC "<< nRPCHits
	    <<" DT "<< nDTHits
	    <<" CSC "<< nCSCHits
	    <<std::endl;


   if(im->outerTrack().isNonnull())
     std::cout <<"i-mu outer (pt,p,eta,phi) "<<im->outerTrack()->pt()<<" "<<im->outerTrack()->p()
	       <<" "<<im->outerTrack()->eta()<<" "<<im->outerTrack()->phi()<<std::endl;
   else
     std::cout <<"i-mu outer brak "
	       <<" Hits:Tk "<< nTrackerHits
	       <<" RPC "<< nRPCHits
	       <<" DT "<< nDTHits
	       <<" CSC "<< nCSCHits
	       <<std::endl;
   

   if(im->globalTrack().isNonnull())
     std::cout <<"i-mu globa (pt,p,eta,phi) "<<im->globalTrack()->pt()<<" "<<im->globalTrack()->p()
	       <<" "<<im->globalTrack()->eta()<<" "<<im->globalTrack()->phi()<<std::endl;
   else
     std::cout <<"i-mu globa brak "
	       <<" Hits:Tk "<< nTrackerHits
	       <<" RPC "<< nRPCHits
	       <<" DT "<< nDTHits
	       <<" CSC "<< nCSCHits
	       <<std::endl;
   
   
   if (nTrackerHits<=0) continue;
   if ( nRPCHits==0 &&nDTHits==0 &&nCSCHits==0  ) continue;

//    std::cout<<" Po    spr"
// 	    <<" Hits:Tk "<< nTrackerHits
// 	    <<" RPC "<< nRPCHits
// 	    <<" DT "<< nDTHits
// 	    <<" CSC "<< nCSCHits
// 	    <<" pt,eta,phi "<< im->track()->pt()<<","<<im->track()->eta()<<","<<im->track()->phi()
// 	    <<std::endl;



   if (!theMuon || (im->track()->pt() > theMuon->track()->pt()) ) theMuon = &(*im); 
  }
   // best RPC muon
  TrajectoryStateOnSurface tsos;
  if (theMuon) { 
    muon->setKine(theMuon->track()->pt(), theMuon->track()->eta(), theMuon->track()->phi(), theMuon->track()->charge());
    muon->setBits(theMuon->isGlobalMuon(), theMuon->isTrackerMuon(), theMuon->isStandAloneMuon(),  theMuon->isCaloMuon(), theMuon->isMatchesValid());

/*
    std::cout <<"Is mached valid: "<< theMuon->isMatchesValid();
    std::cout <<" inner (pt,eta,phi) "<<  muon->pt()<<" "<<muon.eta<<" "<< muon->phi()<< std::endl;
    std::cout <<" outer (pt,eta,phi) "<<theMuon->outerTrack()->pt()<<" "<<theMuon->outerTrack()->eta()<<" "<<theMuon->outerTrack()->phi()<<std::endl;
    std::cout <<" combi (pt,eta,phi) "<<theMuon->globalTrack()->pt()<<" "<<theMuon->globalTrack()->eta()<<" "<<theMuon->globalTrack()->phi()<<std::endl;
*/

    // get muon tsos for muon matching to RPC
    TrajectoryStateOnSurface muTSOS;
//     if (theMuon->isStandAloneMuon()) { 
//        muTSOS = TrajectoryStateTransform().innerStateOnSurface(*(theMuon->outerTrack()), *globalGeometry, magField.product());
//     } else {
//        muTSOS = TrajectoryStateTransform().outerStateOnSurface(*(theMuon->track()), *globalGeometry, magField.product());
//     }

    muTSOS = TrajectoryStateTransform().innerStateOnSurface(*(theMuon->track()), *globalGeometry, magField.product());

    tsos = muTSOS;


    //rpc Hits compatible with muon tsts
    edm::Handle<RPCRecHitCollection> recHits;
    ev.getByLabel("rpcRecHits", recHits);
    edm::ESHandle<RPCGeometry> rpcGeometry;
    es.get<MuonGeometryRecord>().get(rpcGeometry);
    edm::ESHandle<Propagator> propagator;
    //    es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAlong", propagator);
    es.get<TrackingComponentsRecord>().get("SteppingHelixPropagatorAny", propagator);
    
    typedef RPCRecHitCollection::const_iterator IH;
    for (IH ih=recHits->begin(); ih != recHits->end(); ++ih) {
      RPCDetId rpcDet = ih->rpcId();
      LocalPoint hitPoint = ih->localPosition();
      LocalError hitError = ih->localPositionError();
      TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(muTSOS, globalGeometry->idToDet(rpcDet)->surface());
      if (!trackAtRPC.isValid()) continue;
      LocalPoint trackAtRPCPoint = trackAtRPC.localPosition() ;
      LocalError trackAtRPCError = trackAtRPC.localError().positionError();
      float distX = hitPoint.x()-trackAtRPCPoint.x();
      float distY = hitPoint.y()-trackAtRPCPoint.y();
      float pullX = distX/ sqrt( trackAtRPCError.xx()+hitError.xx());
      float pullY = distY/ sqrt( trackAtRPCError.yy()+hitError.yy());
//       if(theMuon->isStandAloneMuon()) hPullX_Sta->Fill(pullX); else  hPullX_Tk->Fill(pullX);
//       if(theMuon->isStandAloneMuon()) hPullY_Sta->Fill(pullY); else  hPullY_Tk->Fill(pullY);
      if(theMuon->isStandAloneMuon()) hPullX_Sta->Fill(pullX);
      hPullX_Tk->Fill(pullX);
      if(theMuon->isStandAloneMuon()) hPullY_Sta->Fill(pullY); 
      hPullY_Tk->Fill(pullY);

      std::cout<< "i-mu pull x,y"<< pullX<< ","<<pullY<<std::endl;


      bool hitCompatible = (fabs(pullX) < 3.5 && fabs(pullY) < 3.5 );
      if (hitCompatible) {
        int region = rpcDet.region();
        if (region==0) {
          int sM = rpcDet.station();
          int sI = rpcDet.layer(); 
          int layer = (sM <=2)? 2*(sM-1)+sI : sM+2;
          hitBarrel[layer-1]=true;
	  std::cout<< "hitCompatible: i-mu pull x,y"<< pullX<< ","<<pullY<<" region "<<region<< " "<<layer<<std::endl;
        } else {
          hitEndcap[rpcDet.station()-1]=true;
	  std::cout<< "hitCompatible: i-mu pull x,y"<< pullX<< ","<<pullY<<" region "<<region<< " "<<rpcDet.station()-1<<std::endl;
        }
      
      }
    }

    //rpc dets compatible with muon
    const TrackingGeometry::DetIdContainer & detIds = rpcGeometry->detIds();
    if ( (fabs(muon->eta()) > 1.24 && muon->pt() > 3.5) || (muon->pt() >5.) ) {
    for (TrackingGeometry::DetIdContainer::const_iterator it = detIds.begin(); it != detIds.end(); ++it) {
      const GeomDet * det = rpcGeometry->idToDet(*it);
      GlobalPoint detPosition = det->position();
      if (deltaR(muon->eta(), muon->phi(), detPosition.eta(), detPosition.phi()) > 1.1) continue;
      TrajectoryStateOnSurface trackAtRPC =  propagator->propagate(muTSOS, det->surface());
      if (!trackAtRPC.isValid()) continue;
      if (! (det->surface().bounds().inside(trackAtRPC.localPosition()))) continue;
      hPropToDetDeltaR->Fill(deltaR(muon->eta(), muon->phi(), detPosition.eta(), detPosition.phi()));
/*
         std::cout << " **** HERE PROBLEM  "<< std::endl;  else   std::cout << " **** HERE OK "<< std:: endl; 

         std::cout <<" is valid: "<< trackAtRPC.isValid() <<" is inside: "<< det->surface().bounds().inside(trackAtRPC.localPosition()) << std::endl;
         std::cout <<" dR= "<<deltaR(muon->eta(), muon->phi(), detPosition.eta(), detPosition.phi())<<std::endl; 
         std::cout <<" DetPosition: (eta: "<<detPosition.eta()<<",phi: "<< detPosition.phi()<<",r: "<< detPosition.perp()<<",z :"<< detPosition.z()<<")"<<std::endl; 
         std::cout <<" Tk Position  (eta:"<<trackAtRPC.globalPosition().eta()<<", phi; "<<trackAtRPC.globalPosition().phi()<<", r: "<<trackAtRPC.globalPosition().perp()<<", z"<<trackAtRPC.globalPosition().z()<<")"<<std::endl;
         std::cout <<" local Tk Position:"<< trackAtRPC.localPosition();
         std::cout << std::endl;
*/
      
      RPCDetId rpcDet(*it);
      int region = rpcDet.region();
      if (region==0) {
        int sM = rpcDet.station();
        int sI = rpcDet.layer();
        int layer = (sM <=2)? 2*(sM-1)+sI : sM+2;
        detBarrel[layer-1]++;
      } else {
        detEndcap[rpcDet.station()-1]++;
      }
    } 
    }
  }

  //create L1muon helper 
  edm::InputTag l1Tag(theConfig.getParameter<edm::InputTag>("l1MuReadout"));
  L1ObjMaker l1(l1Tag,ev);

  // set L1Others
  std::vector<L1Obj> l1Others=  l1(L1ObjMaker::DT,L1ObjMaker::CSC);
  std::vector<bool> l1OthersMatching(l1Others.size(),false);

  edm::Handle<reco::TrackCollection> tracks;
  ev.getByLabel( theConfig.getParameter<std::string>("trackColl"), tracks);
  typedef reco::TrackCollection::const_iterator IT;
  for (IT it = tracks->begin(); it !=tracks->end(); ++it) {
    const reco::Track & muTrack = *it;
    if (muTrack.pt() < theConfig.getParameter<double>("minPt")) continue;
    if (muTrack.dxy(reference) >  theConfig.getParameter<double>("maxTIP")) continue;
    if (fabs(muTrack.eta()) >  theConfig.getParameter<double>("maxEta")) continue;
    if (muTrack.pt() < track->pt()) continue;
    TrajectoryStateOnSurface tTSOS = TrajectoryStateTransform().outerStateOnSurface(muTrack, *globalGeometry, magField.product());
    TrackToL1ObjMatcher matcher(theConfig.getParameter<edm::ParameterSet>("dtcscMatcherPSet"));
// sprawdzic matchowanie
    bool isTrackCompatible = false;
    std::vector<bool> l1OthersThisTrackMatching(l1Others.size(),false);

    for (unsigned int i=0; i< l1Others.size(); ++i) {
      if ( matcher(l1Others[i].eta, l1Others[i].phi, tTSOS, ev,es) ) {
        isTrackCompatible = true; 
        l1OthersThisTrackMatching[i]=true;
      }
    }
    if (isTrackCompatible) {
      if (!theMuon) tsos = tTSOS;
      track->setKine( muTrack.pt(), muTrack.eta(), muTrack.phi(), muTrack.charge());
      l1OthersMatching = l1OthersThisTrackMatching;
    }
  }
  l1OtherColl->set(l1Others);
  l1OtherColl->set(l1OthersMatching);


  // set L1 RPC 
  std::vector<L1Obj> l1Rpcs = l1(L1ObjMaker::RPCB,L1ObjMaker::RPCF);
  std::vector<bool> l1RpcsMatching(l1Rpcs.size(), false);
  TrackToL1ObjMatcher matcher(theConfig.getParameter<edm::ParameterSet>("rpcMatcherPSet"));
  double minDeltaR = 100.;
  if (tsos.isValid()) for (unsigned int i=0; i< l1Rpcs.size(); ++i) {
     if (matcher(l1Rpcs[i].eta, l1Rpcs[i].phi, tsos, ev,es)) l1RpcsMatching[i]=true;
     double dR = sqrt( sqr(matcher.lastResult().deltaEta) + sqr(matcher.lastResult().deltaPhi)); 
     if (dR < minDeltaR) minDeltaR = dR;
  }
  l1RpcColl->set(l1Rpcs);
  l1RpcColl->set(l1RpcsMatching);
  if (theMuon && theMuon->isStandAloneMuon()) hDeltaR_Sta->Fill(minDeltaR); 
  else if (theMuon)  hDeltaR_TkMu->Fill(minDeltaR); 
  else   hDeltaR_TkTk->Fill(minDeltaR); 


  // fill ntuple and cleanup
  if (tsos.isValid() || l1Rpcs.size() != 0 ||  l1Others.size() !=0) theTree->Fill();
  delete event; event = 0;
  delete muon;  muon = 0;
  delete track; track = 0;
  delete l1RpcColl; l1RpcColl = 0;
  delete l1OtherColl; l1OtherColl = 0;

}
