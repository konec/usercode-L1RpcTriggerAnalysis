#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroSelectorL1Muon.h"


#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"

#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"

#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "TObjArray.h"
#include "TH1F.h"
#include <cmath>
using namespace edm;
using namespace std;


SynchroSelectorL1Muon::SynchroSelectorL1Muon(const edm::ParameterSet& cfg, TObjArray& histos)
  : SynchroSelector(cfg)
{
  hDeltaEta = new TH1F("hDeltaEta","hDeltaEta",100.,-1.,1.); histos.Add(hDeltaEta);
  hDeltaPhi = new TH1F("hDeltaPhi","hDeltaPhi",100.,-1.,1.); histos.Add(hDeltaPhi);
}

void SynchroSelectorL1Muon::update(const edm::Event&ev, const edm::EventSetup& es)
{
  theL1Muons.clear();
  edm::Handle<L1MuGMTReadoutCollection> pCollection;
  ev.getByLabel(theConfig.getParameter<edm::InputTag>("l1MuReadout"),pCollection);

  L1MuGMTReadoutCollection const* gmtrc = pCollection.product();
  if (!gmtrc) return;

  vector<L1MuGMTReadoutRecord> gmt_records = gmtrc->getRecords();

  vector<string> systems = theConfig.getParameter<vector<string> >("systems");
  
  for (vector<string>::const_iterator ic = systems.begin(); ic != systems.end(); ++ic) {
    for( vector<L1MuGMTReadoutRecord>::const_iterator it = gmt_records.begin() ; it != gmt_records.end() ; it++ ) {
      vector<L1MuRegionalCand> cands;
      if (      (*ic) == "rpcb" ) cands = it->getBrlRPCCands(); 
      else if ( (*ic) == "rpcf" ) cands = it->getFwdRPCCands();
      else if ( (*ic) == "csc" ) cands = it->getCSCCands();
      else if ( (*ic) == "dt" ) cands = it->getDTBXCands();
      for (vector<L1MuRegionalCand>::const_iterator il=cands.begin(); il < cands.end(); ++il) {
        if (!il->empty() && ( (il->bx() == 0) || !theConfig.getParameter<bool>("checkBX0") ) )theL1Muons.push_back(*il);
      }
    }
  } 
}


bool SynchroSelectorL1Muon::takeIt(const RPCDetId & det, const edm::Event&ev, const edm::EventSetup& es)
{
  bool result = false;
  edm::ESHandle<GlobalTrackingGeometry> globalGeometry;
  es.get<GlobalTrackingGeometryRecord>().get(globalGeometry);
  

  GlobalPoint detPos = globalGeometry->idToDet(det)->position();
  float detEta = detPos.eta();
  float detPhi = detPos.phi();
  for(vector<L1MuRegionalCand>::const_iterator it =theL1Muons.begin(); it != theL1Muons.end(); ++it) {
    if (it->empty()) continue;
    float l1Eta = it->etaValue();
    float l1Phi = it->phiValue();
    bool matchedDeltaPhi = fabs(deltaPhi(l1Phi,detPhi)) < theConfig.getParameter<double>("maxDeltaPhi");
    bool matchedDeltaEta = fabs(l1Eta-detEta) < theConfig.getParameter<double>("maxDeltaEta");
    if (matchedDeltaPhi) hDeltaEta->Fill(l1Eta-detEta);
    if (matchedDeltaEta) hDeltaPhi->Fill(deltaPhi(l1Phi,detPhi));
    if ( !matchedDeltaPhi ) continue;
    if ( !matchedDeltaEta ) continue;
    
//    std::cout <<" compatible det, (eta,phi,r,z)= ("<<detEta<<", "<<detPhi<<", "<<detPos.perp()<<", "<<detPos.z()
//              <<")   L1 (eta,phi): ("<< l1Eta<<", "<<l1Phi<<")"<<std::endl;

    result = true;
  }
   
  return result;
}

