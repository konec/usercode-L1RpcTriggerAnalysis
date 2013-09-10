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

////
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
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
    unsigned int key_strip;
    int key_ch;   
    unsigned int pat_Case;
    UInt_t patDet;
    int posOrBend;
    unsigned int freq; 
  } ENTRY;
}

PatternManager::PatternManager(const edm::ParameterSet &cfg) 
  : theConfig(cfg), theEvForPatCounter(0), theEvUsePatCounter(0), phiMapDone(false) 
{}

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

}
////////////////////////////////////////////
////////////////////////////////////////////
void PatternManager::makePhiMap(const edm::EventSetup& es){

  if(phiMapDone) return;
  phiMapDone = true;

  edm::ESHandle<RPCGeometry> rpcGeometry;
  es.get<MuonGeometryRecord>().get(rpcGeometry);

  for (auto igps = theGPs.begin(); igps != theGPs.end(); ++igps){
    igps->second.makeIntegratedCache();
    //Create phi map of Golden Patterns. Rotate key to use S6 symmetry.
    //Encode rotation degree in the pt code 
    for(int iStep=0;iStep<1;++iStep){
      const RPCRoll* roll = rpcGeometry->roll(Pattern::rotateDetId(igps->first.theDet,iStep));
      unsigned int detref = igps->first.theDet;
      unsigned int stripRef = igps->first.theRefStrip;
      unsigned int stripFrom = 0;
      unsigned int stripTo = 0;
      if(stripRef%2==0){
        stripFrom = stripRef/2;
        stripTo = stripFrom;
      }
      else{
        stripFrom = (stripRef-1)/2;
        stripTo = stripFrom+1;
      }
      LocalPoint aLocalPoint1 = roll->centreOfStrip((int)stripFrom);
      LocalPoint aLocalPoint2 = roll->centreOfStrip((int)stripTo);
      GlobalPoint aGlobalPoint1 = rpcGeometry->idToDet(Pattern::rotateDetId(detref,iStep))->toGlobal(aLocalPoint1);
      GlobalPoint aGlobalPoint2 = rpcGeometry->idToDet(Pattern::rotateDetId(detref,iStep))->toGlobal(aLocalPoint2);
      float phi = ((float)aGlobalPoint2.phi() + (float)aGlobalPoint1.phi())/2.0;
      GoldenPattern::Key rotatedKey(Pattern::rotateDetId(igps->first.theDet,iStep), 
				    0,
				    igps->first.theCharge, 
				    phi);
      rotatedKey.theRotation = iStep;
      rotatedKey.thePhiCode = igps->first.thePhiCode;      
      /*
      while  (phi < 0) { phi+=2*M_PI; }
      unsigned int phiCode = round( phi * 2*1152./(2*M_PI));      
      //if(igps->first.thePhiCode!=36 || phiCode!=36) continue;
      if(phiCode!=36) continue;
      if(abs(igps->first.thePhiCode-phiCode)>0){
	std::cout
	  //<<rotatedKey<<std::endl
	         <<" igps->first.thePhiCode: "<< igps->first.thePhiCode
		 <<" from geometry: "<<phiCode
		 <<" stripRef: "<<stripRef
		 <<std::endl;
	//std::cout<<igps->second<<std::endl;       
	//continue;
      }
      */
      theGPsPhiMap.insert(std::pair<GoldenPattern::Key,GoldenPattern::Key>(rotatedKey,igps->first));
    }
  }
}
////////////////////////////////////////////
////////////////////////////////////////////
void PatternManager::run(const EventObj* ev,  const edm::EventSetup& es,
			   const TrackObj * simu, const HitSpecObj * hitSpec,  
			   const VDigiSpec & vDigi){

  if (theConfig.exists("patternInpFile") && 
      theConfig.exists("patternOutFile")) return;

  if (!hitSpec) return;
  if (hitSpec->rawId() == 0 ) return;

  int stripRef = -1;
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); ++is) {   
    if(is->first!=hitSpec->rawId()) continue;
    RPCDigiSpec digi(is->first, is->second);
    stripRef = digi.halfStrip();   
  }
  ///Use only events with a digi in reference station
  if(stripRef<0){
    //std::cout<<"phiref: "<<hitSpec->position().phi()<<std::endl;
    return;
  }

  float phiref = hitSpec->position().phi();
  float ptref  = simu->pt(); 
  int    chargeref = simu->charge();
  unsigned int detref =  hitSpec->rawId();

  /*
  unsigned int stripFrom = 0;
  unsigned int stripTo = 0;
  if(stripRef%2==0){
    stripFrom = stripRef/2;
    stripTo = stripFrom;
  }
  else{
    stripFrom = (stripRef-1)/2;
    stripTo = stripFrom+1;
  }
  
  edm::ESHandle<RPCGeometry> rpcGeometry;
  es.get<MuonGeometryRecord>().get(rpcGeometry);
  const RPCRoll* roll = rpcGeometry->roll(detref);
  LocalPoint aLocalPoint1 = roll->centreOfStrip((int)stripFrom);
  LocalPoint aLocalPoint2 = roll->centreOfStrip((int)stripTo);
  GlobalPoint aGlobalPoint1 = rpcGeometry->idToDet(detref)->toGlobal(aLocalPoint1);
  GlobalPoint aGlobalPoint2 = rpcGeometry->idToDet(detref)->toGlobal(aLocalPoint2);
  phiref = ((float)aGlobalPoint2.phi() + (float)aGlobalPoint1.phi())/2.0;
  */
  GoldenPattern::Key key(detref, ptref, chargeref, phiref);
  key.theRefStrip = stripRef;

  Pattern pattern;
  theEvForPatCounter++;
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); ++is) {
    DetId detId( is->first);    
    if(detId.subdetId()==MuonSubdetId::DT && DTphDigiSpec(is->first, is->second).bxNum()!=0) continue;
    bool isOK = pattern.add(*is); 
    if (!isOK) return;
  }
  if (pattern.size() == 0) return; 
  theEvUsePatCounter++;

  if (theGPs.find(key)==theGPs.end()) theGPs[key]=GoldenPattern(key); 
  theGPs[key].add(pattern);
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
L1Obj PatternManager::check(const EventObj* ev, const TrackObj * simu, const HitSpecObj * hitSpec,  const VDigiSpec & vDigi)
{
  L1Obj candidate;

  if (theConfig.exists("patternInpFile") && 
      theConfig.exists("patternOutFile")) return candidate;

  if (!hitSpec) return candidate;
  if (hitSpec->rawId() == 0 ) return candidate;
  float phiref = hitSpec->position().phi();
  float ptref  = simu->pt();
  int    chargeref = simu->charge();
  unsigned int detref =  hitSpec->rawId();
  
  VDigiSpec::const_iterator is=vDigi.end();
  for (is = vDigi.begin(); is!=vDigi.end(); ++is) {   
    if(is->first==hitSpec->rawId()) break;
  }
  if(is==vDigi.end())return candidate; //Require a digi in reference station. Creates inefficiency!

  theEvForPatCounter++;
  
  RPCDigiSpec digiRef(is->first, is->second);
  phiref = digiRef.halfStrip();

  Pattern pattern;
  static bool skipRpcData   = theConfig.getUntrackedParameter<bool>("skipRpcData",  false);
  static bool skipDtCscData = theConfig.getUntrackedParameter<bool>("skipDtCscData",false);
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); ++is) {
    DetId detId( is->first);
    if (skipRpcData   && detId.subdetId()==MuonSubdetId::RPC) continue;
    if (skipDtCscData && (detId.subdetId()==MuonSubdetId::DT || detId.subdetId()==MuonSubdetId::CSC) ) continue;
    if(detId.subdetId()==MuonSubdetId::DT && DTphDigiSpec(is->first, is->second).bxNum()!=0) continue;
    pattern.add(*is); 
  }
  if (pattern.size() == 0) return candidate; 
  
  GoldenPattern::Key thisKey(detref, ptref, chargeref,phiref);
  GoldenPattern::Result bestMatching;
  GoldenPattern::Key    bestKey;

  /*
  std::cout << thisKey << std::endl;  
  RPCDetId aId(637632554);
  std::cout<<aId<<std::endl;
  uint32_t rawIdRotated = Pattern::rotateDetId(thisKey.theDet,1);  
  RPCDetId bId(rawIdRotated);
  std::cout<<rawIdRotated<<" "<<bId<<std::endl;
  RPCDetId cId(637602109);
  std::cout<<637602109<<" "<<cId<<std::endl;
  */

  ///Find rotation degree to match the base set of patterns. This still can be optimised.
  //if(theGPs.find(thisKey)== theGPs.end()) return candidate;
  //auto beginIt = theGPs.find(thisKey);
  //auto endIt = ++theGPs.find(thisKey);

  auto beginIt = theGPs.begin();
  auto endIt =   theGPs.end();
  Pattern rotatedPattern;
  int phiRotation = -1;
  for (std::map< GoldenPattern::Key, GoldenPattern>::iterator igps = beginIt; igps != endIt; ++igps) {
    ///Rotate the hit pattern to match the golden pattern      
    ///For the moment a single rotation is good for all golden patterns,
    ///so we make rotation only once
    if(phiRotation==-1){
      for(int iStep=0;iStep<5;++iStep){
	uint32_t rawIdRotated = Pattern::rotateDetId(digiRef.rawId(),iStep);
	if(rawIdRotated==igps->first.theDet && igps->first.thePhiCode==digiRef.halfStrip()){
	  //if(rawIdRotated==igps->first.theDet){
	  rotatedPattern = pattern.getRotated(iStep);
	  phiRotation = iStep;
	  break;
	}
      }
    }
    if(phiRotation<0) continue;
    /*
      if(phiRotation!=0){
      std::cout<<"phiRotation: "<<phiRotation<<std::endl;
      std::cout<<"detId: "<<hitSpec->rawId()<<std::endl;
      std::cout<<"digiRef.halfStrip(): "<<digiRef.halfStrip()<<std::endl;
      }
    */
    GoldenPattern::Key thisRotatedKey(Pattern::rotateDetId(digiRef.rawId(),phiRotation), ptref, chargeref,phiref);
    GoldenPattern & gp = igps->second;
    GoldenPattern::Result result = gp.compare(rotatedPattern);  
    /*
      std::cout<<"Step: "<<phiRotation<<std::endl;
      std::cout<<*ip<<std::endl;
      std::cout<<pattern<<std::endl;
      std::cout<<gp<<std::endl;
      //std::cout <<"PATT KEY: "<<igps->first<<" "<<result<<std::endl;
      */
    if (bestMatching < result) {
      bestMatching = result;
      bestKey =  igps->first;
      //      std::cout <<" ----"<<" pt: "<< bestKey.ptValue()<<std::endl; 
    } 
    //else std::cout <<std::endl;
  }

  //std::cout <<" ------------------ END COMPARE: " << std::endl;
  //std::cout <<"THIS KEY: "<<thisKey << std::endl;
  //std::cout <<"BEST KEY: "<<bestKey<<" "<< bestMatching<<std::endl;
  //abort();
  if (bestMatching) {
    candidate.pt = bestKey.ptValue();
    candidate.eta = bestKey.etaValue();
    candidate.phi = bestKey.phiValue();
    candidate.charge = bestKey.chargeValue();
    candidate.q   = bestMatching.nMatchedTot();
    candidate.type = L1Obj::OTF;
  }
  return candidate;  
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
L1Obj PatternManager::checkNew(const EventObj* ev, const TrackObj * simu, 
			       const HitSpecObj * hitSpec,  const HitSpecObj * hitSpecSt1,  
			       const VDigiSpec & vDigi){
  
  L1Obj candidate;

  if (theConfig.exists("patternInpFile") && 
      theConfig.exists("patternOutFile")) return candidate;

  if(!hitSpec) return candidate;   
  if (hitSpec->rawId() == 0) return candidate;
  
  int    chargeref = simu->charge();
  float phiref = hitSpec->position().phi();
  unsigned int detref =  hitSpec->rawId();
  /*  
  VDigiSpec::const_iterator is=vDigi.end();
  for (is = vDigi.begin(); is!=vDigi.end(); ++is) {   
    if(is->first==hitSpec->rawId()) break;
  }
  if(is==vDigi.end())return candidate; //Require a digi in reference station. Creates inefficiency!
  RPCDigiSpec digiRef(is->first, is->second);
  */
  theEvForPatCounter++;

  Pattern pattern;
  static bool skipRpcData   = theConfig.getUntrackedParameter<bool>("skipRpcData",  false);
  static bool skipDtCscData = theConfig.getUntrackedParameter<bool>("skipDtCscData",false);
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); ++is) {
    DetId detId( is->first);
    if (skipRpcData   && detId.subdetId()==MuonSubdetId::RPC) continue;
    if (skipDtCscData && (detId.subdetId()==MuonSubdetId::DT || detId.subdetId()==MuonSubdetId::CSC) ) continue;
    if(detId.subdetId()==MuonSubdetId::DT && DTphDigiSpec(is->first, is->second).bxNum()!=0) continue;
    pattern.add(*is); 
  }
  pattern.makeHitDetsList();
  if (pattern.size() == 0) return candidate; 

  GoldenPattern::Key thisKey(detref, 0, chargeref,phiref);
  GoldenPattern::Result bestMatching;
  GoldenPattern::Key    bestKey;

  auto aRange = theGPsPhiMap.equal_range(thisKey);
  auto beginIt = aRange.first;
  auto endIt =   aRange.second;


  ///Rotation stored in keys in maps correposnds to key rotation,
  ///we want to rotate the hit pattern into opposite direction.
  int theRotation = 6 - theGPsPhiMap.find(thisKey)->first.theRotation;
  Pattern rotatedPattern = pattern.getRotated(theRotation);

  for (auto igps = beginIt; igps != endIt; ++igps) {
    GoldenPattern::Result result =  theGPs[igps->second].compare(rotatedPattern);
    if (bestMatching < result) {
      bestMatching = result;
      bestKey =  igps->second;
    } 
  }
  
  //RPCDetId aId(637620906);
  //std::cout<<aId<<std::endl;
  if(!bestMatching){
    std::cout<<"--- BEGIN ---"<<std::endl;
    std::cout<<thisKey<<std::endl;
    std::cout<<pattern<<std::endl;
    std::cout<<" bestMatching: "<<bestMatching<<std::endl;
    if(theGPsPhiMap.find(thisKey)!=theGPsPhiMap.end()){
      std::cout<<"golden: "<<theGPsPhiMap.find(thisKey)->second<<std::endl;
    }
    for (auto igps = beginIt; igps != endIt; ++igps) {
      GoldenPattern::Result result =  theGPs[igps->second].compare(rotatedPattern);      
      std::cout<<"result: "<<result<<std::endl;
      std::cout<<theGPs[igps->second]<<std::endl;
      std::cout<<rotatedPattern<<std::endl;
    }
    std::cout<<"--- END ---"<<std::endl;
  }
  if(detref==637620906 && thisKey.thePhiCode==2237) exit(0);
  
  if (bestMatching) {
    candidate.pt = bestKey.ptValue();
    candidate.eta = bestKey.etaValue();
    candidate.phi = bestKey.phiValue();
    candidate.charge = bestKey.chargeValue();
    candidate.q   = bestMatching.nMatchedTot();
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
    key.theCharge =  entry.key_ch;
    key.theRefStrip =  entry.key_strip;

    GoldenPattern::PosBenCase pat_Case = static_cast<GoldenPattern::PosBenCase>(entry.pat_Case);
    if (theGPs.find(key)==theGPs.end()) theGPs[key]=GoldenPattern(key); 
    theGPs[key].add(pat_Case, entry.patDet, entry.posOrBend, entry.freq);
  }
 
  delete tree;
  patternInpFile.Close();

  /*
  float a;
  std::cout<<"Pause"<<std::endl;
  std::cin>>a;
  exit(0);
  */
}

void PatternManager::endJob(){

  if ( !theConfig.exists("patternOutFile") ) return;
  std::string patternOutFileName = theConfig.getParameter<std::string>("patternOutFile");
  TFile patternOutFile( patternOutFileName.c_str(),"RECREATE");   

  //if (theConfig.exists("patternInpFile") && theConfig.exists("patternOutFile")){
    for (auto igps = theGPs.begin(); igps != theGPs.end();) {
      GoldenPattern & gp = igps->second;      
      if(!gp.purge()) {theGPs.erase(igps++);} else { ++igps; } 
    }
    //}  

  static ENTRY entry;
  TTree *tree = new TTree("FlatPatterns","FlatPatterns");
  tree->Branch("entry",&entry,"key_det/i:key_pt/i:key_phi/i:key_strip/i:key_ch/I:pat_Case/i:patDet/i:posOrBend/I:freq/i");

  for (std::map< GoldenPattern::Key, GoldenPattern>::const_iterator igps = theGPs.begin(); igps != theGPs.end(); ++igps) {
    const GoldenPattern & gp = igps->second; 
    entry.key_det =  gp.theKey.theDet;
    entry.key_pt =  gp.theKey.thePtCode;
    entry.key_phi =  gp.theKey.thePhiCode;
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
//  delete tree;
}

