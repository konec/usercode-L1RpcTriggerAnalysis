#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroSelectorTrack.h"


#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"

#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"


#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/TrackToL1ObjMatcher.h"
#include "TObjArray.h"
#include "TH1F.h"

#include "TH1.h"
#include <cmath>
using namespace edm;
using namespace std;

SynchroSelectorTrack::SynchroSelectorTrack(const edm::ParameterSet& cfg, TObjArray& histos)
  : SynchroSelector(cfg)
{
  hDxy   = new TH1F("hDxy_SSTrack","hDxy_SSTrack",100.,0.,1.); histos.Add(hDxy);
  SynchroSelector::hPullX = new TH1F("hPullX_SSTrack","hPullX_SSTrack",100,-10.,10.); histos.Add(hPullX);
  SynchroSelector::hDistX = new TH1F("hDistX_SSTrack","hDistX_SSTrack",100,-100.,100.); histos.Add(hDistX);
  hDeltaPhi = new TH1F("hDeltaPhi_SSTrack","hDeltaPhi_SSTrack",100, 0., 1.); histos.Add(hDeltaPhi);
  hDeltaEta = new TH1F("hDeltaEta_SSTrack","hDeltaEta_SSTrack",100, 0., 1.);  histos.Add(hDeltaEta);
  hNum = new TH1F("hNumTracks","hNumTracks",50,0.,250.); histos.Add(hNum); 
}



bool SynchroSelectorTrack::takeIt(const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es)
{
  bool result = false;
  float theMaxTIP = theConfig.getParameter<double>("maxTIP");
  float theMinPt = theConfig.getParameter<double>("minPt");

  edm::Handle<reco::TrackCollection> trackCollection;
  ev.getByLabel(InputTag( theConfig.getParameter<std::string>("collection")),trackCollection);

  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);

  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);

  math::XYZPoint reference(0.,0.,0.);
  if (theConfig.exists("beamSpot")) {
    edm::InputTag beamSpot =  theConfig.getParameter<edm::InputTag>("beamSpot");
    edm::Handle<reco::BeamSpot> bsHandle;
    ev.getByLabel( beamSpot, bsHandle);
    if (bsHandle.isValid()) reference = math::XYZPoint(bsHandle->x0(), bsHandle->y0(), bsHandle->z0());
  }

  reco::TrackCollection tracks = *(trackCollection.product());
  if (hNum) hNum->Fill(tracks.size());

  mindetaForStudy = 100.;
  mindphiForStudy = 100.;

  typedef reco::TrackCollection::const_iterator IT;
  for (IT it = tracks.begin(); it !=tracks.end(); ++it) {
    const reco::Track & track = *it;
    if (track.pt() < theMinPt ) continue;
    double dxy = track.dxy(reference);
    if (fabs(dxy) > theMaxTIP) continue;

    TrajectoryStateOnSurface aTSOS = trajectoryStateTransform::outerStateOnSurface(track, *globalGeometry, magField.product());
    if (!checkL1RpcMatching(aTSOS, ev,es) ) continue;
    if (!checkRpcDetMatching(aTSOS,det,ev,es)) continue;
    if (theConfig.getParameter<bool>("checkUniqueRecHitMatching") && !checkUniqueRecHitMatching(aTSOS,det,ev,es)) continue;

    if (hDxy) hDxy->Fill(fabs(dxy));
    result = true;
  }

  if (mindetaForStudy < 99.) hDeltaEta->Fill(mindetaForStudy);
  if (mindphiForStudy < 99.) hDeltaPhi->Fill(mindphiForStudy);

  return result;
}

bool SynchroSelectorTrack::checkL1RpcMatching( const TrajectoryStateOnSurface & tsos,  const edm::Event&ev, const edm::EventSetup& es)
{
  edm::Handle<L1MuGMTReadoutCollection> pCollection;
  InputTag l1MuReadout = theConfig.getParameter<edm::InputTag>("l1MuReadout");
  ev.getByLabel(l1MuReadout,pCollection);
  L1MuGMTReadoutCollection const* gmtrc = pCollection.product();
  if (!gmtrc) return false;

  TrackToL1ObjMatcher matcher(theConfig.getParameter<edm::ParameterSet>("rpcMatcherPSet"));

  vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();
  vector<L1MuGMTReadoutRecord>::const_iterator RRItr;
  typedef vector<L1MuRegionalCand>::const_iterator ITC;
  for( RRItr = gmt_records.begin() ; RRItr != gmt_records.end() ; RRItr++ ) {
    for (int i=1; i<=2; ++i) {
      vector<L1MuRegionalCand> Cands = (i==1) ? RRItr->getBrlRPCCands() : RRItr->getFwdRPCCands();
      for(ITC it = Cands.begin() ; it != Cands.end() ; ++it ) {
        if (it->empty()) continue;
        //propagate and check matching for candidate
        float rpcEta = it->etaValue();
        float rpcPhi = it->phiValue();
        bool matched = matcher(rpcEta, rpcPhi, tsos, ev, es);
        TrackToL1ObjMatcher::LastResult matchingResult = matcher.lastResult();
        if (matchingResult.isValid) {
          if (matchingResult.deltaEta < mindetaForStudy) mindetaForStudy = matchingResult.deltaEta; 
          if (matchingResult.deltaPhi < mindphiForStudy) mindphiForStudy = matchingResult.deltaPhi; 
        }
        if (matched) return true;
      }
    }
  }
  return false;
}




