#include "LinkSynchroAnalysis.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "CondFormats/RPCObjects/interface/RPCReadOutMapping.h"
#include "CondFormats/RPCObjects/interface/LinkBoardSpec.h"

#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESWatcher.h"
#include "CondFormats/RPCObjects/interface/RPCEMap.h"
#include "CondFormats/DataRecord/interface/RPCEMapRcd.h"


#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "TFile.h"
#include "TH1F.h"
#include <map> 

LinkSynchroAnalysis::LinkSynchroAnalysis(const edm::ParameterSet& cfg) 
    : RPCMonitorLinkSynchro(cfg.getParameter<edm::ParameterSet>("linkMonitorPSet")), theAnaConfig(cfg),theCabling(0), theEventCounter(0) { }

LinkSynchroAnalysis::~LinkSynchroAnalysis(){ delete theCabling;}

void LinkSynchroAnalysis::beginJob()
{
  RPCMonitorLinkSynchro::beginJob();
  theHistos.SetOwner();
  theHistos.Add(new TH1F("hBX","hBX",3564,0.,3564));
  theHistos.Add(new TH1F("hDeltaPhi","hDeltaPhi",100, 0., 1.));
  theHistos.Add(new TH1F("hDeltaEta","hDeltaEta",100, 0., 1.));
  theHistos.Add(new TH1F("hDxy","hDxy",100.,0.,1.));
  theHistos.Add(new TH1F("hNumTracks","hNumTracks",50,0.,250.)); 

  typedef std::vector<edm::ParameterSet> VPSet;
  VPSet selectorConfigs = theAnaConfig.getParameter<VPSet>("ORedSynchroFilters");
  for (VPSet::const_iterator it=selectorConfigs.begin(); it != selectorConfigs.end(); ++it) {
    SynchroSelector f(*it);
    if (it->getParameter<std::string>("collection") == "generalTracks") {
      f.setHistoNum(static_cast<TH1F* >(theHistos.FindObject("hNumTracks")));
      f.setHistoPhi(static_cast<TH1F* >(theHistos.FindObject("hDeltaPhi")));
      f.setHistoEta(static_cast<TH1F* >(theHistos.FindObject("hDeltaEta")));
      f.setHistoDxy(static_cast<TH1F* >(theHistos.FindObject("hDxy")));
    }
    theSynchroFilters.push_back(f); 
  }
}

void LinkSynchroAnalysis::endJob()
{
  RPCMonitorLinkSynchro::endJob();
  bool writeHistos = theAnaConfig.getUntrackedParameter<bool>("writeHistograms", false);
  if (writeHistos) {
    std::string histoFile = theAnaConfig.getUntrackedParameter<std::string>("histoFileName");
    TFile f(histoFile.c_str(),"RECREATE");
    theHistos.Write();
    histos().Write(); 
    edm::LogInfo(" END JOB, histos saved!");
    edm::LogInfo("") <<"LINK SYNCHRO ANALYSIS number of events SELECTED: "<<theEventCounter;
    f.Close();
  }
}

const RPCRawSynchro::ProdItem& LinkSynchroAnalysis::select(const RPCRawSynchro::ProdItem & vItem, const edm::Event &ev, const edm::EventSetup &es)
{
  static RPCRawSynchro::ProdItem selected;
  selected.clear();

  bool wasTakeIt = false;

  if (theMapWatcher.check(es)) {
    delete theCabling;
    edm::ESTransientHandle<RPCEMap> readoutMapping;
    es.get<RPCEMapRcd>().get(readoutMapping);
    theCabling = readoutMapping->convert();
    LogTrace("") << "LinkSynchroAnalysis - record has CHANGED!!, read map, VERSION: " << theCabling->version();
  }

  for(RPCRawSynchro::ProdItem::const_iterator it = vItem.begin(); it != vItem.end(); ++it) {
    const LinkBoardElectronicIndex & path = it->first;
    const  std::vector<FebConnectorSpec> & febs = theCabling->location(path)->febs();
    std::map<uint32_t,bool> dets;
    for (std::vector<FebConnectorSpec>::const_iterator iif = febs.begin(); iif != febs.end(); ++iif) dets[iif->rawId()] = true;

    bool takeIt = false;
    for ( std::map<uint32_t,bool>::const_iterator im = dets.begin(); im != dets.end(); ++im) {
      RPCDetId rpcDet(im->first);
      for (std::vector<SynchroSelector>::iterator ix=theSynchroFilters.begin(); ix != theSynchroFilters.end(); ++ix) if(ix->takeIt(rpcDet,ev,es) ) takeIt = true;
    }
    if (takeIt) selected.push_back(*it);
    if (takeIt) wasTakeIt = true;
  }
  if (wasTakeIt) static_cast<TH1*>(theHistos.FindObject("hBX"))->Fill( ev.bunchCrossing());
  if (wasTakeIt) theEventCounter++;
  return selected;
}

