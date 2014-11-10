#include <iostream>
#include <iomanip>

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "UserCode/OMTFSimulation/interface/OMTFProcessor.h"
#include "UserCode/OMTFSimulation/interface/OMTFinputMaker.h"
#include "UserCode/OMTFSimulation/interface/OMTFinput.h"
#include "UserCode/OMTFSimulation/interface/OMTFSorter.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfiguration.h"
#include "UserCode/OMTFSimulation/interface/OMTFConfigMaker.h"
#include "UserCode/OMTFSimulation/interface/XMLConfigWriter.h"

#include "UserCode/L1RpcTriggerAnalysis/plugins/OMTFROOTReader.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaEff.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/AnaSiMuDistribution.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h"

#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"


#include "TChain.h"
#include "TVector2.h"

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
OMTFROOTReader::OMTFROOTReader(const edm::ParameterSet & cfg){

  if (cfg.exists("anaEff")) myAnaEff = new   AnaEff(cfg.getParameter<edm::ParameterSet>("anaEff") );
  if (cfg.exists("anaSiMuDistribution")) myAnaSiMu = new AnaSiMuDistribution( cfg.getParameter<edm::ParameterSet>("anaSiMuDistribution"));

  if (cfg.exists("omtf")){
    myOMTFConfig = new OMTFConfiguration(cfg.getParameter<edm::ParameterSet>("omtf"));
    myOMTFConfigMaker = new OMTFConfigMaker(cfg.getParameter<edm::ParameterSet>("omtf"));
    myOMTF = new OMTFProcessor(cfg.getParameter<edm::ParameterSet>("omtf"));
  }
  myInputMaker = new OMTFinputMaker();
  mySorter = new OMTFSorter();

  myWriter = new XMLConfigWriter();
  std::string fName = "OMTF_Events";
  myWriter->initialiseXMLDocument(fName);

  theConfig = cfg;

  dumpResultToXML = theConfig.getParameter<bool>("dumpResultToXML");
  dumpGPToXML = theConfig.getParameter<bool>("dumpGPToXML");
  makeConnectionsMaps = theConfig.getParameter<bool>("makeConnectionsMaps");
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
OMTFROOTReader::~OMTFROOTReader(){

  delete myOMTF;
  delete myOMTFConfig;
  delete myInputMaker;
  delete mySorter;

  if(myAnaEff) delete myAnaEff;
  if(myAnaSiMu) delete myAnaSiMu;

}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void OMTFROOTReader::beginJob(){

  myHistos.SetOwner();
  myAnaEff->init(myHistos);
  myAnaSiMu->init(myHistos);

}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void OMTFROOTReader::analyze(const edm::Event&, const edm::EventSetup& es){
  /*
  myInputMaker->initialize(es);

  // define input chain
  TChain chain("tL1Rpc");
  std::vector<std::string> treeFileNames = theConfig.getParameter<std::vector<std::string> >("treeFileNames");
  for (auto it:treeFileNames)  chain.Add(it.c_str());
  int maxEvents = theConfig.getParameter<int>("maxEvents");
 
  // prepare datastructures and branches
  std::vector<std::pair<uint32_t, uint32_t> > *digSpec = 0;
  TBranch *digSpecBranch = 0;

  EventObj * event = 0;
  TrackObj * simu = 0;
  L1ObjColl* l1ObjColl = 0;
  HitSpecObj* hitSpecProp = 0;
  HitSpecObj* hitSpec = 0;

  chain.SetBranchAddress("event",&event);
  chain.SetBranchAddress("simu",&simu);
  chain.SetBranchAddress("digSpec",&digSpec,&digSpecBranch);
  chain.SetBranchAddress("l1ObjColl",&l1ObjColl);
  chain.SetBranchAddress("hitSpecProp",&hitSpecProp);
  chain.SetBranchAddress("hitSpec",&hitSpec);

  // number of events
  Int_t nentries= (Int_t) chain.GetEntries();
  if(maxEvents>0 && maxEvents<=nentries) nentries = maxEvents;

  std::cout <<" ENTRIES: " << nentries << std::endl;

  xercesc::DOMElement *aTopElement = 0;

  // main loop
  unsigned int lastRun = 0;
  for (int ev=0; ev<nentries; ev+=1) {

    chain.GetEntry(ev);

    std::vector<std::pair<uint32_t, uint32_t> > filteredDigis = filterDigis(digSpec);

    L1ObjColl myL1ObjColl = *l1ObjColl;

    if ( (lastRun != (*event).run) || 
	 (nentries>1000 && ev%(nentries/10)==0) || 
	 nentries<1000) { 
      lastRun = (*event).run; 
      std::cout <<"RUN:"    << std::setw(7) << (*event).run
                <<" event:" << std::setw(8) << ev
                <<" done:"  << std::setw(6)<< std::setiosflags(std::ios::fixed) << std::setprecision(2) << ev*100./nentries<<"%";
      std::cout<<std::endl; 
    }

    if (myAnaSiMu && 
	!myAnaSiMu->filter(event, simu, hitSpec, hitSpecProp) && 
	theConfig.getParameter<bool>("filterByAnaSiMuDistribution") ) continue;

    if(dumpResultToXML) aTopElement = myWriter->writeEventHeader(ev);

    for(unsigned int iProcessor=0;iProcessor<6;++iProcessor){
      //edm::LogInfo("OMTF ROOTReader")<<"iProcessor: "<<iProcessor;

	const OMTFinput *myInput = myInputMaker->buildInputForProcessor(*digSpec,iProcessor);

	///Input data with phi ranges shifted for each processor, so it fits 10 bits range
	const OMTFinput myShiftedInput =  myOMTF->shiftInput(iProcessor,*myInput);	

	///Phi maps should be made with original, global phi values.
	if(makeConnectionsMaps) myOMTFConfigMaker->makeConnetionsMap(iProcessor,*myInput);
	/////////////////////////

	///Results for each GP in each logic region of given processor
	const std::vector<OMTFProcessor::resultsMap> & myResults = myOMTF->processInput(iProcessor,myShiftedInput);

	L1Obj myOTFCandidate = mySorter->sortProcessorResults(myResults);
	////Swith from internal processor 10bit scale to global one
	int procOffset = OMTFConfiguration::globalPhiStart(iProcessor);
	if(procOffset<0) procOffset+=OMTFConfiguration::nPhiBins;
	myOTFCandidate.phi+=OMTFConfiguration::globalPhiStart(iProcessor)+511;
	///
	if(myOTFCandidate.pt) myL1ObjColl.push_back(myOTFCandidate, false, 0.); 

	///Write to XML
	if(dumpResultToXML){
	  xercesc::DOMElement * aProcElement = myWriter->writeEventData(aTopElement,iProcessor,myShiftedInput);
	  for(unsigned int iRegion=0;iRegion<6;++iRegion){
	    ///Dump only regions, where a candidate was found
	    L1Obj myCand = mySorter->sortRegionResults(myResults[iRegion]);
	    if(myCand.pt){
	      myWriter->writeCandidateData(aProcElement,iRegion,myCand);
	      for(auto & itKey: myResults[iRegion]) myWriter->writeResultsData(aProcElement, 
									       iRegion,
									       itKey.first,itKey.second);    
	    }
	  }
	  //delete aProcElement;
	}
    }
    //if(dumpResultToXML) delete aTopElement;
    if (myAnaEff) myAnaEff->run(simu, &myL1ObjColl, hitSpecProp);
  }
  */
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void OMTFROOTReader::endJob(){

  if(dumpResultToXML){
    std::string fName = "TestEvents.xml";
    myWriter->finaliseXMLDocument(fName);
  }

  if(dumpGPToXML){
    std::string fName = "OMTF";
    myWriter->initialiseXMLDocument(fName);
    const std::map<Key,GoldenPattern*> & myGPmap = myOMTF->getPatterns();
    for(auto itGP: myGPmap){
      std::cout<<*itGP.second<<std::endl;
      myWriter->writeGPData(*itGP.second);
    }
    fName = "GPs.xml";
    myWriter->finaliseXMLDocument(fName);
  }

  if(makeConnectionsMaps){
    std::string fName = "Connections.xml";  
    myWriter->writeConnectionsData(OMTFConfiguration::measurements4D);
    myWriter->finaliseXMLDocument(fName);
    myOMTFConfigMaker->printPhiMap(std::cout);
    myOMTFConfigMaker->printConnections(std::cout,0,0);
  }
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
std::vector<std::pair<uint32_t, uint32_t> > OMTFROOTReader::filterDigis(std::vector<std::pair<uint32_t, uint32_t> > *digSpec){

  if(!theConfig.getParameter<bool>("dropRPCPrimitives") &&
     !theConfig.getParameter<bool>("dropDTPrimitives") &&
     !theConfig.getParameter<bool>("dropCSCPrimitives")) return *digSpec;
  
  std::vector<std::pair<uint32_t, uint32_t> > filteredDigiSpec;
  for(auto it:*digSpec){
    uint32_t rawId = it.first;   
    DetId detId(rawId);
    switch (detId.subdetId()) {
    case MuonSubdetId::RPC: {
      if(!theConfig.getParameter<bool>("dropRPCPrimitives")) filteredDigiSpec.push_back(it);
      break;
    }
    case MuonSubdetId::DT: {
      if(!theConfig.getParameter<bool>("dropDTPrimitives")) filteredDigiSpec.push_back(it);
      break;
    }
    case MuonSubdetId::CSC: {
      if(!theConfig.getParameter<bool>("dropCSCPrimitives")) filteredDigiSpec.push_back(it);
      break;
    }
    }
  }
  return filteredDigiSpec;
}
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

