import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")
import os


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32( -1) )
#
# For CRAB jobs leave fileNames vector empty.
# For processing single files insert lines with 'file:/PATH/FILE.root'
# (there is 255 file limit though).
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 
#  '/store/2012C_MinimumBias_RECO/6CCBE4DE-FEF4-E111-B54E-003048F118AA.root',
  '/store/2012C_SingleMu_RAW-RECO/0AE926CA-94CB-E111-A92F-00261834B51E.root',
#  '/store/2012C_SingleMu_RAW-RECO/24ADA49F-89F5-E111-AFA6-E0CB4E1A118A.root',
  ),
  skipEvents = cms.untracked.uint32(0)
)

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
# rpc emulator 
#
process.load("L1TriggerConfig.RPCTriggerConfig.L1RPCConfig_cff")
process.load("L1TriggerConfig.RPCTriggerConfig.RPCBxOrConfig_cff")
process.load("L1Trigger.RPCTrigger.RPCConeConfig_cff")
process.load("L1Trigger.RPCTrigger.l1RpcEmulDigis_cfi")
#process.rpcconf.filedir = cms.untracked.string('UserCode/L1RpcTriggerAnalysis/data/Paterny/Marcin_Wide/')
#process.rpcconf.PACsPerTower = cms.untracked.int32(1)
process.rpcconf.filedir = cms.untracked.string('UserCode/L1RpcTriggerAnalysis/data/Paterny/D_20110921_fixedCones_new36__all_12/')
process.rpcconf.PACsPerTower = cms.untracked.int32(12)
process.l1RPCBxOrConfig.lastBX = cms.int32(0)
process.l1RPCBxOrConfig.firstBX = cms.int32(0) 

#
# Gt,Gmt unpacker (not in reco)
#
process.load("EventFilter.L1GlobalTriggerRawToDigi.l1GtUnpack_cfi")
process.l1GtUnpack.DaqGtInputTag = cms.InputTag('rawDataCollector')

#
# re-emulate GMT with changed RPC from Emulator 
#
import L1Trigger.GlobalMuonTrigger.gmtDigis_cfi
process.l1GmtEmulDigis = L1Trigger.GlobalMuonTrigger.gmtDigis_cfi.gmtDigis.clone()
process.l1GmtEmulDigis.DTCandidates = cms.InputTag('l1GtUnpack','DT')
process.l1GmtEmulDigis.CSCCandidates = cms.InputTag('l1GtUnpack','CSC')
process.l1GmtEmulDigis.RPCbCandidates = cms.InputTag('l1RpcEmulDigis','RPCb')
process.l1GmtEmulDigis.RPCfCandidates = cms.InputTag('l1RpcEmulDigis','RPCf')
process.l1GmtEmulDigis.MipIsoData = 'gctDigis'

#
# refit Muon
#
process.load("TrackingTools.TrackRefitter.globalMuonTrajectories_cff")

#
# RPC Raw data monitoring
#
process.load("DQM.RPCMonitorClient.RPCMonitorRaw_cfi")
process.rpcMonitorRaw.writeHistograms = cms.untracked.bool(False)

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
process.load("L1Trigger.HardwareValidation.L1Comparator_cfi")
process.l1compare.RPCsourceData = cms.InputTag("l1GtUnpack") 
process.l1compare.RPCsourceEmul = cms.InputTag("l1RpcEmulDigis") 
process.l1compare.COMPARE_COLLS = cms.untracked.vuint32(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0)

#
# some filters
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
# Tree Maker
#
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
    maxEta = cms.double(2.4),
    maxChi2Mu = cms.double(2.),
    maxChi2Tk = cms.double(2.),
    minNumberTrackerHits = cms.int32(10),
    minNumberRpcHits = cms.int32(1),
    minNumberDtCscHits = cms.int32(1),
    minNumberOfMatchedStations = cms.int32(1),
    deltaPhiUnique = cms.double(1.0),
    deltaEtaUnique = cms.double(0.5)
  ),

  linkSynchroGrabber = cms.PSet(
    writeHistograms = cms.untracked.bool(True),
    deltaR_MuonToDetUnit_cutoff = cms.double(0.5), 
    checkInside = cms.bool(True),
    linkMonitorPSet = cms.PSet(
      useFirstHitOnly = cms.untracked.bool(True),
      dumpDelays = cms.untracked.bool(True) # set to True for LB delay plots
    ),
    synchroSelector = cms.PSet(
      checkRpcDetMatching_minPropagationQuality = cms.int32(0), 
      checkRpcDetMatching_matchingScaleValue = cms.double(3),
      checkRpcDetMatching_matchingScaleAuto  = cms.bool(True), 
      checkUniqueRecHitMatching_maxPull = cms.double(2.),
      checkUniqueRecHitMatching_maxDist = cms.double(5.)
    ) 
  ),
  
  l1ObjMaker = cms.PSet(
    l1RpcSource       = cms.InputTag("gtDigis"),
    l1CscSource       = cms.InputTag("gtDigis"),
    l1DtSource        = cms.InputTag("gtDigis"),
    l1GmtSource       = cms.InputTag("gtDigis"),
    l1RpcEmuSource    = cms.InputTag("l1RpcEmulDigis"),
    l1GmtEmuSource    = cms.InputTag("l1GmtEmulDigis") 
  ),
  matcherPSet =  cms.PSet( maxDeltaEta = cms.double(0.4), maxDeltaPhi = cms.double(0.3))
)
#print process.dumpPython();

process.p = cms.Path(
  process.filterGM*
  process.muonRPCDigis*         #needed by RPC synchronisation and l1RpcEmulDigis
  process.l1GtUnpack*           #needed by l1Compare and GmtEmulDigis
  process.rpcMonitorRaw*process.rpcFEDIntegrity*
  process.l1tRpctf*process.l1compare*
  process.l1demon*
  process.l1RpcEmulDigis* 
  process.l1GmtEmulDigis*
  process.globalMuons*process.l1RpcTree
)
