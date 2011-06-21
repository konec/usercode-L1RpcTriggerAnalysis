import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

# get list of input files from a file
# (255 file limit does not apply)
import FWCore.Utilities.FileUtils as FileUtils
#mylist = FileUtils.loadListFromFile(_LIST_OF_FILES_) 
process.source = cms.Source("PoolSource", 
fileNames = cms.untracked.vstring(''),
inputCommands=cms.untracked.vstring( 'keep *', 'drop *_hltL1GtObjectMap_*_*')
)

# apply lumi section mask from JSON file
import PhysicsTools.PythonAnalysis.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes
myLumis = LumiList.LumiList(filename='goodRuns.json').getCMSSWString().split(',')
process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
process.source.lumisToProcess.extend(myLumis)
process.source.skipBadFiles = cms.untracked.bool(True)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))
#process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 
#'file:/disk00/work/data/MinimimBias-MuonDPG_skim-May27thSkim_v3/863EAB8A-8D6E-DF11-9EFA-00304867402A.root'),
#'file:/disk00/work/data/Commissioning-MuonDPG_skim-May27thSkim_v2/1E1A93A9-B96D-DF11-848B-003048D47A2E.root'),
#'file:/disk00/work/data/Commissioning-MuonDPG_skim-May27thSkim_v2/96984B02-996D-DF11-BB4F-003048D460AE.root'),
#inputCommands=cms.untracked.vstring( 'keep *', 'drop *_hltL1GtObjectMap_*_*')
#)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('FWCore.MessageService.MessageLogger_cfi')
#process.load('Configuration.StandardSequences.GeometryDB_cff')
process.load('Configuration.StandardSequences.GeometryExtended_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
#process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')

#process.load("EventFilter.RPCRawToDigi.RPCSQLiteCabling_cfi")
#process.RPCCabling.connect = 'sqlite_file:RPCEMap2.db'
#process.es_prefer_RPCEMap = cms.ESPrefer("PoolDBESSource","RPCCabling");

process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'GR_P_V20::All'

process.load("L1TriggerConfig.L1GtConfigProducers.L1GtConfig_cff")
process.load("L1TriggerConfig.GMTConfigProducers.L1MuGMTParametersConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerScalesConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerPtScaleConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuGMTScalesConfig_cff")

process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Components.test.dqm_onlineEnv_cfi")
process.dqmSaver.convention = 'Online'
process.dqmEnv.subSystemFolder = 'R2DTEST'


process.load("FWCore.MessageLogger.MessageLogger_cfi")
'''
process.MessageLogger = cms.Service("MessageLogger",
    #debugModules = cms.untracked.vstring('rpcunpacker','rpcMonitorLinkSynchro'),
    #debugModules = cms.untracked.vstring('muonRPCDigis','rpc*','rpcMonitorRaw'),
    debugModules = cms.untracked.vstring(' '),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)
'''
process.MessageLogger.cerr.FwkReport.reportEvery = 5000
process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
    )

process.load("DQM.L1TMonitor.L1TRPCTF_cfi")
process.l1trpctf.rpctfSource =  cms.InputTag("gtDigis")

process.load("DQM.RPCMonitorClient.RPCFEDIntegrity_cfi")
process.load("DQM.RPCMonitorClient.RPCMonitorRaw_cfi")
process.rpcMonitorRaw.writeHistograms = cms.untracked.bool(True)
process.rpcMonitorRaw.histoFileName = cms.untracked.string("rawMonitor.root")


process.load("RecoMuon.GlobalTrackingTools.GlobalTrackQuality_cfi")
process.load("RecoMuon.MuonIdentification.refitMuons_cfi")
process.load("TrackingTools.TrackRefitter.globalMuonTrajectories_cff")


process.filterBX = cms.EDFilter("FilterBX", beamBX = cms.vuint32(1,100) )

process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
  vertexCollection = cms.InputTag('offlinePrimaryVertices'),
  minimumNDOF = cms.uint32(4) ,
  maxAbsZ = cms.double(15),	
  maxd0 = cms.double(2)	
)

process.filterL1_GM = cms.EDFilter("Filter_L1_GM", l1MuReadout = cms.InputTag("gtDigis"))

process.linkSynchroAnalysis =  cms.EDAnalyzer("LinkSynchroAnalysis",
  writeHistograms = cms.untracked.bool(True),
  histoFileName = cms.untracked.string("synchroAnalysis.root"),
  linkMonitorPSet = cms.PSet(
    useFirstHitOnly = cms.untracked.bool(True),
    dumpDelays = cms.untracked.bool(True)
  ),
  synchroSelectorMuon = cms.PSet( 
    muonColl= cms.string("muons"),
    minPt = cms.double(2.),  
    maxTIP = cms.double(0.5),
    maxEta = cms.double(1.55),
    beamSpot = cms.InputTag("offlineBeamSpot"),
    checkUniqueRecHitMatching = cms.bool(True)
  ),
#  synchroSelectorTrack =  cms.PSet( 
#    collection = cms.string("generalTracks"),
#    minPt = cms.double(2.),
#    maxTIP = cms.double(0.5),
#    beamSpot = cms.InputTag("offlineBeamSpot"),
#    checkUniqueRecHitMatching = cms.bool(True),
#    l1MuReadout = cms.InputTag("gtDigis"),
#    maxDeltaEta = cms.double(0.2),
#    maxDeltaPhi = cms.double(0.25),
#    rpcMatcherPSet =  cms.PSet( maxDeltaEta = cms.double(0.2), maxDeltaPhi = cms.double(0.25))
#  )
)

process.efficiencyTree = cms.EDAnalyzer("EfficiencyTree",
  histoFileName = cms.string("efficiencyHelper.root"),
  treeFileName = cms.string("efficiencyTree.root"),
  muonColl = cms.string("muons"),
  trackColl = cms.string("generalTracks"),
  beamSpot = cms.InputTag("offlineBeamSpot"),
  minPt = cms.double(2.),
  maxTIP = cms.double(0.5),
  maxEta = cms.double(1.55),
  l1MuReadout = cms.InputTag("gtDigis"),
  l1MuReadoutEmu = cms.InputTag("gtDigis"),
  rpcMatcherPSet =  cms.PSet( maxDeltaEta = cms.double(0.5), maxDeltaPhi = cms.double(0.5)), 
  dtcscMatcherPSet = cms.PSet( maxDeltaEta = cms.double(0.1), maxDeltaPhi = cms.double(0.1))
)

process.p = cms.Path( 
#  process.filterBX*
  process.primaryVertexFilter*
  process.gtDigis*
  process.filterL1_GM* 
  process.muonRPCDigis*
  #process.rpcFEDIntegrity*process.rpcMonitorRaw*
  #process.linkSynchroAnalysis*
  process.globalMuons*
  process.efficiencyTree)
