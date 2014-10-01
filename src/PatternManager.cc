#include <fstream>
#include <sstream>
#include <bitset>
#include <typeinfo>

#include "UserCode/L1RpcTriggerAnalysis/interface/PatternManager.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/Pattern.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/GoldenPattern.h"

#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
////
#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/RPCGeometry/interface/RPCRollSpecs.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/CommonTopologies/interface/RectangularStripTopology.h"
#include "Geometry/CommonTopologies/interface/TrapezoidalStripTopology.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

// Xerces include files
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOM.hpp"
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include "xercesc/sax/HandlerBase.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/util/XercesDefs.hpp"
XERCES_CPP_NAMESPACE_USE

#include "DQMServices/ClientConfig/interface/ParserFunctions.h"
////


namespace {
  typedef struct {
    UInt_t key_det;
    unsigned int key_pt;
    unsigned int key_phi;
    int key_eta;
    unsigned int key_strip;
    int key_ch;   
    unsigned int pat_Case;
    UInt_t patDet;
    int posOrBend;
    unsigned int freq; 
  } ENTRY;
}


// OMTF configuration. To be moved to separate, configuration holder class.
std::map<int,int> PatternManager::hwToLogicLayer;
std::map<int,int> PatternManager::logicToHwLayer;
int PatternManager::nLogicLayers = 0;
std::vector<int> PatternManager::refToLogicNumber(8);

PatternManager::PatternManager(const edm::ParameterSet &cfg) 
  : theConfig(cfg), theEvForPatCounter(0), theEvUsePatCounter(0), phiMapDone(false){

  myPhiConverter = 0;

  std::string configFile = getenv("CMSSW_BASE");
  configFile+="/src/UserCode/L1RpcTriggerAnalysis/data/OMTF/hwToLogicLayer.xml";
  readXMLConfig(configFile);

  if(theConfig.getUntrackedParameter<bool>("dump",false)) {
    
    domImpl = DOMImplementationRegistry::getDOMImplementation(qtxml::_toDOMS("Range"));   
    theDoc = domImpl->createDocument(0,qtxml::_toDOMS("OMTF_Events"), 0);
    theTopElement = theDoc->getDocumentElement();    
  }
}

PatternManager::~PatternManager(){
  std::cout <<" Events checked for pattenrs:    " << theEvForPatCounter << std::endl;
  std::cout <<" Events used    for pattenrs:    " << theEvUsePatCounter << std::endl;
  std::cout <<" Size of GoldenPatterns:         " << theGPs.size() << std::endl;

  if (theConfig.getUntrackedParameter<bool>("dump",false)) {
    for (std::map< GoldenPattern::Key, GoldenPattern>::const_iterator 
	   im = theGPs.begin(); im!= theGPs.end(); ++im) {
      GoldenPattern *gp =  const_cast<GoldenPattern*>(&(*im).second);      
      gp->makeIntegratedCache();      
      //std::cout <<" GP: "<< *gp << std::endl;
      //gp->plot();
      writeXML("Patterns.xml");      
      std::string fname = "TestEvents.xml";
      XMLFormatTarget* formTarget = new LocalFileFormatTarget(fname.c_str());
      xercesc::DOMWriter* domWriter = (dynamic_cast<DOMImplementation*>(domImpl))->createDOMWriter();
      domWriter->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
      domWriter->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
      domWriter->writeNode(formTarget,*theTopElement);
      delete formTarget;
      //theDoc->release();
    }
  }

  if(myPhiConverter) delete myPhiConverter;
}
////////////////////////////////////////////
////////////////////////////////////////////
void PatternManager::run(const EventObj* ev,  const edm::EventSetup& es,
			   const TrackObj * simu, const HitSpecObj * hitSpec,  
			   const VDigiSpec & vDigi){

  if (theConfig.exists("patternInpFile") && 
      theConfig.exists("patternOutFile")) return;

  int theEtaCode = L1RpcTriggerAnalysisEfficiencyUtilities::EtaScale::etaCode(simu->eta());
  if(theEtaCode<6) theEtaCode = 0;
  if(theEtaCode>5 && theEtaCode<10) theEtaCode = 1;
  if(theEtaCode>9 && theEtaCode<13) theEtaCode = 2;
  if(theEtaCode>12 && simu->eta()<2.1) theEtaCode = 3;
  if(simu->eta()>2.1) theEtaCode = 4;

  if(theEtaCode!=1) return;

  //std::cout<<"eta: "<<simu->eta()<<" phi: "<<simu->phi()<<std::endl;

  float phiref  = 0;
  float ptref  = simu->pt(); 
  int chargeref = simu->charge();
  int stripRef = 0;

  std::map<int,int> refPhi;
  ///Create hits pattern   
  Pattern pattern;
  theEvForPatCounter++;
  if(!myPhiConverter) myPhiConverter = new MtfCoordinateConverter(es);
  myPhiConverter->setReferencePhi(0);
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); ++is) {
    DetId detId( is->first);    
    if(detId.subdetId()==MuonSubdetId::DT && DTphDigiSpec(is->first, is->second).bxNum()!=0) continue; 
    bool isOK = pattern.add(*is); 
    if (!isOK) return;

    for(int iGranularity=3;iGranularity<4;++iGranularity){      
      //int aLayer = myPhiConverter->getLayerNumber(is->first) + 1000*iGranularity; 
      int aLayer = myPhiConverter->getLayerNumber(is->first); 
      int nPhi = GoldenPattern::Key::nPhi(aLayer);
      int iPhi = myPhiConverter->convert(*is,nPhi);
      //Accept only events with single hit in each layer
      if(refPhi.find(aLayer)!=refPhi.end()) return; 
      ///Use only selected layers as reference layers
      for(unsigned int i=0;i<refToLogicNumber.size();++i){
	//std::cout<<refToLogicNumber[i]<<" "<<aLayer<<std::endl;
	if(refToLogicNumber[i] ==  aLayer) refPhi[aLayer] = iPhi;	
      }
    }
  }
  if (pattern.size() == 0) return; 

  theEvUsePatCounter++;
  ////////////////////////////////////////////  
  ///Create GPs for different reference layers
  for (auto const & it : refPhi){  

  GoldenPattern::Key key(theEtaCode, ptref, chargeref, phiref,stripRef);
  key.theDet = it.first;
  int nPhi = GoldenPattern::Key::nPhi(it.first);
  myPhiConverter->setReferencePhi((float)it.second/nPhi*2*M_PI);
  if (theGPs.find(key)==theGPs.end()) theGPs[key]=GoldenPattern(key);
  theGPs[key].add(pattern, myPhiConverter);
  //theGPs[key].add(GoldenPattern::PosBenCase::TOTDEV,501,pattern.deviationSum(myPhiConverter,nPhi),1);

  ///Count number of events for each key
  if (aCounterMap.find(key)==aCounterMap.end()) aCounterMap[key]=1;
  else aCounterMap[key]++;
  theGPs[key].theKey.theRefStrip = aCounterMap[key];
  }
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
L1Obj PatternManager::check(const EventObj* ev, const edm::EventSetup& es,
			    const TrackObj * simu, 
			    const HitSpecObj * hitSpec,  
			    const VDigiSpec & vDigi,
			    int iGranularity){

  L1Obj candidate;
  int theEtaCode = L1RpcTriggerAnalysisEfficiencyUtilities::EtaScale::etaCode(simu->eta());  
  if(theEtaCode<6) theEtaCode = 0;
  if(theEtaCode>5 && theEtaCode<10) theEtaCode = 1;
  if(theEtaCode>9 &&  theEtaCode<13) theEtaCode = 2;
  if(theEtaCode>12 && simu->eta()<2.1) theEtaCode = 3;
  if(simu->eta()>2.1) theEtaCode = 4;
  
  if (theConfig.exists("patternInpFile") && 
      theConfig.exists("patternOutFile")) return candidate;

  if(!myPhiConverter) myPhiConverter = new MtfCoordinateConverter(es);
  myPhiConverter->setReferencePhi(0);
  //////////////////////
  theEvForPatCounter++;
  
  Pattern pattern;
  std::map<uint32_t,int> refPhi;
  static bool skipRpcData   = theConfig.getUntrackedParameter<bool>("skipRpcData",  false);
  static bool skipDtCscData = theConfig.getUntrackedParameter<bool>("skipDtCscData",false);
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); ++is) {
    DetId detId( is->first);
    if (skipRpcData   && detId.subdetId()==MuonSubdetId::RPC) continue;
    if (skipDtCscData && (detId.subdetId()==MuonSubdetId::DT || detId.subdetId()==MuonSubdetId::CSC) ) continue;    
    int aLayer = myPhiConverter->getLayerNumber(is->first);
    int nPhi = GoldenPattern::Key::nPhi(aLayer);
    int iPhi = myPhiConverter->convert(*is,nPhi);

    pattern.add(*is);

    if(iPhi<0) iPhi+=nPhi;
    
    bool skipLayer = true;
    for(auto aRef : refToLogicNumber){
      if(aRef==aLayer){
	skipLayer = false;
	break;
      }
    }
    if(skipLayer) continue;    
    refPhi[aLayer] = iPhi; //FIXME! Use multimap
  }

  pattern.makeHitDetsList();
  if (pattern.size() == 0) return candidate; 

  GoldenPattern::Result bestMatching;
  GoldenPattern::Key    bestKey;

 ///////Dump to XML
  xercesc::DOMElement *aEvent = 0;
  xercesc::DOMElement* aGP = 0;

  if (theConfig.getUntrackedParameter<bool>("dump",false)){
    std::ostringstream stringStr;
    int nPhi = 4096;
    myPhiConverter->setReferencePhi(0);    
    aEvent = theDoc->createElement(qtxml::_toDOMS("Event"));

    stringStr.str("");
    stringStr<<ev->id;
    aEvent->setAttribute(qtxml::_toDOMS("iEvent"), qtxml::_toDOMS(stringStr.str()));
    xercesc::DOMElement *aBx = theDoc->createElement(qtxml::_toDOMS("bx"));
    stringStr.str("");
    //stringStr<<ev->bx;
    stringStr<<ev->id*2;
    aBx->setAttribute(qtxml::_toDOMS("iBx"), qtxml::_toDOMS(stringStr.str()));
    aEvent->appendChild(aBx);
    
    pattern.dumpToXML(theDoc,aBx,
    		      myPhiConverter,nPhi);        
    
    theTopElement->appendChild(aEvent);
  }

  ///High precision chambers first
  for (auto igps = theGPs.begin(); igps!=theGPs.end();++igps) {    
    //AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA if(igps->first.theDet>15) continue;
    ////
    if(igps->first.theEtaCode!=theEtaCode) continue;
    ////
    igps->second.makeIntegratedCache();
    /// Dump intermediate resutls to XML
    if (theConfig.getUntrackedParameter<bool>("dump",false)){
      aGP = theDoc->createElement(qtxml::_toDOMS("GP"));
      std::ostringstream stringStr;
      stringStr.str("");
      stringStr<<igps->first.thePtCode;
      aGP->setAttribute(qtxml::_toDOMS("iPt"), qtxml::_toDOMS(stringStr.str()));
      stringStr.str("");
      stringStr<<igps->first.theEtaCode;
      aGP->setAttribute(qtxml::_toDOMS("iEta"), qtxml::_toDOMS(stringStr.str()));
      stringStr.str("");
      stringStr<<"0";
      aGP->setAttribute(qtxml::_toDOMS("iPhi"), qtxml::_toDOMS(stringStr.str()));
      stringStr.str("");
      stringStr<<igps->first.theCharge;
      aGP->setAttribute(qtxml::_toDOMS("iCharge"), qtxml::_toDOMS(stringStr.str()));
    }

    for (auto const & it : refPhi){
      if(igps->first.theDet!=it.first) continue;     
      if (theConfig.getUntrackedParameter<bool>("dump",false)) aEvent->appendChild(aGP);
      int nPhi = GoldenPattern::Key::nPhi(it.first);
      myPhiConverter->setReferencePhi((float)it.second/nPhi*2*M_PI);	  
      GoldenPattern::Result result =  igps->second.compare(pattern,myPhiConverter);
      /// Dump intermediate resutls to XML
      if (theConfig.getUntrackedParameter<bool>("dump",false)){
	xercesc::DOMElement* aResult = theDoc->createElement(qtxml::_toDOMS("Result"));
	std::ostringstream stringStr;
	stringStr.str("");
	stringStr<<igps->first.theDet;
	std::cout<<" iRefLayer: "<<stringStr.str()<<std::endl;
	aResult->setAttribute(qtxml::_toDOMS("iRefLayer"), qtxml::_toDOMS(stringStr.str()));
	for(auto mType=result.myResults.cbegin();mType!=result.myResults.cend();++mType){    
	  for (auto it=mType->second.cbegin(); it!=mType->second.cend();++it){
	    float val = it->second;
	    xercesc::DOMElement* aLayer = theDoc->createElement(qtxml::_toDOMS("Layer"));
	    stringStr.str("");
	    stringStr<<myPhiConverter->getLayerNumber(it->first);
	    aLayer->setAttribute(qtxml::_toDOMS("iLayer"), qtxml::_toDOMS(stringStr.str()));
	    stringStr.str("");
	    stringStr<<val;
	    aLayer->setAttribute(qtxml::_toDOMS("value"), qtxml::_toDOMS(stringStr.str()));
	    aResult->appendChild(aLayer);
	  }
	}
	aGP->appendChild(aResult);
	std::cout<<result<<std::endl;
      }
      ////////////////////////////////////
      ////////////////////////////////////
      //if((theEtaCode==2 || theEtaCode==3) && result.value()<-8) result = GoldenPattern::Result();
      ////////////////////
      if (bestMatching < result) {
	bestMatching = result;
	bestKey =  igps->first;
	//bestKey.thePhiCode = it.second + igps->second.phiOffset();       
	//bestKey.theCharge = it.first;       
	} 	
    }
  }
  ///Then RPC
  if(false && !bestMatching){
    for (auto igps = theGPs.begin(); igps!=theGPs.end();++igps) {
      igps->second.makeIntegratedCache();
      ////
      if(igps->first.theEtaCode!=theEtaCode) continue;
      ////
      /// Dump intermediate resutls to XML
      if (theConfig.getUntrackedParameter<bool>("dump",false)){
	aGP = theDoc->createElement(qtxml::_toDOMS("GP"));
	std::ostringstream stringStr;
	stringStr.str("");
	stringStr<<igps->first.thePtCode;
	aGP->setAttribute(qtxml::_toDOMS("iPt"), qtxml::_toDOMS(stringStr.str()));
	stringStr.str("");
	stringStr<<igps->first.theEtaCode;
	aGP->setAttribute(qtxml::_toDOMS("iEta"), qtxml::_toDOMS(stringStr.str()));
	stringStr.str("");
	stringStr<<"0";
	aGP->setAttribute(qtxml::_toDOMS("iPhi"), qtxml::_toDOMS(stringStr.str()));
	stringStr.str("");
	stringStr<<igps->first.theCharge;
	aGP->setAttribute(qtxml::_toDOMS("iCharge"), qtxml::_toDOMS(stringStr.str()));
      }
      for (auto const & it : refPhi){  
	if(igps->first.theDet<16) continue;     
	if(igps->first.theDet!=it.first) continue;  
	if (theConfig.getUntrackedParameter<bool>("dump",false)) aEvent->appendChild(aGP);   
	int nPhi = GoldenPattern::Key::nPhi(it.first);
	myPhiConverter->setReferencePhi((float)it.second/nPhi*2*M_PI);	  
	GoldenPattern::Result result =  igps->second.compare(pattern,myPhiConverter);
	////////////////////
	//if((theEtaCode==2 || theEtaCode==3) && result.value()<-8) result = GoldenPattern::Result();
	////////////////////
	////
	/// Dump intermediate resutls to XML
	if (theConfig.getUntrackedParameter<bool>("dump",false)){
	aGP = theDoc->createElement(qtxml::_toDOMS("GP"));
	std::ostringstream stringStr;
	stringStr.str("");
	stringStr<<igps->first.thePtCode;
	aGP->setAttribute(qtxml::_toDOMS("iPt"), qtxml::_toDOMS(stringStr.str()));
	stringStr.str("");
	stringStr<<igps->first.theEtaCode;
	aGP->setAttribute(qtxml::_toDOMS("iEta"), qtxml::_toDOMS(stringStr.str()));
	stringStr.str("");
	stringStr<<"0";
	aGP->setAttribute(qtxml::_toDOMS("iPhi"), qtxml::_toDOMS(stringStr.str()));
	stringStr.str("");
	stringStr<<igps->first.theCharge;
	aGP->setAttribute(qtxml::_toDOMS("iCharge"), qtxml::_toDOMS(stringStr.str()));
      }
	if (bestMatching < result) {
	  bestMatching = result;
	  bestKey =  igps->first;
	  //bestKey.thePhiCode = it.second + igps->second.phiOffset();       
	  //bestKey.theCharge = it.first;       
	} 	
      }
    }
  }

  if (false && bestMatching) {
    ////////////DEBUG
    std::cout<<"eta: "<<simu->eta()<<" phi: "<<hitSpec->position().phi()<<std::endl;
    std::cout<<"Best match: "<<bestKey<<" "<<bestMatching<<std::endl;
    std::cout<<theGPs[bestKey]<<std::endl;   
    int nPhi = GoldenPattern::Key::nPhi(bestKey.theDet);
    //myPhiConverter->setReferencePhi((float)it.second/nPhi*2*M_PI);
    pattern.print(myPhiConverter,nPhi);	
    std::cout<<"-----------------"<<std::endl;

    for (auto igps = theGPs.begin(); igps!=theGPs.end();++igps) {
      for (auto const & it : refPhi){
	if(igps->first.theDet!=it.first) continue;  
	int nPhi = GoldenPattern::Key::nPhi(it.first);
	myPhiConverter->setReferencePhi((float)it.second/nPhi*2*M_PI);	  
	GoldenPattern::Result result =  igps->second.compare(pattern,myPhiConverter);
	if(result.nMatchedTot()>4 && igps->first.thePtCode==19){
	  //if(result.nMatchedTot()>=bestMatching.nMatchedTot() && igps->first.theDet%1000/100<3 && igps->first.thePtCode>10){
	  //if(igps->first.thePtCode==19){
	    std::cout<<igps->first<<" "<<result<<" better? "<<(bestMatching<result)<<std::endl;
	    std::cout<<"GP: "<<igps->second<<std::endl; 
	    pattern.print(myPhiConverter,nPhi);
	  }
	}
      }
    }
  ////////////DEBUG            
  if (bestMatching) {
    candidate.pt = bestKey.ptValue();
    candidate.eta = bestKey.etaValue();
    candidate.phi = bestKey.phiValue();
    candidate.charge = bestKey.chargeValue();
    candidate.q   = bestMatching.nMatchedTot()
 	+ 100*bestMatching.withStation1()
        + 200*bestMatching.withStation2()    
        + 500*bestMatching.withStation3();    
    candidate.disc = bestMatching.value();
    candidate.type = L1Obj::OTF;
  }

  /// Dump intermediate resutls to XML
  if (theConfig.getUntrackedParameter<bool>("dump",false)){
    xercesc::DOMElement* aResult = theDoc->createElement(qtxml::_toDOMS("FinalResult"));
    std::ostringstream stringStr;
    stringStr.str("");
    stringStr<<bestKey.thePtCode;
    aResult->setAttribute(qtxml::_toDOMS("OMTFpt"), qtxml::_toDOMS(stringStr.str()));
    stringStr.str("");
    stringStr<<candidate.charge;
    aResult->setAttribute(qtxml::_toDOMS("OMTFcharge"), qtxml::_toDOMS(stringStr.str()));
    stringStr.str("");
    stringStr<<candidate.q%100;
    aResult->setAttribute(qtxml::_toDOMS("OMTFnHits"), qtxml::_toDOMS(stringStr.str()));
    stringStr.str("");
    stringStr<<candidate.disc;
    aResult->setAttribute(qtxml::_toDOMS("OMTFvalue"), qtxml::_toDOMS(stringStr.str()));
    aEvent->appendChild(aResult);
  }

  //////////////////
  return candidate;
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
void PatternManager::beginJob()
{
  if ( !theConfig.exists("patternInpFile") ) return;
  std::string patternInpFileName = theConfig.getParameter<std::string>("patternInpFile");
  TFile patternInpFile( patternInpFileName.c_str());
  TTree * tree = (TTree*) patternInpFile.Get("FlatPatterns");
  static ENTRY entry;
  tree->SetBranchAddress("entry",&entry); 
  Int_t nentries = (Int_t) tree->GetEntries();
  for (Int_t i=0; i<nentries; ++i) {
    tree->GetEntry(i);

    //if(entry.key_strip<5E3) continue;
    if(entry.key_pt>20 || entry.key_pt<16) continue;
        
    bool skipLayer = true;
    for(auto aRef : refToLogicNumber){
      if(aRef==(int)entry.key_det){
	skipLayer = false;
	break;
      }
    }
    if(skipLayer) continue;    
    
    GoldenPattern::Key key;
    key.theDet =     entry.key_det;
    key.thePtCode =  entry.key_pt;
    key.thePhiCode = entry.key_phi;
    key.theEtaCode = entry.key_eta;
    key.theCharge =  entry.key_ch;
    key.theRefStrip =  entry.key_strip;
    
    GoldenPattern::PosBenCase pat_Case = static_cast<GoldenPattern::PosBenCase>(entry.pat_Case);
    if (theGPs.find(key)==theGPs.end()) theGPs[key]=GoldenPattern(key);
    theGPs[key].add(pat_Case, entry.patDet, entry.posOrBend, entry.freq);
  }
  
  for (auto igps = theGPs.begin(); igps != theGPs.end();) {
    GoldenPattern & gp = igps->second;      
    if(!gp.purge()) {theGPs.erase(igps++);} else { ++igps; } 
  }
  
  delete tree;
  patternInpFile.Close();
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void PatternManager::endJob(){

  if ( !theConfig.exists("patternOutFile") ) return;
  std::string patternOutFileName = theConfig.getParameter<std::string>("patternOutFile");
  TFile patternOutFile( patternOutFileName.c_str(),"RECREATE");   
  
  for (auto igps = theGPs.begin(); igps != theGPs.end();) {
    GoldenPattern & gp = igps->second;
    if(!gp.purge()) {theGPs.erase(igps++);} else { ++igps; } 
  }
  
  
  static ENTRY entry;
  TTree *tree = new TTree("FlatPatterns","FlatPatterns");
  tree->Branch("entry",&entry,"key_det/i:key_pt/i:key_phi/i:key_eta/i:key_strip/i:key_ch/I:pat_Case/i:patDet/i:posOrBend/I:freq/i");

  for (std::map< GoldenPattern::Key, GoldenPattern>::const_iterator igps = theGPs.begin(); igps != theGPs.end(); ++igps) {
    const GoldenPattern & gp = igps->second; 
    entry.key_det =  gp.theKey.theDet;
    entry.key_pt =  gp.theKey.thePtCode;
    entry.key_phi =  gp.theKey.thePhiCode;
    entry.key_eta =  gp.theKey.theEtaCode;
    entry.key_ch =  gp.theKey.theCharge;
    entry.key_strip =  gp.theKey.theRefStrip;
    
    for (auto it=gp.PattCore.cbegin();it!=gp.PattCore.cend();++it){
      entry.pat_Case = it->first; 
      for (GoldenPattern::DetFreq::const_iterator idf = it->second.cbegin(); idf !=it->second.cend(); ++idf) {
        entry.patDet = idf->first;
        for (GoldenPattern::MFreq::const_iterator imf = idf->second.cbegin(); imf != idf->second.cend(); ++imf) {
          entry.posOrBend = imf->first;
          entry.freq = imf->second; 
          tree->Fill();
        }
      }
    }
  } 
  patternOutFile.Write();
  patternOutFile.Close();
   
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void PatternManager::writeXML(std::string fname){

  xercesc::DOMElement* theTopElement;
  xercesc::DOMDocument* theDoc ;
  xercesc::DOMImplementation* domImpl;
  xercesc::DOMWriter* domWriter;
  
  ///Init XML
  domImpl = DOMImplementationRegistry::getDOMImplementation(qtxml::_toDOMS("Range"));
  domWriter = (dynamic_cast<DOMImplementation*>(domImpl))->createDOMWriter();
  domWriter->canSetFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
  theDoc = domImpl->createDocument(0,qtxml::_toDOMS("OMTF"), 0);
  theTopElement = theDoc->getDocumentElement();
  /////////////

  //writeGlobalData(theDoc, theTopElement);

  dumpPatternsXML(theDoc, theTopElement);

  /////////////
  ///Finialise and write XML to file
  XMLFormatTarget* formTarget = new LocalFileFormatTarget(fname.c_str());
  domWriter->setFeature(XMLUni::fgDOMWRTFormatPrettyPrint, true);
  domWriter->writeNode(formTarget, *theTopElement);
  delete formTarget;
  theDoc->release(); 
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void PatternManager::writeGlobalData(xercesc::DOMDocument* theDoc, 
				     xercesc::DOMElement* theTopElement){

  std::ostringstream stringStr;
  xercesc::DOMElement* aData=theDoc->createElement(qtxml::_toDOMS("GlobalData"));
  stringStr.str("");
  stringStr<<GoldenPattern::nBitsPdfAddr;
  aData->setAttribute(qtxml::_toDOMS("nPdfAddrBits"), qtxml::_toDOMS(stringStr.str()));
  stringStr.str("");
  stringStr<<GoldenPattern::nBitsVal;
  aData->setAttribute(qtxml::_toDOMS("nPdfValBits"), qtxml::_toDOMS(stringStr.str()));
  stringStr.str("");
  stringStr<<GoldenPattern::minP;
  aData->setAttribute(qtxml::_toDOMS("minPdfVal"), qtxml::_toDOMS(stringStr.str()));
  theTopElement->appendChild(aData);
  stringStr.str("");
  int nRefLayers = 8;
  stringStr<<theGPs.size()/nRefLayers;
  aData->setAttribute(qtxml::_toDOMS("nGPs"), qtxml::_toDOMS(stringStr.str()));
  theTopElement->appendChild(aData);

}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void PatternManager::dumpPatternsXML(xercesc::DOMDocument* theDoc, 
				     xercesc::DOMElement* theTopElement){

  std::ostringstream stringStr;
  xercesc::DOMElement* aGP=0, *aLayer=0, *aRefLayer=0, *aPdf=0;
  int iEtaCode = 1;
  int iPhiCode = 0;
  int nRefLayers = 8;

  //for(int iPtCode=31;iPtCode>5;--iPtCode){
  for(int iPtCode=20;iPtCode>15;--iPtCode){
    for(int iCharge=-1;iCharge<2;++++iCharge){     
      if(iCharge!=-1) continue;
      std::vector<std::vector<int> > meanDistPhiVec;
      std::vector<std::vector<int> > selDistPhiVec;
      std::vector<std::vector<int> > pdf;
      for(unsigned int iRefLayer=0;iRefLayer<refToLogicNumber.size();++iRefLayer){
	GoldenPattern::Key key(iEtaCode,0,0,0,0);
	key.theDet = refToLogicNumber[iRefLayer];
	key.thePtCode = iPtCode;
	key.theEtaCode = iEtaCode;
	key.thePhiCode = 0;
	key.theCharge = iCharge;
	if(theGPs.find(key)!=theGPs.end()){
	  GoldenPattern &myGP = theGPs[key];
	  std::vector<std::vector<int> > tmp = myGP.dump(0); 
	  if(!meanDistPhiVec.size()) meanDistPhiVec = tmp;
	  else meanDistPhiVec.insert(meanDistPhiVec.end(),tmp.begin(),tmp.end());
	  tmp = myGP.dump(1); 
	  if(!pdf.size()) pdf = tmp;
	  else pdf.insert(pdf.end(),tmp.begin(),tmp.end());
	  tmp = myGP.dump(2); 
	  if(!selDistPhiVec.size()) selDistPhiVec = tmp;
	  else selDistPhiVec.insert(selDistPhiVec.end(),tmp.begin(),tmp.end());
	}	
	else{
	  std::vector<int> dummyMeanDist(1);
	  std::vector<std::vector<int> > dummyVec(nLogicLayers,dummyMeanDist);
	  if(!meanDistPhiVec.size()) meanDistPhiVec = dummyVec;
	  else meanDistPhiVec.insert(meanDistPhiVec.end(),dummyVec.begin(),dummyVec.end());
	  std::vector<int> dummyLayer(128);
	  std::vector<std::vector<int> > dummyGP(nLogicLayers,dummyLayer);
	  if(!pdf.size()) pdf = dummyGP;
	  else pdf.insert(pdf.end(),dummyGP.begin(),dummyGP.end());
	  if(!selDistPhiVec.size()) selDistPhiVec = dummyGP;
	  else selDistPhiVec.insert(selDistPhiVec.end(),dummyGP.begin(),dummyGP.end());
	}	
      }     
      aGP = theDoc->createElement(qtxml::_toDOMS("GP"));
      stringStr.str("");
      stringStr<<iPtCode;
      aGP->setAttribute(qtxml::_toDOMS("iPt"), qtxml::_toDOMS(stringStr.str()));
      stringStr.str("");
      stringStr<<iEtaCode;
      aGP->setAttribute(qtxml::_toDOMS("iEta"), qtxml::_toDOMS(stringStr.str()));
      stringStr.str("");
      stringStr<<iPhiCode;
      aGP->setAttribute(qtxml::_toDOMS("iPhi"), qtxml::_toDOMS(stringStr.str()));
      stringStr.str("");
      stringStr<<iCharge;
      aGP->setAttribute(qtxml::_toDOMS("iCharge"), qtxml::_toDOMS(stringStr.str()));
      ////
      for(int iLayer = 0;iLayer<nLogicLayers;++iLayer){
	  int nOfPhis = 0;
	  aLayer = theDoc->createElement(qtxml::_toDOMS("Layer"));
	  stringStr.str("");
	  stringStr<<iLayer;
	  aLayer->setAttribute(qtxml::_toDOMS("iLayer"), qtxml::_toDOMS(stringStr.str()));
	  stringStr.str("");
	  stringStr<<nOfPhis;
	  aLayer->setAttribute(qtxml::_toDOMS("nOfPhis"), qtxml::_toDOMS(stringStr.str()));
	  for(int iRefLayer=0;iRefLayer<nRefLayers;++iRefLayer){
	    aRefLayer = theDoc->createElement(qtxml::_toDOMS("RefLayer"));
	    int meanDistPhi = meanDistPhiVec[iLayer+iRefLayer*nLogicLayers][0];	       
	    stringStr.str("");
	    stringStr<<meanDistPhi;
	    aRefLayer->setAttribute(qtxml::_toDOMS("meanDistPhi"), qtxml::_toDOMS(stringStr.str()));
	    int selDistPhi = 0;
	    stringStr.str("");
	    stringStr<<selDistPhi;
	    aRefLayer->setAttribute(qtxml::_toDOMS("selDistPhi"), qtxml::_toDOMS(stringStr.str()));
	    int selDistPhiShift = 0;
	    stringStr.str("");
	    stringStr<<selDistPhiShift;
	    aRefLayer->setAttribute(qtxml::_toDOMS("selDistPhiShift"), qtxml::_toDOMS(stringStr.str()));
	    int distMsbPhiShift = 0;
	    stringStr.str("");
	    stringStr<<distMsbPhiShift;
	    aRefLayer->setAttribute(qtxml::_toDOMS("distMsbPhiShift"), qtxml::_toDOMS(stringStr.str()));
	    aLayer->appendChild(aRefLayer);
	  }
	  for(int iRefLayer=0;iRefLayer<nRefLayers;++iRefLayer){
	    for(unsigned int iPdf=0;iPdf<pdf[iLayer+iRefLayer*nLogicLayers].size();++iPdf){
	      aPdf = theDoc->createElement(qtxml::_toDOMS("PDF"));
	      stringStr.str("0");
	      stringStr<<pdf[iLayer+iRefLayer*nLogicLayers][iPdf];
	      aPdf->setAttribute(qtxml::_toDOMS("value"), qtxml::_toDOMS(stringStr.str()));
	      aLayer->appendChild(aPdf);
	    }
	  }
	  aGP->appendChild(aLayer);
      }
      theTopElement->appendChild(aGP);
    }
  }
  ////
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
void PatternManager::readXMLConfig(std::string configFile){

  XercesDOMParser *parser = new XercesDOMParser();     
  parser->setValidationScheme(XercesDOMParser::Val_Auto);
  parser->setDoNamespaces(false);
  parser->parse(configFile.c_str()); 
  xercesc::DOMDocument* doc = parser->getDocument();
  assert(doc);
  unsigned int nElem = doc->getElementsByTagName(qtxml::_toDOMS("OMTF"))->getLength();
  std::cout<<"OMTF size: "<<nElem<<std::endl;
  if(nElem!=1){
    std::cout<<"Problem parsing XML file "<<configFile<<std::endl;
    exit(0);
  }
  DOMNode *aNode = doc->getElementsByTagName(qtxml::_toDOMS("OMTF"))->item(0);
  DOMElement* aOMTFElement = static_cast<DOMElement *>(aNode);  
  ///hw <-> logic numbering map
  nElem = aOMTFElement->getElementsByTagName(qtxml::_toDOMS("LayerMap"))->getLength();
  std::cout<<"LayerMap size: "<<nElem<<std::endl;
  DOMElement* aLayerElement = 0;
  for(uint i=0;i<nElem;++i){
    aNode = aOMTFElement->getElementsByTagName(qtxml::_toDOMS("LayerMap"))->item(i);
    aLayerElement = static_cast<DOMElement *>(aNode); 
    int hwNumer = std::atoi(qtxml::_toString(aLayerElement->getAttribute(qtxml::_toDOMS("hwNumber"))).c_str());
    int logicNumber = std::atoi(qtxml::_toString(aLayerElement->getAttribute(qtxml::_toDOMS("logicNumber"))).c_str());
    hwToLogicLayer[hwNumer] = logicNumber;
    logicToHwLayer[logicNumber] = hwNumer;
    if(nLogicLayers<logicNumber) nLogicLayers = logicNumber;
  }
  ++nLogicLayers;//logic number in XML starts from 0.

  ///ref<->logic numberig map
  nElem = aOMTFElement->getElementsByTagName(qtxml::_toDOMS("RefLayerMap"))->getLength();
  std::cout<<"RefLayerMap size: "<<nElem<<std::endl;
  DOMElement* aRefLayerElement = 0;
  for(uint i=0;i<nElem;++i){
    aNode = aOMTFElement->getElementsByTagName(qtxml::_toDOMS("RefLayerMap"))->item(i);
    aRefLayerElement = static_cast<DOMElement *>(aNode); 
    int refLayer = std::atoi(qtxml::_toString(aRefLayerElement->getAttribute(qtxml::_toDOMS("refLayer"))).c_str());
    int logicNumber = std::atoi(qtxml::_toString(aRefLayerElement->getAttribute(qtxml::_toDOMS("logicNumber"))).c_str());
    refToLogicNumber[refLayer] = logicNumber;
  }
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


