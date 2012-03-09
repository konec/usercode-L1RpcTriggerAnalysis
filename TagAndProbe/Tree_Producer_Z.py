import FWCore.ParameterSet.Config as cms

process = cms.Process("TagProbe")
process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1),
)


# get list of input files from a file
# (255 file limit does not apply)
import FWCore.Utilities.FileUtils as FileUtils
mylist = FileUtils.loadListFromFile('filelist_Z_IsoMu24_2p1_1')


process.source = cms.Source("PoolSource",
fileNames = cms.untracked.vstring( *mylist )
)
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAny_cfi")
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorAlong_cfi")
process.load("TrackPropagation.SteppingHelixPropagator.SteppingHelixPropagatorOpposite_cfi")
process.load("RecoMuon.DetLayers.muonDetLayerGeometry_cfi")

process.load('Configuration.StandardSequences.GeometryExtended_cff')
process.load("Configuration.StandardSequences.Reconstruction_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#process.GlobalTag.globaltag = 'GR_R_42_V14::All'
#process.GlobalTag.globaltag = 'GR_R_42_V21::All'
process.GlobalTag.globaltag = 'GR_R_44_V12::All'
from MuonAnalysis.MuonAssociators.TnPTreeProducer_separation_variables_Z_cfi import *

process.tnpTreeZ = tnpTreeZProducer.clone()
###################
treeProducer = process.tnpTreeZ

from MuonAnalysis.TagAndProbe.nearbyMuonsInfo_cfi import nearbyMuonsInfo
nearbyMuonsInfo.filename = cms.string("Debug_Z_IsoMu24_2p1_1.root")
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
#####################

process.TFileService = cms.Service("TFileService", fileName = cms.string('rfio:/castor/cern.ch/user/r/rybinska/Feb/Z_IsoMu24_2p1_1.root'))  

process.tnpSequence = cms.Sequence(
getattr(process, tpp+"DiMuonInfo")+
process.tnpTreeZ 
)
from MuonAnalysis.TagAndProbe.jpsi.tp_from_skim_common_cff import addDiMuonSeparationVariables
addDiMuonSeparationVariables(process, process.tnpSequence, process.tnpTreeZ)

process.p = cms.Path(
  process.tnpSequence
#process.tnpTreeZ 
)

process.outpath = cms.EndPath()
## -----------------------------------------------------------
# initialize MessageLogger and output report
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1
process.options   = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True),
)


print process.p
