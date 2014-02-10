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

PatternManager::PatternManager(const edm::ParameterSet &cfg) 
  : theConfig(cfg), theEvForPatCounter(0), theEvUsePatCounter(0), phiMapDone(false){

  myPhiConverter = 0;

}

PatternManager::~PatternManager(){
  std::cout <<" Events checked for pattenrs:    " << theEvForPatCounter << std::endl;
  std::cout <<" Events used    for pattenrs:    " << theEvUsePatCounter << std::endl;
  std::cout <<" Size of GoldenPatterns:         " << theGPs.size() << std::endl;

  if (theConfig.getUntrackedParameter<bool>("dump",false)) {
  for (std::map< GoldenPattern::Key, GoldenPattern>::const_iterator 
      im = theGPs.begin(); im!= theGPs.end(); ++im) {
    std::cout <<" GP: "<< (*im).second << std::endl;
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

  if (!hitSpec) return;

  int theEtaCode = L1RpcTriggerAnalysisEfficiencyUtilities::EtaScale::etaCode(simu->eta());
  float phiref = hitSpec->position().phi();
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

    int aLayer = myPhiConverter->getLayerNumber(is->first)+100*detId.subdetId();
    int iPhi = myPhiConverter->convert(*is);

    //int nDivisions = GoldenPattern::Key::nPhi;
    //if(iPhi<0) iPhi+=nDivisions;
    //Accept only events with single hit in each layer
    if(refPhi.find(aLayer)!=refPhi.end()) return; 
    refPhi[aLayer] = iPhi;
  }
  if (pattern.size() == 0) return; 

  theEvUsePatCounter++;
  ////////////////////////////////////////////  
  ///Create GPs for different reference layers
  for (auto const & it : refPhi){  

  GoldenPattern::Key key(theEtaCode, ptref, chargeref, phiref,stripRef);
  key.theDet = it.first;
  myPhiConverter->setReferencePhi((float)it.second/GoldenPattern::Key::nPhi*2*M_PI);

  if (theGPs.find(key)==theGPs.end()) theGPs[key]=GoldenPattern(key);
  theGPs[key].add(pattern, myPhiConverter);

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
			    const HitSpecObj * hitSpec,  const HitSpecObj * hitSpecSt1,  
			    const VDigiSpec & vDigi){

  L1Obj candidate;
 
  if (theConfig.exists("patternInpFile") && 
      theConfig.exists("patternOutFile")) return candidate;

  if(!myPhiConverter) myPhiConverter = new MtfCoordinateConverter(es);
  myPhiConverter->setReferencePhi(0);

  //int chargeref = simu->charge();
  //float phiref = hitSpec->position().phi();
  //int theEtaCode = L1RpcTriggerAnalysisEfficiencyUtilities::EtaScale::etaCode(simu->eta());
  //////////////////////
  theEvForPatCounter++;

  ////////////DEBUG
  /*
  std::cout.precision(4);
  std::cout<<"eta: "<<simu->eta()<<" phi: "<<hitSpec->position().phi()
	   <<" theEtaCode: "<<L1RpcTriggerAnalysisEfficiencyUtilities::EtaScale::etaCode(simu->eta())
	   <<" pt: "<<simu->pt()
	   <<std::endl;
  */
  //////////////////////
  std::vector<int> myActiveRefs = {301, 302, 303, 304, 101, 102, 312, 202};

  Pattern pattern;
  std::map<uint32_t,int> refPhi;
  static bool skipRpcData   = theConfig.getUntrackedParameter<bool>("skipRpcData",  false);
  static bool skipDtCscData = theConfig.getUntrackedParameter<bool>("skipDtCscData",false);
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); ++is) {
    DetId detId( is->first);
    if (skipRpcData   && detId.subdetId()==MuonSubdetId::RPC) continue;
    if (skipDtCscData && (detId.subdetId()==MuonSubdetId::DT || detId.subdetId()==MuonSubdetId::CSC) ) continue;    
    if(!pattern.add(*is)) continue; //For refernce use only layers with single hit (most likely too restrictive)
    int aLayer = myPhiConverter->getLayerNumber(is->first)+100*detId.subdetId();
    int iPhi = myPhiConverter->convert(*is);
    //int nDivisions = GoldenPattern::Key::nPhi;
    //if(iPhi<0) iPhi+=nDivisions;

    bool skipLayer = true;
    for(auto aRef : myActiveRefs){
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

  for (auto igps = theGPs.begin(); igps!=theGPs.end();++igps) {
    igps->second.makeIntegratedCache();
    for (auto const & it : refPhi){  
      if(igps->first.theDet!=it.first) continue;
      if(igps->first.theEtaCode<8 && 
	 (it.first==312 || it.first==202)) continue;
      if(igps->first.theEtaCode>7 && 
	 (it.first!=312 && it.first!=202)) continue;
      myPhiConverter->setReferencePhi((float)it.second/GoldenPattern::Key::nPhi*2*M_PI);	  
      GoldenPattern::Result result =  igps->second.compare(pattern,myPhiConverter);
      ////////////DEBUG      
      //std::cout<<igps->first<<" "<<result<<std::endl;
      //std::cout<<"Ref dig: "<<it.first<<" "<<it.second<<std::endl;
      //std::cout<<"GP: "<<igps->second<<std::endl;      
      ////////////////////
      if (bestMatching < result) {
	bestMatching = result;
	bestKey =  igps->first;
	bestKey.thePhiCode = it.second;       
	bestKey.theCharge = it.first;       
	} 	
    }
  }

 if (bestMatching && bestKey.ptValue()<10 && false) {
      ////////////DEBUG
      std::cout<<"Best match: "<<bestKey<<" "<<bestMatching<<std::endl;
      std::cout<<pattern<<std::endl;
      //////////////////
      //exit(0);
 }

  if (bestMatching) {
    candidate.pt = bestKey.ptValue();
    candidate.eta = bestKey.etaValue();
    //candidate.eta = bestMatching.value();
    candidate.phi = bestKey.phiValue();
    candidate.charge = bestKey.chargeValue();
    candidate.q   = bestMatching.nMatchedTot()
 	+ 100*bestMatching.withStation1()
        + 200*bestMatching.withStation2()    
        + 500*bestMatching.withStation3();    
    //candidate.disc = bestMatching.value();
    candidate.type = L1Obj::OTF;
  }

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
    GoldenPattern::Key key;
    key.theDet =     entry.key_det;
    key.thePtCode =  entry.key_pt;
    key.thePhiCode = entry.key_phi;
    key.theEtaCode = entry.key_eta;
    key.theCharge =  entry.key_ch;
    key.theRefStrip =  entry.key_strip;
    
    if(key.theRefStrip<1000) continue;
    //if(key.theEtaCode<9) continue;
    //if(!(key.thePtCode==30 || key.thePtCode<10)) continue;

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
  
  /*
  for (auto igps = theGPs.begin(); igps != theGPs.end();) {
    GoldenPattern & gp = igps->second;      
    if(!gp.purge()) {theGPs.erase(igps++);} else { ++igps; } 
  }
  */
  
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

  for (auto igps = theGPs.begin(); igps != theGPs.end();++igps) {
    GoldenPattern & gp = igps->second;      
    gp.plot();   
  }  
}

