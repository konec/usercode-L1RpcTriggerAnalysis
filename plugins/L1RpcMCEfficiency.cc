#include "L1RpcMCEfficiency.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "TH1D.h"
#include <utility>


#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "L1Trigger/RPCTrigger/interface/RPCConst.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1RpcTriggerAnalysisEfficiencyUtilities.h"


using namespace L1RpcTriggerAnalysisEfficiencyUtilities;


L1RpcMCEfficiency::L1RpcMCEfficiency(const edm::ParameterSet& conf) 
  : ptCutCodes(conf.getParameter<PtCutCodes>("ptCutCodes")), theSimpleEffNum(0), theSimpleEffDen(0)
{
  edm::LogInfo("L1RpcMCEfficiency")<<" CTORXX";
}


L1RpcMCEfficiency::~L1RpcMCEfficiency()
{
  edm::LogInfo("L1RpcMCEfficiency")<<" DTOR";
}

void L1RpcMCEfficiency::beginJob()
{
  TH1D* h;
  for (int ieta = 0; ieta <= 17; ieta++) {
    Key kg(ieta);
    h = theFileService->make<TH1D>(kg.name().c_str(), kg.name().c_str(), PtScale::nbins(), PtScale::xbins());
    theHistos[kg.name()] = h;
    for (unsigned int i = 0; i < ptCutCodes.size(); i++) {
      Key k(ieta, ptCutCodes[i]);
      h = theFileService->make<TH1D>(k.name().c_str(), k.name().c_str(), PtScale::nbins(), PtScale::xbins());
      theHistos[k.name()] = h;
    }
  }
  theHistos["etaDen"] = theFileService->make<TH1D>("etaDen", "{eta} denominator", 65, -2.1, 2.1);
  theHistos["etaNum"] = theFileService->make<TH1D>("etaNum", "{eta} numerator", 65, -2.1, 2.1);
  theHistos["ptStat"] = theFileService->make<TH1D>("ptStat", "event Statistics", PtScale::nbins(), PtScale::xbins());
  theHistos["dimu"]   = theFileService->make<TH1D>("dimu",   "diumon events; p_{T}; fraction",PtScale::nbins(), PtScale::xbins());
}


void L1RpcMCEfficiency::analyze( const edm::Event& ev, const edm::EventSetup& es)
{
  int nprint = 500;
  bool dbg = ( ev.id().event()/nprint*nprint== ev.id().event());
  if (dbg) std::cout <<"-----> L1RpcMCEfficiency, analyze event: " << ev.id()<< std::endl;

  edm::Handle<edm::SimTrackContainer> simTk;
  ev.getByLabel("g4SimHits",simTk);
  std::vector<SimTrack> mySimTracks = *(simTk.product());

  edm::Handle<edm::SimVertexContainer> simVc;
  ev.getByLabel("g4SimHits", simVc);
  std::vector<SimVertex> mySimVertices = *(simVc.product());

  typedef std::vector<L1MuRegionalCand> RegCand;
  RegCand allRpcCand;

  edm::Handle<std::vector<L1MuRegionalCand> > candB;
  ev.getByLabel("rpcTriggerDigis", "RPCb", candB);
  allRpcCand.insert(allRpcCand.end(), candB->begin(), candB->end());

  edm::Handle<std::vector<L1MuRegionalCand> > candF;
  ev.getByLabel("rpcTriggerDigis", "RPCf", candF);
  allRpcCand.insert(allRpcCand.end(), candF->begin(), candF->end());

  //
  // associate RPC to MC
  //
  typedef std::vector< std::pair<SimTrack, std::vector<L1MuRegionalCand> > > Compatibility;
  Compatibility compatibility; // pairs: gen_muon with good candidates vector
  for (std::vector<SimTrack>::const_iterator it=mySimTracks.begin(); it<mySimTracks.end(); it++) {
    const SimTrack & myTrack = *it;
    if ( !(myTrack.type() == 13 || myTrack.type() == -13) )continue;
    if (dbg) print(myTrack);
    float etaValMC = myTrack.momentum().eta();
    float phiValMC = myTrack.momentum().phi();
    RegCand goodCands; //create empty vector with for good candidates
    for (RegCand::const_iterator ir = allRpcCand.begin(); ir < allRpcCand.end(); ir++) {
      if (ir->empty()) continue;
      if (dbg) print(*ir);
      int tower = ir->eta_packed();
      if (tower > 16) tower = - ( (~tower & 63) + 1); 
      float etaValRpc = RPCConst::etaFromTowerNum(tower);
      float phiValRpc = L1RpcMCEfficiency::phiValue(ir->phi_packed());
      float delta_R=deltaR(etaValMC, phiValMC, etaValRpc, phiValRpc);
      bool isCompatible = (delta_R < 1.5);
      if (isCompatible) goodCands.push_back(*ir);
//      if (abs(tower)==8 && (ir->pt_packed() >= 18)) { std::cout <<" HERE"<< std::endl; }
    }
    compatibility.push_back( make_pair(myTrack, goodCands)); // make pair -> gen_muon - goodCands
  } 

  //
  // fill efficiency histos 
  //
  if (compatibility.size() >1) std::cout <<"WARNING - number of MC muons > 1, use only first one!"<<std::endl;
  for (Compatibility::const_iterator it = compatibility.begin(); it < compatibility.end(); it++) {
    if (it != compatibility.begin()) continue;
    
    float etaValMC = ((*it).first).momentum().eta();
    float ptValMC = std::min( ((*it).first).momentum().pt(), 159.);

    theHistos["etaDen"]->Fill(etaValMC);
    theHistos["ptStat"]->Fill(ptValMC);
    if( fabs(etaValMC) < 1.61) theSimpleEffDen++;

    unsigned int etaCode = EtaScale::etaCode(etaValMC);
    theHistos[Key(etaCode).name()]->Fill(ptValMC);

    if (it->second.size() >1) theHistos["dimu"]->Fill(ptValMC);
     
    if ( !(it->second.empty()) ) {
      theHistos["etaNum"]-> Fill(etaValMC);
      for (unsigned int i = 0; i < ptCutCodes.size(); i++){
        bool cutPassed = false;
        for ( RegCand::const_iterator ir = ((*it).second).begin(); ir < ((*it).second).end(); ir++){
          if (((*ir).pt_packed()) >= ptCutCodes[i]) cutPassed = true;
        }
        if (cutPassed) theHistos[Key(etaCode, ptCutCodes[i]).name()]->Fill(ptValMC);
      }
      if(fabs(etaValMC) < 1.61) theSimpleEffNum++;
    }
  }
}

void L1RpcMCEfficiency::endJob(){
  float efficiency = theSimpleEffNum/theSimpleEffDen;
  float efficiency_err = L1RpcMCEfficiency::effError(theSimpleEffNum,theSimpleEffDen);
  std::cout << "OVERALL EFFICIENCY: (" << efficiency*100 << " +/- " << efficiency_err*100 << ") %" <<  std::endl;
}



double L1RpcMCEfficiency::phiValue(const int iseg)
{
  double phi_val = 2*M_PI*(iseg)/144; // + (5./360.)*2*M_PI;
  return phi_val;
}


float L1RpcMCEfficiency::effError(float num, float denom)
{
  if (denom > 0) return (sqrt(denom*(num/denom)*(1-num/denom)))/denom;
  else return 0.;
}

void L1RpcMCEfficiency::print(const SimTrack & track)
{
  if ( track.type() == -99) return;
  float phi_gen = track.momentum().phi();
  float pt_gen = track.momentum().pt();
  float eta_gen = track.momentum().eta();
   std::cout <<" trackId: " <<track.trackId()
          << " pt_gen: " << pt_gen <<" eta_gen: "<<eta_gen<<" phi_gen: "<<phi_gen
          <<" vtx: "<<track.vertIndex()<<" type: "<<track.type()
          << std::endl;
}

void L1RpcMCEfficiency::print(const SimVertex & vertex)
{
   std::cout <<" vertexId: " << vertex.vertexId() << " position: " << vertex.position()
          << std::endl;
}

void L1RpcMCEfficiency::print(const L1MuRegionalCand & l1mu)
{
  if (l1mu.empty()) {
   std::cout <<"empty candidate!"<<std::endl;
   return;
  }
  int tower = l1mu.eta_packed(); if (tower > 16) { tower = - ( (~tower & 63) + 1); }
  std::cout << " L1 Cand:"
            << " ptValue: " << RPCConst::ptFromIpt(l1mu.pt_packed())<<" ("<<l1mu.pt_packed()<<")"
            << " etaValue: " << RPCConst::etaFromTowerNum(tower)<<" ("<<l1mu.eta_packed()<<")"
            << " phiValue: " << phiValue(l1mu.phi_packed()) <<" ("<<l1mu.phi_packed()<<")"
            << std::endl;
}

