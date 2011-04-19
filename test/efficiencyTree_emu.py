import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

# get list of input files from a file
# (255 file limit does not apply)
import FWCore.Utilities.FileUtils as FileUtils
mylist = FileUtils.loadListFromFile(_LIST_OF_FILES_) 
process.source = cms.Source("PoolSource", 
fileNames = cms.untracked.vstring( *mylist )
inputCommands=cms.untracked.vstring( 'keep *', 'drop *_hltL1GtObjectMap_*_*')
)

# apply lumi section mask from JSON file
import PhysicsTools.PythonAnalysis.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes
myLumis = LumiList.LumiList(filename=_JSON_FILE_).getCMSSWString().split(',')
process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
process.source.lumisToProcess.extend(myLumis)

#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))
#process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 
#'file:/afs/cern.ch/cms/L1/rpc/EffWithEmu/CMSSW_4_1_4_patch2/src/UserCode/L1RpcTriggerAnalysis/test/emu.root'),
#)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.StandardSequences.GeometryExtended_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'GR_R_311_V2::All'

process.load("L1TriggerConfig.L1GtConfigProducers.L1GtConfig_cff")
process.load("L1TriggerConfig.GMTConfigProducers.L1MuGMTParametersConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerScalesConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerPtScaleConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuGMTScalesConfig_cff")

process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Components.test.dqm_onlineEnv_cfi")
process.dqmSaver.convention = 'Online'
process.dqmEnv.subSystemFolder = 'R2DTEST'

process.MessageLogger = cms.Service("MessageLogger",

    debugModules = cms.untracked.vstring(' '),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)

process.load("DQM.L1TMonitor.L1TRPCTF_cfi")
process.l1trpctf.rpctfSource =  cms.InputTag("gmtDigis")

process.load("DQM.RPCMonitorClient.RPCFEDIntegrity_cfi")
process.load("DQM.RPCMonitorClient.RPCMonitorRaw_cfi")
process.rpcMonitorRaw.writeHistograms = cms.untracked.bool(True)
process.rpcMonitorRaw.histoFileName = cms.untracked.string("rawMonitor.root")

process.load("RecoMuon.GlobalTrackingTools.GlobalTrackQuality_cfi")
process.load("RecoMuon.MuonIdentification.refitMuons_cfi")
process.load("TrackingTools.TrackRefitter.globalMuonTrajectories_cff")

process.efficiencyTree = cms.EDAnalyzer("EfficiencyTree",
  histoFileName = cms.string("efficiencyHelper.root"),
  treeFileName = cms.string("efficiencyTree_emu_test.root"),
  muonColl = cms.string("muons"),
  trackColl = cms.string("generalTracks"),
  beamSpot = cms.InputTag("offlineBeamSpot"),
  minPt = cms.double(2.),
  maxTIP = cms.double(0.5),
  maxEta = cms.double(1.55),
  l1MuReadout = cms.InputTag("gmtDigis"),
  l1MuReadoutEmu = cms.InputTag("gmtDigis"),
  rpcMatcherPSet =  cms.PSet( maxDeltaEta = cms.double(0.5), maxDeltaPhi = cms.double(0.5)), 
  dtcscMatcherPSet = cms.PSet( maxDeltaEta = cms.double(0.1), maxDeltaPhi = cms.double(0.1))
)

process.p = cms.Path( 
  process.globalMuons*
  process.efficiencyTree)
