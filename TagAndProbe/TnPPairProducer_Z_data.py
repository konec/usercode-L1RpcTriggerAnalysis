import FWCore.ParameterSet.Config as cms

import math

process = cms.Process("RPCTagAndProbe")
### standard includes
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.GeometryExtended_cff')
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))
### global tag
process.GlobalTag.globaltag = 'GR_R_44_V12::All'

# get list of input files from a file
# (255 file limit does not apply)
#import FWCore.Utilities.FileUtils as FileUtils
#mylist = FileUtils.loadListFromFile(_LIST_OF_FILES_)
#process.source = cms.Source("PoolSource",
#fileNames = cms.untracked.vstring( *mylist )
#)

# apply lumi section mask from JSON file
#import PhysicsTools.PythonAnalysis.LumiList as LumiList
#import FWCore.ParameterSet.Types as CfgTypes
#myLumis = LumiList.LumiList(filename=_JSON_FILE_).getCMSSWString().split(',')
#process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
#process.source.lumisToProcess.extend(myLumis)


process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
    
    '/store/data/Run2011A/MuOnia/RECO/PromptReco-v4/000/167/913/8CCB1E2C-78A3-E011-957C-BCAEC532972B.root'

    )
)
### number of events
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
process.ZHLT = HLTrigger.HLTfilters.triggerResultsFilter_cfi.triggerResultsFilter.clone()
process.ZHLT.hltResults = cms.InputTag('TriggerResults', '', 'HLT')
process.ZHLT.l1tResults = cms.InputTag('')
process.ZHLT.throw      = cms.bool(False)
process.ZHLT.triggerConditions = cms.vstring('HLT_IsoMu24_eta2p1_v*')
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
## -0- Prefilter
process.preFilter = cms.Sequence(process.ZHLT *
                                 process.noScraping * 
                                 process.primaryVertexFilter +  
                                 process.globalMuFilter)
############################################                                 
## -1- l1muonsAnyBX - from L1extraParticles
from L1Trigger.L1ExtraFromDigis.l1extraParticles_cfi import l1extraParticles
process.l1muonsAnyBX = l1extraParticles.clone(
centralBxOnly = cms.bool(True)
)   
#maczowanie RPC
#process.l1extraToRPCValueMap = cms.EDProducer("L1ExtraToRPCValueMapProducer",
#L1Extra = cms.InputTag("l1muonsAnyBX"),
#) 

############################################
## -2- patMuonsWithTrigger
process.load("MuonAnalysis.MuonAssociators.patMuonsWithTrigger_cff")
############################################
## -3- tagMuons - from patMuonsWithTrigger
process.tagMuons = cms.EDFilter("PATMuonRefSelector",
  src = cms.InputTag("patMuonsWithTrigger"),
  cut = cms.string('isGlobalMuon && isTrackerMuon'\
                   +'&& abs(dB)<0.2 && globalTrack().normalizedChi2<10'\
                   +'&& globalTrack().hitPattern().numberOfValidTrackerHits>10'\
                   +'&& globalTrack().hitPattern().numberOfValidMuonHits>0'\
                   +'&& globalTrack().hitPattern().numberOfValidPixelHits>0'\
                   +'&& numberOfMatches>1'\
                   +'&& (chargedHadronIso()+neutralHadronIso()+photonIso())<0.1*pt'\
                   +'&& pt > 3 && abs(eta) < 2.1'\
                   +"&&(!triggerObjectMatchesByPath('HLT_IsoMu24_eta2p1_v*',1,1).empty())")
)
############################################
## -4- goodTracks - from generalTracks
process.goodTracks = cms.EDFilter("TrackSelector",
    src = cms.InputTag("generalTracks"),
   cut = cms.string("pt > 3 && abs(eta) < 2.1&&numberOfValidHits > 11 && hitPattern.pixelLayersWithMeasurement > 1 && normalizedChi2 < 4 && abs(d0) < 3 && abs(dz) < 30"),
)
############################################
## -5- selectedLayer1TrackCands - from goodTracks
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")
process.load("PhysicsTools.PatAlgos.patSequences_cff")
from PhysicsTools.PatAlgos.tools.trackTools import *
process.selectedLayer1TrackCands = cms.EDProducer("ConcreteChargedCandidateProducer",
src = cms.InputTag("goodTracks"),
particleType = cms.string('mu+'),   # to fix mass hypothesis
)
############################################
## -6- l1MuTkMatch - from l1muonsAnyBX and selectedLayer1TrackCands
process.l1MuTkMatch = process.muonL1Info.clone()
process.l1MuTkMatch.src     = cms.InputTag('selectedLayer1TrackCands') 
process.l1MuTkMatch.matched = cms.InputTag('l1muonsAnyBX')
process.l1MuTkMatch.resolveAmbiguitiesByMatchQuality = cms.bool(True)
process.l1MuTkMatch.sortByRes = cms.string("deltaR")
process.l1MuTkMatch.maxDeltaPhi = 0.3
process.l1MuTkMatch.maxDeltaR   = 1
process.l1MuTkMatch.maxDeltaEta = 0.4       
process.l1MuTkMatch.writeExtraInfo = cms.bool(True)
############################################
## -8- trackMuonMatch from selectedLayer1TrackCands and l1MuTkMatch muToTkMatch
from PhysicsTools.PatAlgos.producersLayer1.genericParticleProducer_cfi import patGenericParticles
process.trackMuonMatch = patGenericParticles.clone(
    src = cms.InputTag("selectedLayer1TrackCands")
)
process.trackMuonMatch.userData.userCands.src += [cms.InputTag("l1MuTkMatch") ]
#process.trackMuonMatch.userData.userFloats.src += [ cms.InputTag("l1MuTkMatch","quality")]
#process.trackMuonMatch.userData.userFloats.src += [ cms.InputTag("l1MuTkMatch","RpcPtCode")]
############################################
## -9- tagProbesZ from tagMuons and trackMuonMatch => Z
process.tagProbesZ = cms.EDProducer("CandViewShallowCloneCombiner",
decay = cms.string("tagMuons@+ trackMuonMatch@-"), # charge coniugate states are implied; 'tagMuons' and 'trkProbes' should be collections of Candidates
cut   = cms.string("60 < mass < 120"),                   
)
############################################
## -10- tagProbesFilter from tagProbesMuTrk 
process.tagProbesZFilter = cms.EDFilter("PATCandViewCountFilter",
minNumber = cms.uint32(1),
maxNumber = cms.uint32(999999),
src = cms.InputTag("tagProbesZ")
)
############################################
########### P A T H ########################
############################################
process.p = cms.Path(
    process.preFilter +
    process.l1muonsAnyBX *
   process.patMuonsWithTriggerSequence* 
   process.tagMuons *
   process.goodTracks *
   process.selectedLayer1TrackCands *
   process.l1MuTkMatch *
   process.trackMuonMatch *
   process.tagProbesZ +
   process.tagProbesZFilter 

)
############################################
########### O U T P U T ####################
############################################
process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('TnPPairs_data.root'),
    outputCommands = cms.untracked.vstring('drop *',
        'keep patMuons_patMuonsWithTrigger__*',                    
        'keep edmTriggerResults_TriggerResults_*_*',             
        'keep l1extraL1MuonParticles_l1extraParticles_*_*',
        'keep *_hltTriggerSummaryAOD_*_*',                 
        'keep *_l1extraRPC_*_*',
        'keep *_tagMuons_*_*',
        'keep *_goodTracks_*_*',
        'keep *_selectedLayer1TrackCands_*_*',
        'keep *_l1muonsAnyBX_*_*',
        'keep *_l1MuTkMatch_*_*',
        'keep *_trackMuonMatch_*_*',
        'keep *_tagProbesZ_*_*',
        'keep *_gtDigis_*_*',
        'keep *_offlinePrimaryVertices__*',                   
        'keep *_offlineBeamSpot__*',                        
        'keep recoTrackExtras_standAloneMuons_*_*',
        'keep *_*_*_RPCTagAndProbe'       
    ),
    SelectEvents = cms.untracked.PSet( SelectEvents = cms.vstring('p') )
)
process.e = cms.EndPath(process.out)
