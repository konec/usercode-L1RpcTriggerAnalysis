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

namespace {
  typedef struct {
    UInt_t key_det;
    unsigned int key_pt;
    unsigned int key_phi;
    int key_ch;
    unsigned int pat_Case;
    UInt_t patDet;
    int posOrBend;
    unsigned int freq; 
  } ENTRY;
}

PatternManager::PatternManager(const edm::ParameterSet &cfg) 
  : theConfig(cfg), theEvForPatCounter(0), theEvUsePatCounter(0) 

{}

PatternManager::~PatternManager()
{
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

void PatternManager::run(const EventObj* ev, const TrackObj * simu, const HitSpecObj * hitSpec,  const VDigiSpec & vDigi)
{

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
  if(stripRef<0) return;

  //float phiref = hitSpec->position().phi();
  float phiref = stripRef;
  float ptref  = simu->pt(); 
  int    chargeref = simu->charge();
  unsigned int detref =  hitSpec->rawId();

  GoldenPattern::Key key( detref, ptref, chargeref, phiref);
  //if(detref!=637602109 || chargeref<0 || phiref!=80) return;
  //std::cout<<key<<std::endl;

  Pattern pattern;
  theEvForPatCounter++;
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); ++is) {
    bool isOK = pattern.add(*is); 
    if (!isOK) return;
  }
  if (pattern.size() == 0) return; 
  theEvUsePatCounter++;

  if (theGPs.find(key)==theGPs.end()) theGPs[key]=GoldenPattern(key); 
  theGPs[key].add(pattern);

}

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
/*
  if (detref != 637602109 && detref != 637634877 &&
      detref != 637599914 && detref != 637632682 ) return candidate;

  bool precisePos = ( fabs(hitSpec->position().phi()-1.025) < 0.001);
  if (!precisePos) return candidate;
  if ( simu->pt() < 26. || simu->pt() > 27.  ) return candidate;

*/
//  std::cout <<" ------------------ EVENT: "<<*ev <<" number of digis: "<<vDigi.size()<< std::endl;
  std::vector<Pattern> vpattern(1);
  theEvForPatCounter++;
  static bool skipRpcData   = theConfig.getUntrackedParameter<bool>("skipRpcData",  false);
  static bool skipDtCscData = theConfig.getUntrackedParameter<bool>("skipDtCscData",false);
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); ++is) {
    DetId detId( is->first);
    if (skipRpcData   && detId.subdetId()==MuonSubdetId::RPC) continue;
    if (skipDtCscData && (detId.subdetId()==MuonSubdetId::DT || detId.subdetId()==MuonSubdetId::CSC) ) continue;
    if(detId.subdetId()==MuonSubdetId::DT && DTphDigiSpec(is->first, is->second).bxNum()!=0) continue; //AK. Correct?
//    std::cout << "adding------- "<< is-vDigi.begin()+1 <<" digi det: "<<is->first<<"(rpc:"<<(detId.subdetId()==MuonSubdetId::RPC)<<") data: "<<is->second<< std::endl; 
    Pattern::add(vpattern,*is);
//    std::cout <<" after vpattern has size: "<<vpattern.size() << std::endl;
    if (vpattern.size() > 100) break;
  }
  if (vpattern[0].size() == 0) return candidate;
  //std::cout <<" ------------------ END EVENT, NOW COMPARE, has #patterns: "<<vpattern.size()<<" vpattern[0].size="<<vpattern[0].size() << std::endl;

  GoldenPattern::Key thisKey(detref, ptref, chargeref, phiref );
  //std::cout << thisKey << std::endl;

  GoldenPattern::Result bestMatching;
  GoldenPattern::Key    bestKey;
  for (auto ip=vpattern.cbegin(); ip!= vpattern.cend();++ip) {
    const Pattern & pattern = *ip;
    //  std::cout << " HAS PATTERN "<<pattern << std::endl;
    for (std::map< GoldenPattern::Key, GoldenPattern>::iterator igps = theGPs.begin(); igps != theGPs.end(); ++igps) {
      //if (!(thisKey==igps->first)) continue;
      GoldenPattern & gp = igps->second;
      GoldenPattern::Result result = gp.compare(pattern);
      
      std::cout <<"PATT KEY: "<<igps->first<<" "<<result<<std::endl;
      if (bestMatching < result) {
	bestMatching = result;
	bestKey =  igps->first;
	//      std::cout <<" ----"<<" pt: "<< bestKey.ptValue()<<std::endl; 
      } 
      //else std::cout <<std::endl;
    }
  }

//  std::cout <<" ------------------ END COMPARE: " << std::endl;
//  std::cout <<"BEST KEY: "<<bestKey<<" "<< bestMatching<<std::cout <<" ######"<<" pt: "<< bestKey.ptValue()<<std::endl;
//  abort();
  if (bestMatching) {
    candidate.pt = bestKey.ptValue();
    candidate.eta = bestKey.etaValue();
    candidate.phi = bestKey.phiValue();
    candidate.q   = bestMatching.nMatchedTot();
    candidate.type = L1Obj::OTF;
  }
  return candidate;

}


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

    /*
    if(entry.key_det!=637602109 ||
       entry.key_pt!=7 ||
       entry.key_phi!=166 ||
       entry.key_ch!=-1) continue;
    */

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
  
  if (theConfig.exists("patternInpFile") && theConfig.exists("patternOutFile")){
    for (auto igps = theGPs.begin(); igps != theGPs.end();) {
      GoldenPattern & gp = igps->second;      
      if(!gp.purge()) {theGPs.erase(igps++); } else { ++igps; } 
    }
  }
  
  static ENTRY entry;
  TTree *tree = new TTree("FlatPatterns","FlatPatterns");
  tree->Branch("entry",&entry,"key_det/i:key_pt/i:key_phi/i:key_ch/I:pat_Case/i:patDet/i:posOrBend/I:freq/i");

  for (std::map< GoldenPattern::Key, GoldenPattern>::const_iterator igps = theGPs.begin(); igps != theGPs.end(); ++igps) {
    const GoldenPattern & gp = igps->second; 
    entry.key_det =  gp.theKey.theDet;
    entry.key_pt =  gp.theKey.thePtCode;
    entry.key_phi =  gp.theKey.thePhiCode;
    entry.key_ch =  gp.theKey.theCharge;
    
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

