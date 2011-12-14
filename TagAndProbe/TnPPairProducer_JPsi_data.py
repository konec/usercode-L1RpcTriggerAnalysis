import FWCore.ParameterSet.Config as cms

import math

process = cms.Process("RPCTagAndProbe")
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.GeometryExtended_cff')
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
process.GlobalTag.globaltag = 'GR_R_44_V12::All'

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
	'/store/data/Run2011B/MuOnia/RECO/PromptReco-v1/000/178/101/CEA16BB9-5AF6-E011-B1A7-BCAEC518FF5A.root'
    ),
)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
############################################
############# P R E F I L T E R ############
############################################
## Removal of Beam Scraping Events // Collisions2010Recipes
process.noScraping = cms.EDFilter("FilterOutScraping",
    applyfilter = cms.untracked.bool(True),
    debugOn = cms.untracked.bool(False),
    numtrack = cms.untracked.uint32(10),
    thresh = cms.untracked.double(0.25)
)
############################################
## HLT Filter
import HLTrigger.HLTfilters.triggerResultsFilter_cfi
process.JPsiHLT = HLTrigger.HLTfilters.triggerResultsFilter_cfi.triggerResultsFilter.clone()
process.JPsiHLT.hltResults = cms.InputTag('TriggerResults', '', 'HLT')
process.JPsiHLT.l1tResults = cms.InputTag('')
process.JPsiHLT.throw      = cms.bool(False)
process.JPsiHLT.triggerConditions = cms.vstring('HLT_Mu5_Track2_Jpsi_v*','HLT_Mu7_Track7_Jpsi_v*')
############################################
## Primary Vertex Filter // Collisions2010Recipes
process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
    vertexCollection = cms.InputTag('offlinePrimaryVertices'),
    minimumNDOF = cms.uint32(4) ,
    maxAbsZ = cms.double(24),	
    maxd0 = cms.double(2)	
)
############################################
## Global Muon Filter
process.globalMuFilter = cms.EDFilter("TrackCountFilter",
    src = cms.InputTag("globalMuons"),
    minNumber = cms.uint32(1)
)
############################################
##  Prefilter
process.preFilter = cms.Sequence(process.JPsiHLT *
                                 process.noScraping * 
                                 process.primaryVertexFilter +  
                                 process.globalMuFilter)
############################################                                 
##  l1muonsAnyBX - from L1extraParticles
from L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi import l1extraParticles
process.l1muonsAnyBX = l1extraParticles.clone(
centralBxOnly = cms.bool(True)
)   
#maczowanie RPC
#process.l1extraToRPCValueMap = cms.EDProducer("L1ExtraToRPCValueMapProducer",
#L1Extra = cms.InputTag("l1muonsAnyBX"),
#) 

############################################
##  patMuonsWithTrigger
process.load("MuonAnalysis.MuonAssociators.patMuonsWithTrigger_cff")
############################################
## tagMuons - from patMuonsWithTrigger
process.tagMuons = cms.EDFilter("PATMuonRefSelector",
  src = cms.InputTag("patMuonsWithTrigger"),
  cut = cms.string('isGlobalMuon && isTrackerMuon'\
                   +'&& abs(dB)<0.2 && globalTrack().normalizedChi2<10'\
                   +'&& globalTrack().hitPattern().numberOfValidTrackerHits>10'\
                   +'&& globalTrack().hitPattern().numberOfValidMuonHits>0'\
                   +'&& globalTrack().hitPattern().numberOfValidPixelHits>0'\
                   +'&& numberOfMatches>1'\
                   +'&& (chargedHadronIso()+neutralHadronIso()+photonIso())<0.1*pt'\
                   +'&& pt > 3 & abs(eta) < 2.1'\
                   +"&&(!triggerObjectMatchesByPath('HLT_Mu*_v*').empty()||!triggerObjectMatchesByPath('HLT_IsoMu*_v*').empty())")
)
############################################
##  goodTracks - from generalTracks
process.goodTracks = cms.EDFilter("TrackSelector",
    src = cms.InputTag("generalTracks"),
    cut = cms.string("pt > 3 && abs(eta) < 2.1"),
)
############################################
## selectedLayer1TrackCands - from goodTracks
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi")
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi")
process.load("RecoMuon.DetLayers.muonDetLayerGeometry_cfi")
process.load("PhysicsTools.PatAlgos.patSequences_cff")
from PhysicsTools.PatAlgos.tools.trackTools import *
process.selectedLayer1TrackCands = cms.EDProducer("ConcreteChargedCandidateProducer",
src = cms.InputTag("goodTracks"),
particleType = cms.string('mu+'),   # to fix mass hypothesis
)
############################################
## l1MuTkMatch - from l1muonsAnyBX and selectedLayer1TrackCands

process.l1MuTkMatch = process.muonL1Info.clone()
process.l1MuTkMatch.src     = cms.InputTag('selectedLayer1TrackCands') 
process.l1MuTkMatch.matched = cms.InputTag('l1muonsAnyBX')
process.l1MuTkMatch.maxDeltaPhi = 0.3
process.l1MuTkMatch.maxDeltaR   = 1
process.l1MuTkMatch.maxDeltaEta = 0.4      
process.l1MuTkMatch.writeExtraInfo = cms.bool(True)

############################################
## trackMuonMatch from selectedLayer1TrackCands and l1MuTkMatch muToTkMatch
from PhysicsTools.PatAlgos.producersLayer1.genericParticleProducer_cfi import patGenericParticles
process.trackMuonMatch = patGenericParticles.clone(
    src = cms.InputTag("selectedLayer1TrackCands")
)
process.trackMuonMatch.userData.userCands.src += [cms.InputTag("l1MuTkMatch") ]
#process.trackMuonMatch.userData.userFloats.src += [ cms.InputTag("l1MuTkMatch","quality")]
#process.trackMuonMatch.userData.userFloats.src += [ cms.InputTag("l1MuTkMatch","RpcPtCode")]
############################################
## tagProbesJPsi from tagMuons and trackMuonMatch => JPsi
process.tagProbesJPsi = cms.EDProducer("CandViewShallowCloneCombiner",
decay = cms.string("tagMuons@+ trackMuonMatch@-"), # charge coniugate states are implied; 'tagMuons' and 'trkProbes' should be collections of Candidates
cut   = cms.string("2.8 < mass < 3.5"),                   
)
############################################
## tagProbesFilter from tagProbesMuTrk 
process.tagProbesJPsiFilter = cms.EDFilter("PATCandViewCountFilter",
minNumber = cms.uint32(1),
maxNumber = cms.uint32(999999),
src = cms.InputTag("tagProbesJPsi")
)
############################################
from MuonAnalysis.MuonAssociators.TnPTreeProducer_separation_variables_cfi import *
process.tnpTreeJPsi = tnpTreeJPsiProducer.clone()
treeProducer = process.tnpTreeJPsi
from MuonAnalysis.TagAndProbe.nearbyMuonsInfo_cfi import nearbyMuonsInfo;
tpp = treeProducer.tagProbePairs.moduleLabel
setattr(process, tpp+"DiMuonInfo", nearbyMuonsInfo.clone(src = treeProducer.tagProbePairs))

if not hasattr(treeProducer, 'pairVariables'):
    treeProducer.pairVariables = cms.PSet()
    treeProducer.pairFlags     = cms.PSet()
treeProducer.pairVariables.dphiVtxTimesQ = cms.InputTag(tpp+"DiMuonInfo", "dphiVtxTimesQ")
treeProducer.pairVariables.drM2          = cms.InputTag(tpp+"DiMuonInfo", "drM2")
treeProducer.pairVariables.dphiM2        = cms.InputTag(tpp+"DiMuonInfo", "dphiM2")
treeProducer.pairVariables.distM2        = cms.InputTag(tpp+"DiMuonInfo", "distM2")
treeProducer.pairVariables.drStaIn       = cms.InputTag(tpp+"DiMuonInfo", "drStaIn")
treeProducer.pairVariables.dphiStaIn     = cms.InputTag(tpp+"DiMuonInfo", "dphiStaIn")
process.TFileService = cms.Service("TFileService", fileName = cms.string('TnPTree_JPsi.root'))
process.tnpSequence = cms.Sequence(
getattr(process, tpp+"DiMuonInfo")+
process.tnpTreeJPsi
)
from MuonAnalysis.TagAndProbe.jpsi.tp_from_skim_common_cff import addDiMuonSeparationVariables
addDiMuonSeparationVariables(process, process.tnpSequence, process.tnpTreeJPsi)



############################################
########### P A T H ########################
############################################
process.p = cms.Path(
   process.preFilter *
   process.l1muonsAnyBX *
   process.patMuonsWithTriggerSequence* 
   process.tagMuons *
   process.goodTracks *
   process.selectedLayer1TrackCands *
   process.l1MuTkMatch *
   process.trackMuonMatch *
   process.tagProbesJPsi *
   process.tagProbesJPsiFilter *
   process.tnpSequence
)
process.outpath = cms.EndPath()
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.options   = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True),
)

 
print process.p

