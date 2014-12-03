#include "UserCode/L1RpcTriggerAnalysis/interface/BestSimulatedMuonFinder.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"

#include "Math/VectorUtil.h"

const SimTrack * BestSimulatedMuonFinder::result(const edm::Event &ev, const edm::EventSetup &es, const SimTrack * previous)
{
  const SimTrack * result = 0;
  edm::Handle<edm::SimTrackContainer> simTks;
  ev.getByLabel("g4SimHits",simTks);

  //edm::Handle<edm::SimVertexContainer> simVc;
  //ev.getByLabel("g4SimHits", simVc);
  //std::vector<SimVertex> mySimVertices = *(simVc.product());

  for (std::vector<SimTrack>::const_iterator it=simTks->begin(); it< simTks->end(); it++) {
    const SimTrack & aTrack = *it;
    if ( !(aTrack.type() == 13 || aTrack.type() == -13) )continue;
    if(previous && ROOT::Math::VectorUtil::DeltaR(aTrack.momentum(),previous->momentum())<0.07) continue;
    if ( !result || aTrack.momentum().pt() > result->momentum().pt()) result = &aTrack;
  }
  return result;
}
