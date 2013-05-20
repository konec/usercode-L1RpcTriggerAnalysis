#include "UserCode/L1RpcTriggerAnalysis/interface/PatternManager.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"

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
  std::cout <<" Events used    for pattenrs:    " << theEvForPatCounter << std::endl;
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
  if (!hitSpec) return;
  if (hitSpec->rawId() == 0 ) return;
  double phiref = hitSpec->position().phi();
  double ptref  = simu->pt(); 
  int    chargeref = simu->charge();
  unsigned int detref =  hitSpec->rawId();
/*
  if (detref != 637602109 && detref != 637634877 &&
      detref != 637599914 && detref != 637632682 ) return;

  bool precisePos = ( fabs(hitSpec->position().phi()-1.025) < 0.001);
  if (!precisePos) return;
  if ( simu->pt() < 16.) return;
*/


  GoldenPattern::Key key( detref, ptref, chargeref, phiref);

  Pattern pattern;
  theEvForPatCounter++;
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); is++) {
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
  if (!hitSpec) return candidate;
/*
  if (hitSpec->rawId() == 0 ) return candidate;
  double phiref = hitSpec->position().phi();
  double ptref  = simu->pt();
  int    chargeref = simu->charge();
  unsigned int detref =  hitSpec->rawId();
  if (detref != 637602109 && detref != 637634877 &&
      detref != 637599914 && detref != 637632682 ) return candidate;

  bool precisePos = ( fabs(hitSpec->position().phi()-1.025) < 0.001);
  if (!precisePos) return candidate;
  if ( simu->pt() < 26. || simu->pt() > 27.  ) return candidate;

  std::cout <<" ------------------ EVENT: " << std::endl;
*/
  Pattern pattern;
  theEvForPatCounter++;
  for (VDigiSpec::const_iterator is= vDigi.begin(); is!=vDigi.end(); is++) {
    bool isOK = pattern.add(*is);
    if (!isOK) { return candidate; }
  }
  if (pattern.size() == 0) return candidate;
//  std::cout <<" ------------------ END EVENT, COMPARE" << std::endl;

//  GoldenPattern::Key thisKey(detref, ptref, chargeref, phiref );
//  std::cout << thisKey << std::endl;

  GoldenPattern::Result bestMatching;
  GoldenPattern::Key    bestKey;
  for (std::map< GoldenPattern::Key, GoldenPattern>::iterator igps = theGPs.begin(); igps != theGPs.end(); igps++) {
//    if (!(thisKey==igps->first)) continue;
//    std::cout << " HAS PATTERN "<<pattern << std::endl;
    GoldenPattern & gp = igps->second;
    GoldenPattern::Result result = gp.compare(pattern);
    if (!result.hasRpcDet(637602109) && !result.hasRpcDet(637634877) && !result.hasRpcDet(637599914) && !result.hasRpcDet(637632682)) continue;
    if (result > bestMatching) {
      bestMatching = result;
      bestKey =  igps->first;
    }
//    std::cout <<result << std::endl;
  }
//  std::cout <<" ------------------ END COMPARE: " << std::endl;
//  std::cout <<"BEST: "<< bestMatching<<" pt: "<<bestKey.ptValue()<<std::endl;
  if (bestMatching) {
    candidate.pt = bestKey.ptValue();
    candidate.eta = 1.;
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
  for (Int_t i=0; i<nentries; i++) {
    tree->GetEntry(i);
    GoldenPattern::Key key;
    key.theDet =     entry.key_det;
    key.thePtCode =  entry.key_pt;
    key.thePhiCode = entry.key_phi;
    key.theCharge =  entry.key_ch;
    GoldenPattern::PosBenCase pat_Case = static_cast<GoldenPattern::PosBenCase>(entry.pat_Case);
    if (theGPs.find(key)==theGPs.end()) theGPs[key]=GoldenPattern(key); 
    theGPs[key].add(pat_Case, entry.patDet, entry.posOrBend, entry.freq);
  }
  delete tree;
  patternInpFile.Close();
}

void PatternManager::endJob()
{
  for (std::map< GoldenPattern::Key, GoldenPattern>::iterator igps = theGPs.begin(); igps != theGPs.end(); igps++) {
    GoldenPattern & gp = igps->second;
    gp.purge();
  }

  if ( !theConfig.exists("patternOutFile") ) return;
  std::string patternOutFileName = theConfig.getParameter<std::string>("patternOutFile");
  TFile patternOutFile( patternOutFileName.c_str(),"RECREATE"); 

  static ENTRY entry;
  TTree *tree = new TTree("FlatPatterns","FlatPatterns");
  tree->Branch("entry",&entry,"key_det/i:key_pt/i:key_phi/i:key_ch/I:pat_Case/i:patDet/i:posOrBend/I:freq/i");

  for (std::map< GoldenPattern::Key, GoldenPattern>::const_iterator igps = theGPs.begin(); igps != theGPs.end(); igps++) {
    const GoldenPattern & gp = igps->second; 
    entry.key_det =  gp.theKey.theDet;
    entry.key_pt =  gp.theKey.thePtCode;
    entry.key_phi =  gp.theKey.thePhiCode;
    entry.key_ch =  gp.theKey.theCharge;
    for (unsigned int iCase =1; iCase <=5; ++iCase) {
      entry.pat_Case = iCase; 
      const GoldenPattern::DetFreq * detFreq = 0;
      if (iCase==GoldenPattern::POSRPC) detFreq = &gp.posRpc;
      if (iCase==GoldenPattern::POSCSC) detFreq = &gp.posCsc;
      if (iCase==GoldenPattern::BENCSC) detFreq = &gp.bendingCsc;
      if (iCase==GoldenPattern::POSDT ) detFreq = &gp.posDt;
      if (iCase==GoldenPattern::BENDT ) detFreq = &gp.bendingDt;
      for (GoldenPattern::DetFreq::const_iterator idf = detFreq->begin(); idf != detFreq->end(); idf++) {
        entry.patDet = idf->first;
        for (GoldenPattern::MFreq::const_iterator imf = idf->second.begin(); imf != idf->second.end(); imf++) {
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

