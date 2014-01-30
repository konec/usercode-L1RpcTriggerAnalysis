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
  ////Calculate total and average size of GPs
  int memSize = 0;
  for (auto im = theGPs.begin(); im!= theGPs.end(); ++im) {
    (*im).second.makeIntegratedCache();
    memSize+=(*im).second.size();
  }

  std::cout <<"Number of chambers connected:         " << memSize << std::endl;

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
void PatternManager::makePhiMap(const edm::EventSetup& es){

  if(phiMapDone) return;
  phiMapDone = true;

  es.get<MuonGeometryRecord>().get(rpcGeometry);

  for (auto igps = theGPs.begin(); igps != theGPs.end(); ++igps){
    igps->second.purge();
    igps->second.makeIntegratedCache();

    //Create phi map of Golden Patterns. Rotate key to use S6 symmetry.
    //Encode rotation degree in the pt code 
    for(int iStep=0;iStep<1;++iStep){      
      float phi = igps->first.phiValue() + iStep*60.0/180.0*M_PI;
      while  (phi < 0) { phi+=2*M_PI; }
      while  (phi > 2*M_PI) { phi-=2*M_PI; }
  
      GoldenPattern::Key rotatedKey(igps->first.theEtaCode, 
				    0,
				    igps->first.theCharge, 
				    phi,
				    igps->first.theRefStrip);
      rotatedKey.theRotation = iStep;
      //rotatedKey.theDet = Pattern::rotateDetId(igps->first.theDet,iStep);
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

  int theEtaCode = L1RpcTriggerAnalysisEfficiencyUtilities::EtaScale::etaCode(simu->eta());;
  float phiref = hitSpec->position().phi();
  float ptref  = simu->pt(); 
  int chargeref = simu->charge();
  unsigned int detref = 0;
  int stripRef = 0;

  int aRotation = 0;
  /*
  ///Rotate muons to a single 60deg sector
  if(phiref>0){
    while  (phiref<atan(-2) || phiref>M_PI/4.0){ 
      phiref-=M_PI/3.0; 
      aRotation++;
    }
  }
  if(phiref<0){
    while  (phiref<atan(-2) || phiref>M_PI/4.0){ 
      phiref+=M_PI/3.0; 
      aRotation++;
    }
  }
  aRotation=(6-aRotation)%6;
  */  
  ////////////////////////////////////////////  
  GoldenPattern::Key key(theEtaCode, ptref, chargeref, phiref,stripRef);
  key.theDet = detref;
  
  Pattern pattern;
  theEvForPatCounter++;
  if(!myPhiConverter) myPhiConverter = new MtfCoordinateConverter(es);
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); ++is) {
    DetId detId( is->first);    
    if(detId.subdetId()==MuonSubdetId::DT && DTphDigiSpec(is->first, is->second).bxNum()!=0) continue;
     
    bool isOK = pattern.add(*is); 
    if (!isOK) return;
  }
  if (pattern.size() == 0) return; 

  Pattern rotatedPattern = pattern.getRotated(aRotation);

  theEvUsePatCounter++;
  
  if (theGPs.find(key)==theGPs.end()) theGPs[key]=GoldenPattern(key);

  //float phiWidth = 2*M_PI/(1152.0); //RPC strip
  //float aSmear = (aRandom.Rndm()-0.5)*phiWidth;
  float aSmear = 0.0;

  myPhiConverter->setReferencePhi(phiref+aSmear);
  theGPs[key].add(rotatedPattern, myPhiConverter);

  ///Count number of events for each key
  if (aCounterMap.find(key)==aCounterMap.end()) aCounterMap[key]=1;
  else aCounterMap[key]++;
  theGPs[key].theKey.theRefStrip = aCounterMap[key];
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

  int chargeref = simu->charge();
  float phiref = hitSpec->position().phi();

  //int iPhiCodeMin = 0;
  //int iPhiCodeMax = GoldenPattern::Key::nPhi;

  int iEtaCodeMin = 0;
  int iEtaCodeMax = 9;

  int theEtaCode = L1RpcTriggerAnalysisEfficiencyUtilities::EtaScale::etaCode(simu->eta());

  iEtaCodeMin = theEtaCode;
  iEtaCodeMax = theEtaCode;

  if(iEtaCodeMin<0) iEtaCodeMin = 0;
  if(iEtaCodeMax>9) iEtaCodeMax = 9;

  float radius = 0.0;
  float maxPhi = phiref+radius;
  float minPhi = phiref-radius;
  while  (maxPhi < 0) {maxPhi+=2*M_PI; };
  while  (minPhi < 0) {minPhi+=2*M_PI; };
  while  (maxPhi > 2*M_PI) {maxPhi-=2*M_PI; };
  while  (minPhi > 2*M_PI) {minPhi-=2*M_PI; };
  //iPhiCodeMin = floor(minPhi * GoldenPattern::Key::nPhi/(2*M_PI));
  //iPhiCodeMax = floor(maxPhi * GoldenPattern::Key::nPhi/(2*M_PI));
  //////////////////////
  theEvForPatCounter++;

  Pattern pattern;
  static bool skipRpcData   = theConfig.getUntrackedParameter<bool>("skipRpcData",  false);
  static bool skipDtCscData = theConfig.getUntrackedParameter<bool>("skipDtCscData",false);
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); ++is) {
    DetId detId( is->first);
    if (skipRpcData   && detId.subdetId()==MuonSubdetId::RPC) continue;
    if (skipDtCscData && (detId.subdetId()==MuonSubdetId::DT || detId.subdetId()==MuonSubdetId::CSC) ) continue;    
    if(detId.subdetId()==MuonSubdetId::DT && 
       (DTphDigiSpec(is->first, is->second).bxNum()!=0 ||
	DTphDigiSpec(is->first, is->second).bxCnt()!=0 ||
	DTphDigiSpec(is->first, is->second).ts2()!=0 ||
	DTphDigiSpec(is->first, is->second).code()<4)
	) continue;    
    pattern.add(*is); 
  }
  pattern.makeHitDetsList();
  if (pattern.size() == 0) return candidate; 

  GoldenPattern::Result bestMatching;
  GoldenPattern::Key    bestKey;

  /*
  std::cout<<"eta: "<<simu->eta()<<" phi: "
	   <<maxPhi<<" ("<<maxPhi * GoldenPattern::Key::nPhi/(2*M_PI)<<")"
	   <<" theEtaCode: "<<theEtaCode
	   <<std::endl;
  */
  
  myPhiConverter->setReferencePhi(phiref);

  for(int iEtaCode=iEtaCodeMin;iEtaCode<=iEtaCodeMax;++iEtaCode){
    //for(int iPhiCode=iPhiCodeMin;iPhiCode!=iPhiCodeMax;iPhiCode = (++iPhiCode)%(GoldenPattern::Key::nPhi+1)){
    for(int iPhiCode=0;iPhiCode<1;++iPhiCode){
      
      GoldenPattern::Key thisKey(iEtaCode, 0, chargeref,phiref);      
      //thisKey.thePhiCode = iPhiCode;
      thisKey.thePhiCode = 0;
      
      auto aRange = theGPsPhiMap.equal_range(thisKey);
      auto beginIt = aRange.first;
      auto endIt =   aRange.second;
        
      for (auto igps = beginIt; igps != endIt; ++igps) {

	///Rotation stored in keys in maps correposnds to key rotation,
	///we want to rotate the hit pattern into opposite direction.
	int theRotation = (6 - igps->first.theRotation)%6;
	float thePhiCode =  igps->first.thePhiCode;
	
	Pattern rotatedPattern = pattern.getRotated(theRotation);

	float phiWidth = 2*M_PI/(GoldenPattern::Key::nPhi);//phi granularity
	myPhiConverter->setReferencePhi(phiref);
	myPhiConverter->setReferencePhi( (0+iPhiCode)*phiWidth );
	GoldenPattern::Result result =  theGPs[igps->second].compare(rotatedPattern,myPhiConverter);
        //break;
	//std::cout<<"iPhiCode: "<<iPhiCode<<std::endl;	
	//std::cout<<igps->second<<std::endl;
	//std::cout<< theGPs[igps->second]<<std::endl;
	//std::cout<<result<<std::endl;
      	
	if (bestMatching < result) {
	  bestMatching = result;
	  bestKey =  igps->second;
	  bestKey.thePhiCode = thePhiCode;       
	} 	
      }
    }     
  }

  if(bestKey.thePtCode<7 && false){
    std::cout<<*ev<<std::endl;
    std::cout<<"eta: "<<simu->eta()<<" phi: "
	     <<maxPhi<<" ("<<maxPhi * GoldenPattern::Key::nPhi/(2*M_PI)<<")"
	     <<" theEtaCode: "<<theEtaCode
	     <<std::endl;
    std::cout<<bestKey<<std::endl;
    std::cout<<bestMatching<<std::endl;
    GoldenPattern::Key thisKey(theEtaCode,simu->pt(), chargeref,phiref);      
    std::cout<<thisKey<<std::endl;
    std::cout<<theGPs[thisKey].compare(pattern,myPhiConverter)<<std::endl;
    GoldenPattern::Key thisKey1(theEtaCode,5.5, chargeref,phiref);      
    std::cout<<thisKey1<<std::endl;
    std::cout<<theGPs[thisKey1].compare(pattern,myPhiConverter)<<std::endl;
    std::cout<<pattern<<std::endl;
    std::cout<<theGPs[thisKey]<<std::endl;
  }
  if (!bestMatching && false){
    std::cout<<pattern<<std::endl;
    std::cout<<"aaaa "<<std::endl;
  }

  if (bestMatching) {
    candidate.pt = bestKey.ptValue();
    //candidate.eta = bestKey.etaValue();
    candidate.eta = bestMatching.value();
    candidate.phi = bestKey.phiValue();
    candidate.charge = bestKey.chargeValue();
    candidate.q   = bestMatching.nMatchedTot()
 	+ 100*bestMatching.withStation1()
        + 200*bestMatching.withStation2()    
        + 500*bestMatching.withStation3();    
    //candidate.disc = bestMatching.value();
    candidate.type = L1Obj::OTF;
  }

  //exit(0);

  return candidate;
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
float PatternManager::phiForDigi(uint32_t detref, unsigned int stripRef){

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

  const RPCRoll* roll = rpcGeometry->roll(detref);
  LocalPoint aLocalPoint1 = roll->centreOfStrip((int)stripFrom);
  LocalPoint aLocalPoint2 = roll->centreOfStrip((int)stripTo);
  GlobalPoint aGlobalPoint1 = rpcGeometry->idToDet(detref)->toGlobal(aLocalPoint1);
  GlobalPoint aGlobalPoint2 = rpcGeometry->idToDet(detref)->toGlobal(aLocalPoint2);
  float phiref = ((float)aGlobalPoint2.phi() + (float)aGlobalPoint1.phi())/2.0;
  return phiref;
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

    //if(key.theRefStrip<300 || key.theEtaCode<6 ||  key.theEtaCode>9) continue;
    //if(key.theRefStrip<100 || key.theEtaCode>3) continue;
    if(key.theRefStrip<10000) continue;

    GoldenPattern::PosBenCase pat_Case = static_cast<GoldenPattern::PosBenCase>(entry.pat_Case);
    if (theGPs.find(key)==theGPs.end()) theGPs[key]=GoldenPattern(key);
    theGPs[key].add(pat_Case, entry.patDet, entry.posOrBend, entry.freq);
  }

  delete tree;
  patternInpFile.Close();

}

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

  /*
  for (auto igps = theGPs.begin(); igps != theGPs.end();++igps) {
    GoldenPattern & gp = igps->second;      
    gp.plot();   
  }
  */
}

