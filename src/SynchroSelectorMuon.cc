#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroSelectorMuon.h"


#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/MuonReco/interface/MuonFwd.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"

#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"

#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "TObjArray.h"
#include "TH1F.h"
#include <cmath>
using namespace edm;
using namespace std;

SynchroSelectorMuon::SynchroSelectorMuon(const edm::ParameterSet& cfg) : SynchroSelector(cfg) 
{ }

SynchroSelectorMuon::SynchroSelectorMuon(const edm::ParameterSet& cfg, TObjArray& histos) 
  : SynchroSelector(cfg)
{ initHistos(histos); }

void SynchroSelectorMuon::initHistos(TObjArray& histos)
{
  hDxy   = new TH1F("hDxy_SSMuon","hDxy_SSMuon",100.,0.,1.); histos.Add(hDxy);
  SynchroSelector::hPullX = new TH1F("hPullX_Muon","hPullX_Muon",100,-10.,10.); histos.Add(hPullX);
  SynchroSelector::hDistX = new TH1F("hDistX_Muon","hDistX_Muon",100,-100.,100.); histos.Add(hDistX);
}

bool SynchroSelectorMuon::takeIt(const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es)
{

  math::XYZPoint reference(0.,0.,0.);
  if (theConfig.exists("beamSpot")) {
    edm::InputTag beamSpot =  theConfig.getParameter<edm::InputTag>("beamSpot");
    edm::Handle<reco::BeamSpot> bsHandle;
    ev.getByLabel( beamSpot, bsHandle);
    if (bsHandle.isValid()) reference = math::XYZPoint(bsHandle->x0(), bsHandle->y0(), bsHandle->z0());
  }

  edm::Handle<reco::MuonCollection> muons;
  ev.getByLabel( theConfig.getParameter<std::string>("muonColl"), muons);
  const reco::Muon * theMuon = 0;
  for (reco::MuonCollection::const_iterator im = muons->begin(); im != muons->end(); ++im) {
   if (!im->isTrackerMuon()) continue;
   if (!im->isStandAloneMuon()) continue;
   if (!im->isGlobalMuon()) continue;
   if (im->track()->pt() < theConfig.getParameter<double>("minPt")) continue;
   if (fabs(im->track()->eta()) >  theConfig.getParameter<double>("maxEta")) continue;
   if (im->track()->dxy(reference) >  theConfig.getParameter<double>("maxTIP")) continue;
   if (!theMuon || (im->track()->pt() > theMuon->track()->pt()) ) theMuon = &(*im);
  }

  if (!theMuon) return false;

  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  edm::ESHandle<MagneticField> magField;
  es.get<IdealMagneticFieldRecord>().get(magField);

  if (hDxy) hDxy->Fill(fabs(theMuon->innerTrack()->dxy(reference)));
  TrajectoryStateOnSurface aTSOS = trajectoryStateTransform::innerStateOnSurface( *(theMuon->outerTrack()), *globalGeometry, magField.product());
  return checkTraj(aTSOS, det,ev,es);
}

bool SynchroSelectorMuon::checkTraj(TrajectoryStateOnSurface & aTSOS, const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es)
{
  if (!checkRpcDetMatching(aTSOS,det,ev,es)) return false;
  if (theConfig.getParameter<bool>("checkUniqueRecHitMatching") && !checkUniqueRecHitMatching(aTSOS,det,ev,es)) return false;
  return true;
} 



