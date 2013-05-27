#include "L1RpcTreeAnalysis.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TProfile.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObjBXExtra.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/SynchroCountsObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/DetCluDigiObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TriggerMenuResultObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/HitSpecObj.h"


#include "UserCode/L1RpcTriggerAnalysis/interface/ConverterRPCRawSynchroSynchroCountsObj.h"

L1RpcTreeAnalysis::L1RpcTreeAnalysis(const edm::ParameterSet & cfg)
  : theConfig(cfg),
    theAnaMuonDistribution(0), 
    theAnaRpcVsOth(0), 
    theAnaRpcMisc(0), 
    theAnaEff(0), 
    theAnaDet(0), 
    theAnaEmu(0), 
    theAnaSynch(0), 
    theAnaClu(0), 
    theAnaTimingL1(0), 
    theAnaMenu(0), 
    theAnaEvent(0),
    theAnaDigiSpec(0),
    theAnaHitSpec(0), 
    thePatternProducer(0),
    thePatternProvider(0),
    theAnaSiMuDistribution(0),
    theAnaOtfEff(0)
{ 
  if (theConfig.exists("anaMuonDistribution")) theAnaMuonDistribution = new AnaMuonDistribution( cfg.getParameter<edm::ParameterSet>("anaMuonDistribution"));
  if (theConfig.exists("anaMenu")) theAnaMenu = new AnaMenu(theConfig.getParameter<edm::ParameterSet>("anaMenu"));
  if (theConfig.exists("anaTimingL1")) theAnaTimingL1 = new AnaTimingL1( theConfig.getParameter<edm::ParameterSet>("anaTimingL1") );
  if (theConfig.exists("anaEvent")) theAnaEvent = new   AnaEvent(cfg.getParameter<edm::ParameterSet>("anaEvent") );
  if (theConfig.exists("anaDigiSpec")) theAnaDigiSpec = new AnaDigiSpec(cfg.getParameter<edm::ParameterSet>("anaDigiSpec")); 
  if (theConfig.exists("anaHitSpec")) theAnaHitSpec = new AnaHitSpec(cfg.getParameter<edm::ParameterSet>("anaHitSpec")); 
  if (theConfig.exists("patternProducer")) thePatternProducer = new PatternManager(cfg.getParameter<edm::ParameterSet>("patternProducer"));
  if (theConfig.exists("patternProvider")) thePatternProvider = new PatternManager(cfg.getParameter<edm::ParameterSet>("patternProvider"));
  if (theConfig.exists("anaSiMuDistribution")) theAnaSiMuDistribution = new AnaSiMuDistribution( cfg.getParameter<edm::ParameterSet>("anaSiMuDistribution"));
  if (theConfig.exists("anaOtfEff")) theAnaOtfEff = new AnaOtfEff( cfg.getParameter<edm::ParameterSet>("anaOtfEff"));
  if (theConfig.exists("anaEff")) theAnaEff = new   AnaEff(cfg.getParameter<edm::ParameterSet>("anaEff") );
  if (theConfig.exists("anaRpcVsOth")) theAnaRpcVsOth = new   AnaRpcVsOth(cfg.getParameter<edm::ParameterSet>("anaRpcVsOth") );
  
}

void L1RpcTreeAnalysis::beginJob()
{
  theHistos.SetOwner();

  if (theAnaMuonDistribution) theAnaMuonDistribution->init(theHistos);
  if (theAnaRpcVsOth)         theAnaRpcVsOth->init(theHistos);
  if (theAnaRpcMisc)          theAnaRpcMisc->init(theHistos);
  if (theAnaEff)              theAnaEff->init(theHistos);
  if (theAnaDet)              theAnaDet->init(theHistos);
  if (theAnaEmu)              theAnaEmu->init(theHistos);
  if (theAnaSynch)            theAnaSynch->init(theHistos);
  if (theAnaClu)              theAnaClu->init(theHistos);
  if (theAnaTimingL1)         theAnaTimingL1->init(theHistos);
  if (theAnaEvent)            theAnaEvent->init(theHistos);
  if (theAnaMenu)             theAnaMenu->init(theHistos);
  if (theAnaDigiSpec)         theAnaDigiSpec->init(theHistos);
  if (theAnaHitSpec)          theAnaHitSpec->init(theHistos);
  if (theAnaSiMuDistribution) theAnaSiMuDistribution->init(theHistos);
  if (theAnaOtfEff)           theAnaOtfEff->init(theHistos);

  if (thePatternProvider)     thePatternProvider->beginJob();
}

void L1RpcTreeAnalysis::beginRun(const edm::Run& ru, const edm::EventSetup& es)
{
  if (theAnaSynch) theAnaSynch->beginRun(ru,es);
}

void L1RpcTreeAnalysis::analyze(const edm::Event&, const edm::EventSetup&)
{
  //
  // prevent  multievent execution
  //
  static bool the_EMERGENCY_STOP = false;
  if (the_EMERGENCY_STOP) {
    std::cout <<"**  L1RpcTreeAnalysis::analyze EMERGENCY_STOP fired, skip" << std::endl;
    return;
  }
  the_EMERGENCY_STOP = true;

  // 
  // define input chain
  //
  TChain chain("tL1Rpc");
  std::vector<std::string> treeFileNames = theConfig.getParameter<std::vector<std::string> >("treeFileNames");
  for (std::vector<std::string>::const_iterator it = treeFileNames.begin(); it != treeFileNames.end(); ++it)  chain.Add((*it).c_str() );


  //
  // prepare datastructures and branches
  //
  std::vector<SynchroCountsObj> *counts= 0;
  std::vector<uint32_t> *detsCrossedByMuon = 0;
  std::vector<uint32_t> *detsCrossedByMuonDeepInside = 0;
  std::vector<DetCluDigiObj> *detsHitsCompatibleWithMuon = 0;
  std::vector<uint32_t> *detsSIMU = 0;
  std::vector<std::pair<uint32_t, uint32_t> > *digSpec = 0;

  EventObj * event = 0;
  MuonObj * muon = 0;
  TrackObj * simu = 0;

  TriggerMenuResultObj *bitsL1  = 0;
  TriggerMenuResultObj *bitsHLT = 0;

  TBranch *bcounts=0;
  TBranch *bdetsCrossedByMuon =0;
  TBranch *bdetsCrossedByMuonDeepInside =0;
  TBranch *bdetsHitsCompatibleWithMuon = 0;
  TBranch *bdetsSIMU =0;
  TBranch *bdigSpec = 0;

  L1ObjColl* l1ObjColl = 0;
  HitSpecObj* hitSpec = 0;
  

  chain.SetBranchAddress("event",&event);
  chain.SetBranchAddress("muon",&muon);
  chain.SetBranchAddress("simu",&simu);

  chain.SetBranchAddress("bitsL1",&bitsL1);
  chain.SetBranchAddress("bitsHLT",&bitsHLT);
  
  chain.SetBranchAddress("counts",&counts,&bcounts);
  chain.SetBranchAddress("detsCrossedByMuon",&detsCrossedByMuon,&bdetsCrossedByMuon);
  chain.SetBranchAddress("detsCrossedByMuonDeepInside",&detsCrossedByMuonDeepInside,&bdetsCrossedByMuonDeepInside);
  chain.SetBranchAddress("detsHitsCompatibleWithMuon",&detsHitsCompatibleWithMuon,&bdetsHitsCompatibleWithMuon);
  chain.SetBranchAddress("detsSIMU",&detsSIMU,&bdetsSIMU);
  chain.SetBranchAddress("digSpec",&digSpec,&bdigSpec);

  chain.SetBranchAddress("l1ObjColl",&l1ObjColl);
  chain.SetBranchAddress("hitSpec",&hitSpec);


  //
  // number of events
  //
  Int_t nentries = (Int_t) chain.GetEntries();
  std::cout <<" ENTRIES: " << nentries << std::endl;


  //
  // main loop
  //
  unsigned int lastRun = 0;
  for (int ev=0; ev<nentries; ev++) {
    chain.GetEntry(ev);
    if (theAnaMenu) theAnaMenu->updateMenu(bitsL1->names, bitsHLT->names);

//    if (ev < 44055) continue;
    if ( (lastRun != (*event).run) || (ev/1000*1000==ev) ) { 
//    if (true) {
//    if (! ((*event).run==204601 && (*event).id ==109463402)) { continue;
      lastRun = (*event).run; 
      std::cout <<"RUN:"    << std::setw(7) << (*event).run
                <<" event:" << std::setw(8) << ev
                <<" done:"  << std::setw(6)<< std::setiosflags(std::ios::fixed) << std::setprecision(2) << ev*100./nentries<<"%";
      std::cout<<std::endl; 
    }

/*   
   if (     event->id == 597978012 
        ||  event->id == 14791798  
        ||  event->id == 436261807 
        ||  event->id == 873776280  
        ||  event->id == 960307389 
        ||  event->id == 150097556 
        ||  event->id == 42062946 
        ||  event->id == 1064121551 
        ||  event->id == 1499489591 
        ||  event->id == 60161631
        ||  event->id == 624624695
        ||  event->id == 727298833
        ||  event->id == 835600542
        ||  event->id == 216373776
        ||  event->id == 1107585611
        ||  event->id == 197907027
      ) theAnaMenu.debug = true; else theAnaMenu.debug = false;
*/

   // EVENT NUMBER, BX structure etc.
   EventObjBXExtra eventBx(*event);
   if ( theAnaEvent && !theAnaEvent->filter(&eventBx) && theConfig.getParameter<bool>("filterByAnaEvent") ) continue;
   // ANALYSE AND FILTER KINEMCTICS 
   if ( theAnaMuonDistribution && !theAnaMuonDistribution->filter(muon) && theConfig.getParameter<bool>("filterByAnaMuonDistribution") ) continue;
   // ANALYSE AND FILTER TRIGGER MENU
   if ( theAnaMenu && !theAnaMenu->filter(event, muon, bitsL1, bitsHLT) && theConfig.getParameter<bool>("filterByAnaMenu") ) continue;
   // ANALYSE AND FILTER SIMU KONEMATICs
   if ( theAnaSiMuDistribution && !theAnaSiMuDistribution->filter(event, simu, hitSpec) && theConfig.getParameter<bool>("filterByAnaSiMuDistribution") ) continue;

   const TrackObj * refTrack = theConfig.getParameter<bool>("useSiMuReference") ? simu : muon;

   // ANALYSES 
   if (theAnaRpcVsOth) theAnaRpcVsOth->run(refTrack,l1ObjColl);

//   std::cout <<"refTrack: "<< *refTrack<<std::endl;
//   std::cout <<"l1ObjColl: "<< *l1ObjColl << std::endl;

   if (theAnaRpcMisc)  theAnaRpcMisc->run(event,muon,l1ObjColl);
   if (theAnaDet)      theAnaDet->run( muon, *detsHitsCompatibleWithMuon,  *detsCrossedByMuon, *detsCrossedByMuonDeepInside);
   if (theAnaEmu)      theAnaEmu->run ( event, muon, l1ObjColl);
   if (theAnaSynch)    theAnaSynch->run( event, muon, ConverterRPCRawSynchroSynchroCountsObj::toRawSynchro( *counts));
   if (theAnaClu)      theAnaClu->run( event, muon, l1ObjColl, *detsHitsCompatibleWithMuon);
   if (theAnaTimingL1) theAnaTimingL1->run( &eventBx, muon, l1ObjColl);

   // HITPATTERN ANALYSES & OTF EFFICIENCY
   if (theAnaHitSpec) theAnaHitSpec->run(event, simu, hitSpec);
   if (theAnaDigiSpec) theAnaDigiSpec->run(event, simu, hitSpec, *digSpec);
   if (thePatternProducer) thePatternProducer->run(event, simu, hitSpec, *digSpec);
   L1Obj l1otf;
   if (thePatternProvider) l1otf=thePatternProvider->check(event, simu, hitSpec, *digSpec);
   if (theAnaOtfEff) theAnaOtfEff->run(event,simu,l1otf);  
   L1ObjColl myL1ObjColl = *l1ObjColl;
   myL1ObjColl.push_back(l1otf, false, 0.); 
   if (theAnaEff)      theAnaEff->run(refTrack, &myL1ObjColl);
  }
}

void L1RpcTreeAnalysis::endJob()
{
  std::cout <<"ENDJOB, summaries:"<<std::endl;
  TGraph* hGraph_DetEff = (theAnaDet) ? theAnaDet->resume() : 0;
  TGraph* hGraph_RunClu = (theAnaClu) ? theAnaClu->resume(): 0;
  if (theAnaRpcMisc) theAnaRpcMisc->resume(theHistos);
  if (theAnaTimingL1)  theAnaTimingL1->resume(theHistos);
  if (theAnaMenu)     theAnaMenu->resume(theHistos);
  if (theAnaMenu) theAnaEvent->resume(theHistos);
  if (theAnaHitSpec) theAnaHitSpec->resume(theHistos);
  if (theAnaDigiSpec) theAnaDigiSpec->resume(theHistos);

  if (theAnaSynch) theAnaSynch->endJob();
  if (thePatternProducer) thePatternProducer->endJob();
  if (thePatternProvider) thePatternProvider->endJob();

  std::string histoFile = theConfig.getParameter<std::string>("histoFileName");
  TFile f(histoFile.c_str(),"RECREATE");
  theHistos.Write();
  if (hGraph_DetEff) hGraph_DetEff->Write("hGraph_DetEff");
  if (hGraph_RunClu) hGraph_RunClu->Write("hGraph_RunClu");
  f.Close();
  std::cout <<"END"<<std::endl;

  delete theAnaMuonDistribution;
  delete theAnaRpcVsOth;
  delete theAnaRpcMisc;
  delete theAnaEff;
  delete theAnaDet;
  delete theAnaEmu;
  delete theAnaSynch;
  delete theAnaClu;
  delete theAnaTimingL1;
  delete theAnaMenu;
  delete theAnaEvent;
  delete theAnaDigiSpec;
  delete theAnaHitSpec;
  delete thePatternProducer;
  delete thePatternProvider;
  delete theAnaSiMuDistribution;
  delete theAnaOtfEff;
}
