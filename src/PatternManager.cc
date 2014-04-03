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
  theEtaCode = 0;
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

    for(int iGranularity=1;iGranularity<5;++iGranularity){
      int aLayer = myPhiConverter->getLayerNumber(is->first)+100*detId.subdetId() + 1000*iGranularity;
      int nPhi = GoldenPattern::Key::nPhi(aLayer);
      int iPhi = myPhiConverter->convert(*is,nPhi);
      //Accept only events with single hit in each layer
      if(refPhi.find(aLayer)!=refPhi.end()) return; 
      refPhi[aLayer] = iPhi;
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
  theGPs[key].add(GoldenPattern::PosBenCase::TOTDEV,501,pattern.deviationSum(myPhiConverter,nPhi),1);

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
  //////////////////////
  theEvForPatCounter++;

  std::vector<int> myActiveRefs = {301, 302, 303, 304, 101, 102, 312, 313, 202};
  //std::vector<int> myActiveRefs = {301, 302, 303, 304, 305, 101, 102, 103, 201, 202, 203, 311, 312, 313};
  //std::vector<int> myActiveRefs = {101, 102, 103, 201, 202, 203}; //noRPC
  //std::vector<int> myActiveRefs = {301, 302, 303, 304, 312, 313}; //no DT, CSC

  Pattern pattern;
  std::map<uint32_t,int> refPhi;
  static bool skipRpcData   = theConfig.getUntrackedParameter<bool>("skipRpcData",  false);
  static bool skipDtCscData = theConfig.getUntrackedParameter<bool>("skipDtCscData",false);
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); ++is) {
    DetId detId( is->first);
    if (skipRpcData   && detId.subdetId()==MuonSubdetId::RPC) continue;
    if (skipDtCscData && (detId.subdetId()==MuonSubdetId::DT || detId.subdetId()==MuonSubdetId::CSC) ) continue;    
    if(!pattern.add(*is)) continue; //For refernce use only layers with single hit (most likely too restrictive)


    for(int iGranularity=1;iGranularity<5;++iGranularity){
      int aLayer = myPhiConverter->getLayerNumber(is->first)+100*detId.subdetId() + 1000*iGranularity;
      int nPhi = GoldenPattern::Key::nPhi(aLayer);
      int iPhi = myPhiConverter->convert(*is,nPhi);
      if(iPhi<0) iPhi+=nPhi;

      bool skipLayer = true;
      for(auto aRef : myActiveRefs){
	if(aRef==aLayer%1000){
	  skipLayer = false;
	  break;
	}
      }
      if(skipLayer) continue;    
    refPhi[aLayer] = iPhi; //FIXME! Use multimap
    }
  }
  pattern.makeHitDetsList();
  if (pattern.size() == 0) return candidate; 

  GoldenPattern::Result bestMatching;
  GoldenPattern::Key    bestKey;

  ///High precision chambers first
  for (auto igps = theGPs.begin(); igps!=theGPs.end();++igps) {    
    if(igps->first.theDet%1000/100==3) continue;
    igps->second.makeIntegratedCache();
    for (auto const & it : refPhi){  
      if(igps->first.theDet!=it.first) continue;     
      int nPhi = GoldenPattern::Key::nPhi(it.first);
      myPhiConverter->setReferencePhi((float)it.second/nPhi*2*M_PI);	  
      GoldenPattern::Result result =  igps->second.compare(pattern,myPhiConverter);
      ////////////////////
      if (bestMatching < result) {
	bestMatching = result;
	bestKey =  igps->first;
	bestKey.thePhiCode = it.second;       
	bestKey.theCharge = it.first;       
	} 	
    }
  }
  ///Then RPC
  if(!bestMatching){
    for (auto igps = theGPs.begin(); igps!=theGPs.end();++igps) {
      igps->second.makeIntegratedCache();
      for (auto const & it : refPhi){  
	if(igps->first.theDet%1000/100!=3) continue;     
	if(igps->first.theDet!=it.first) continue;     
	int nPhi = GoldenPattern::Key::nPhi(it.first);
	myPhiConverter->setReferencePhi((float)it.second/nPhi*2*M_PI);	  
	GoldenPattern::Result result =  igps->second.compare(pattern,myPhiConverter);
	////////////////////
	if (bestMatching < result) {
	  bestMatching = result;
	  bestKey =  igps->first;
	  bestKey.thePhiCode = it.second;       
	  bestKey.theCharge = it.first;       
	} 	
      }
    }
  }

  if (false && bestMatching && bestKey.thePtCode<10 && bestKey.theCharge<2000) {
    ////////////DEBUG
    std::cout<<"eta: "<<simu->eta()<<" phi: "<<hitSpec->position().phi()<<std::endl;
    std::cout<<"Best match: "<<bestKey<<" "<<bestMatching<<std::endl;
    std::cout<<theGPs[bestKey]<<std::endl;   
    std::cout<<"-----------------"<<std::endl;

    for (auto igps = theGPs.begin(); igps!=theGPs.end();++igps) {
      for (auto const & it : refPhi){
	if(igps->first.theDet!=it.first) continue;  
	int nPhi = GoldenPattern::Key::nPhi(it.first);
	myPhiConverter->setReferencePhi((float)it.second/nPhi*2*M_PI);	  
	GoldenPattern::Result result =  igps->second.compare(pattern,myPhiConverter);
	if(igps->first.theDet%1000/100<3 && igps->first.thePtCode==17 && igps->first.theCharge==-1){
	//if(result.nMatchedTot()>=bestMatching.nMatchedTot() && igps->first.theDet%1000/100<3 && igps->first.thePtCode>10){
	//if(igps->first.thePtCode==17){
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
    if(entry.key_strip<50000) continue;
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
  
  for (auto igps = theGPs.begin(); igps != theGPs.end();++igps) {
    GoldenPattern & gp = igps->second;      
    gp.plot();   
  }    
}

