import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")
import os


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( -1) )
#
# For CRAB jobs leave fileNames vector empty.
# For processing single files insert lines with 'file:/PATH/FILE.root'
# (there is 255 file limit though).
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 
  '/store/2012C_SingleMu_RAW-RECO/0AE926CA-94CB-E111-A92F-00261834B51E.root',
  '/store/2012C_SingleMu_RAW-RECO/24ADA49F-89F5-E111-AFA6-E0CB4E1A118A.root'
))

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

#
# set proper GlobalTag
#
process.GlobalTag.globaltag = 'FT_R_53_V10::All' #rereco

#
# message logger
#
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger = cms.Service("MessageLogger",
  debugModules = cms.untracked.vstring(),
  destinations = cms.untracked.vstring('cout'),
  cout = cms.untracked.PSet( 
    threshold = cms.untracked.string('DEBUG'),
    default = cms.untracked.PSet ( reportEvery = cms.untracked.int32(100))
  )
)
#process.MessageLogger.debugModules.append('muonRPCDigis')
#process.MessageLogger.debugModules.append('rpcMonitorLinkSynchro')
#process.MessageLogger.debugModules.append('linkSynchroAnalysis')
#process.MessageLogger.debugModules.append('l1GtTrigReport')
#process.options = cms.untracked.PSet( wantSummary=cms.untracked.bool(False))

#
# privare rpc r2d
#
#process.load("EventFilter.RPCRawToDigi.RPCSQLiteCabling_cfi")
#process.RPCCabling.connect = 'sqlite_file:RPCEMap2.db'
#process.es_prefer_RPCEMap = cms.ESPrefer("PoolDBESSource","RPCCabling");

#
# Gt/Gmt
#
#process.load("L1TriggerConfig.L1GtConfigProducers.L1GtConfig_cff")
#process.load("L1TriggerConfig.GMTConfigProducers.L1MuGMTParametersConfig_cff")
#process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerScalesConfig_cff")
#process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerPtScaleConfig_cff")
#process.load("L1TriggerConfig.L1ScalesProducers.L1MuGMTScalesConfig_cff")

#
# rpc emulator 
#
process.load("L1TriggerConfig.RPCTriggerConfig.L1RPCConfig_cff")
process.load("L1Trigger.RPCTrigger.RPCConeConfig_cff")
process.load("L1Trigger.RPCTrigger.l1RpcEmulDigis_cfi")
process.rpcconf.filedir = cms.untracked.string('UserCode/L1RpcTriggerAnalysis/data/Paterny/Marcin_Wide/')
process.rpcconf.PACsPerTower = cms.untracked.int32(1)


#
# refit Muon
#
#process.load("RecoMuon.GlobalTrackingTools.GlobalTrackQuality_cfi")
#process.load("RecoMuon.MuonIdentification.refitMuons_cfi")
process.load("TrackingTools.TrackRefitter.globalMuonTrajectories_cff")


#
# a few filters to apply
#
#process.filterBX = cms.EDFilter("FilterBX", beamBX = cms.vuint32(1,100) )
process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
  vertexCollection = cms.InputTag('offlinePrimaryVertices'),
  minimumNDOF = cms.uint32(4) ,
  maxAbsZ = cms.double(15),	
  maxd0 = cms.double(2)	
)
process.filterMenu = cms.EDFilter("FilterMenu")
process.filterL1 = cms.EDFilter("FilterL1", l1MuReadout = cms.InputTag("gtDigis"))
process.filterGM = cms.EDFilter("FilterGM")

#
# RPC Raw data monitoring
#
process.load("DQM.RPCMonitorClient.RPCMonitorRaw_cfi")
process.rpcMonitorRaw.writeHistograms = cms.untracked.bool(False)

process.l1RpcTree = cms.EDAnalyzer("L1RpcTreeMaker",
  histoFileName = cms.string("l1RpcHelper.root"),
  treeFileName = cms.string("l1RpcTree.root"),
  detHitCollector = cms.PSet(),
  checkDestSIMU = cms.bool(False),
  bestMuonFinder = cms.PSet(
    muonColl = cms.string("muons"),
    beamSpot = cms.InputTag("offlineBeamSpot"),
    requireOuterTrack = cms.bool(False),  
    requireGlobalTrack = cms.bool(True),  
    minPt = cms.double(2.),
    maxTIP = cms.double(0.02),
    maxEta = cms.double(1.6),
    maxChi2Mu = cms.double(2.),
    maxChi2Tk = cms.double(2.),
    minNumberTrackerHits = cms.int32(10),
    minNumberRpcHits = cms.int32(0),
    minNumberDtCscHits = cms.int32(1),
    minNumberOfMatchedStations = cms.int32(2),
    deltaPhiUnique = cms.double(1.0),
    deltaEtaUnique = cms.double(0.5)
  ),
  linkSynchroGrabber = cms.PSet(
    writeHistograms = cms.untracked.bool(True),
    linkMonitorPSet = cms.PSet(
      useFirstHitOnly = cms.untracked.bool(True),
      dumpDelays = cms.untracked.bool(True) # set to True for LB delay plots
    ),
    synchroSelectorMuon = cms.PSet(
      muonColl= cms.string("muons"),
      minPt = cms.double(2.),
      maxTIP = cms.double(0.5),
      maxEta = cms.double(1.6),
      beamSpot = cms.InputTag("offlineBeamSpot"),
      checkUniqueRecHitMatching = cms.bool(True)
    ),
  ),
  l1MuReadout = cms.InputTag("gtDigis"),
  l1RpcEmu    = cms.InputTag("l1RpcEmulDigis"),
  matcherPSet =  cms.PSet( maxDeltaEta = cms.double(0.4), maxDeltaPhi = cms.double(0.3))
)

# print process.dumpPython()
#
# needed by DQM
#
process.load("DQMServices.Core.DQM_cfg")
process.load("DQM.L1TMonitor.L1TDEMON_cfi")
process.l1demon.disableROOToutput = cms.untracked.bool(False)
process.l1demon.HistFile = cms.untracked.string('l1RpcDqm.root')

#
# L1RPCT DQM 
#
process.load("DQM.L1TMonitor.L1TRPCTF_cfi")
process.l1tRpctf.rpctfSource =  cms.InputTag("gtDigis")

#
# DQM of RPCFedIntegrity
#
process.load("DQM.RPCMonitorClient.RPCFEDIntegrity_cfi")

#
# DQM DATA-EMU comparsion
#
process.load("L1Trigger.HardwareValidation.L1HardwareValidation_cff")
process.l1compare.RPCsourceData = cms.InputTag("gtDigis") 
process.l1compare.RPCsourceEmul = cms.InputTag("l1RpcEmulDigis") 
process.l1compare.COMPARE_COLLS = cms.untracked.vuint32(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0)


process.p = cms.Path(
  process.filterGM*
  process.muonRPCDigis*
  process.rpcMonitorRaw*process.rpcFEDIntegrity*
  process.l1tRpctf*
  process.l1RpcEmulDigis* 
  process.gtDigis*process.l1compare*  #FIXME-why without gtDigis it does not work???   
  process.l1demon*
  process.globalMuons*process.l1RpcTree
)

