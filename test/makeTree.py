import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")
import os

#test

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(100) )
#
# For CRAB jobs leave fileNames vector empty.
# For processing single files insert lines with 'file:/PATH/FILE.root'
# (there is 255 file limit though).
process.source = cms.Source("PoolSource", 
fileNames = cms.untracked.vstring('file:Run247068.root'),
#fileNames = cms.untracked.vstring('file:NoRE4_1.root','file:NoRE4_2.root'),
#fileNames = cms.untracked.vstring('/store/express/Run2015B/ExpressPhysics/FEVT/Express-v1/000/251/562/00000/00F6C382-6328-E511-AEE9-02163E0143F8.root'),
#fileNames = cms.untracked.vstring('file:Run250893_Run2015A-ExpressPhysics-FEVT.root'),
#fileNames = cms.untracked.vstring('/store/express/Run2015B/ExpressPhysics/FEVT/Express-v1/000/251/027/00000/50585D31-6324-E511-9F70-02163E011878.root'),
#fileNames = cms.untracked.vstring('/store/express/Run2015A/ExpressPhysics/FEVT/Express-v1/000/250/893/00000/061A49E1-0023-E511-8310-02163E013955.root'),
#fileNames = cms.untracked.vstring('/store/express/Run2015A/ExpressPhysics/FEVT/Express-v1/000/246/951/00000/048B46D7-4D0A-E511-8FA4-02163E01478F.root'),
#fileNames = cms.untracked.vstring('root://xrootd.unl.edu//store/express/Run2015A/ExpressPhysics/FEVT/Express-v1/000/250/893/00000/061A49E1-0023-E511-8310-02163E013955.root'),
# fileNames =  cms.untracked.vstring( 
#  '/store/2012D_Commissioning_RECO/70747E0E-2634-E211-9733-5404A63886C7.root',
#'/store/data/Commissioning2015/Cosmics/RAW/v1/000/238/721/00000/FED46613-F6D0-E411-94D8-02163E01348C.root'
# '/store/express/Commissioning2015/ExpressCosmics/FEVT/Express-v1/000/238/721/00000/005ABD91-D6D0-E411-A46C-02163E01241E.root'
#  'file:test8.root'
#  ),
  skipEvents = cms.untracked.uint32(0)
)

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
#process.load('Configuration.StandardSequences.GeometryRecoDB_cff')
process.load("Configuration.Geometry.GeometryExtended2015Reco_cff")
process.load('Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff')
process.load('Configuration.StandardSequences.RawToDigi_Data_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_Data_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
#process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')

#
# set proper GlobalTag
#
#process.GlobalTag.globaltag = 'FT_R_53_V10::All' #rereco 2012ABC
#process.GlobalTag.globaltag = 'FT_R_53_V18::All' #rereco 2012ABC
#process.GlobalTag.globaltag = 'FT_R_53_V21::All' #rereco 2012D
#process.GlobalTag.globaltag  = 'GR_E_V46::All' #rereco
#process.GlobalTag.globaltag  = 'GR_P_V50::All' #rereco
#process.GlobalTag.globaltag  = 'GR_P_V54::All' #rereco
#process.GlobalTag.globaltag  = 'GR_P_V56::All' #rereco
#process.GlobalTag.globaltag  = '74X_dataRun2_Prompt_v0'
process.GlobalTag.globaltag  = '74X_dataRun2_Express_v0'

#
# message logger
#
process.load('FWCore.MessageService.MessageLogger_cfi')
process.MessageLogger = cms.Service("MessageLogger",
  debugModules = cms.untracked.vstring(),
  destinations = cms.untracked.vstring('cout'),
  cout = cms.untracked.PSet( 
    threshold = cms.untracked.string('DEBUG'),
    default = cms.untracked.PSet ( reportEvery = cms.untracked.int32(1))
  )
)
#process.MessageLogger.debugModules.append('muonRPCDigis')
process.MessageLogger.debugModules.append('rpcunpacker')
#process.MessageLogger.debugModules.append('l1RpcEmulDigis')
#process.MessageLogger.debugModules.append('rpcMonitorLinkSynchro')
#process.MessageLogger.debugModules.append('linkSynchroAnalysis')
#process.MessageLogger.debugModules.append('l1GtTrigReport')

process.options = cms.untracked.PSet( wantSummary=cms.untracked.bool(False))

#
# privare rpc r2d with custom cabling
#
#process.load("EventFilter.RPCRawToDigi.RPCSQLiteCabling_cfi")
#process.RPCCabling.connect = 'sqlite_file:RPCEMap10.db'
#process.es_prefer_RPCEMap = cms.ESPrefer("PoolDBESSource","RPCCabling");
process.rpcunpacker = cms.EDProducer("RPCUnpackingModule",
  InputLabel = cms.InputTag("rawDataCollector","","LHC"),
  doSynchro = cms.bool(True)
)

#
# Gt,Gmt unpacker NEEDED FOR l1GmtEmulDigis and L1TEMU (does not want gtDigis)!
#
process.load("EventFilter.L1GlobalTriggerRawToDigi.l1GtUnpack_cfi")
process.l1GtUnpack.DaqGtInputTag = cms.InputTag('rawDataCollector')

#
# re-emulate GMT with changed RPC from Emulator
#
import L1Trigger.GlobalMuonTrigger.gmtDigis_cfi
process.l1GmtEmulDigis = L1Trigger.GlobalMuonTrigger.gmtDigis_cfi.gmtDigis.clone()
process.l1GmtEmulDigis.RPCbCandidates = cms.InputTag('l1RpcEmulDigis','RPCb')
process.l1GmtEmulDigis.RPCfCandidates = cms.InputTag('l1RpcEmulDigis','RPCf')
process.l1GmtEmulDigis.DTCandidates = cms.InputTag('l1GtUnpack','DT')
process.l1GmtEmulDigis.CSCCandidates = cms.InputTag('l1GtUnpack','CSC')
process.l1GmtEmulDigis.MipIsoData = 'gctDigis'

#
# PACT emulator 
#
process.load("L1Trigger.RPCTrigger.l1RpcEmulDigis_cfi")
process.l1RpcEmulDigis.label =  cms.string('rpcunpacker')
process.l1RpcEmulDigis.RPCTriggerDebug = cms.untracked.int32(1)
process.load("L1Trigger.RPCTrigger.RPCConeConfig_cff")
#GB/sort
process.load("L1TriggerConfig.RPCTriggerConfig.RPCHsbConfig_cff")
process.l1RPCHsbConfig.hsb0Mask = cms.vint32(3, 3, 3, 3, 3, 3, 3, 3)
process.l1RPCHsbConfig.hsb1Mask = cms.vint32(3, 3, 3, 3, 3, 3, 3, 3)
#change bx
process.load("L1TriggerConfig.RPCTriggerConfig.RPCBxOrConfig_cff")
process.l1RPCBxOrConfig.lastBX = cms.int32(0)
process.l1RPCBxOrConfig.firstBX = cms.int32(0)
#alternative patterns
process.load("L1TriggerConfig.RPCTriggerConfig.L1RPCConfig_cff")
#process.rpcconf.filedir = cms.untracked.string('UserCode/L1RpcTriggerAnalysis/data/Paterny/D_20110921_fixedCones_new36__all_12/')
#process.rpcconf.filedir = cms.untracked.string('UserCode/L1RpcTriggerAnalysis/data/Paterny/PostLS1_v03/')
process.rpcconf.filedir = cms.untracked.string('UserCode/L1RpcTriggerAnalysis/data/Paterny/PostLS1_v01/')
process.rpcconf.PACsPerTower = cms.untracked.int32(12)
#process.rpcconf.filedir = cms.untracked.string('UserCode/L1RpcTriggerAnalysis/data/Paterny/Marcin_Wide/')
#process.rpcconf.PACsPerTower = cms.untracked.int32(1)


#
# needed by DQM
#
process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Components.DQMEnvironment_cfi")
process.load("DQM.Integration.test.environment_cfi")
process.dqmSaver.producer = 'DQM'
#process.dqmSaver.dirName = '/tmp/konec/'
process.dqmEnv.subSystemFolder = 'MK'
process.dqmSaver.saveAtJobEnd = cms.bool(True) 
#
# 
#
#
# L1RPCT DQM (L1T)
#
process.load("DQM.L1TMonitor.L1TRPCTF_cfi")
process.l1tRpctf.rpctfSource =  cms.InputTag("gtDigis")

#
# DQM DATA-EMU comparsion (L1TEMU)
#
process.load("DQM.L1TMonitor.L1TDEMON_cfi")
process.load("L1Trigger.HardwareValidation.L1Comparator_cfi")
process.l1compare.RPCsourceData = cms.InputTag("l1GtUnpack")
process.l1compare.RPCsourceEmul = cms.InputTag("l1RpcEmulDigis")
process.l1compare.COMPARE_COLLS = cms.untracked.vuint32(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0)
#process.l1compare.VerboseFlag = cms.untracked.int32(1)


#
# DQM of RPCFedIntegrity
#
process.load("DQM.RPCMonitorClient.RPCFEDIntegrity_cfi")
process.rpcFEDIntegrity.RPCRawCountsInputTag = cms.untracked.InputTag('rpcunpacker')

#
# RPC Raw data monitoring 
#
process.load("DQM.RPCMonitorClient.RPCMonitorRaw_cfi")
process.rpcMonitorRaw.writeHistograms = cms.untracked.bool(True)
process.rpcMonitorRaw.rpcRawDataCountsTag = cms.InputTag('rpcunpacker')


#
# refit Muon
#
#process.load("TrackingTools.TrackRefitter.globalMuonTrajectories_cff")
import TrackingTools.TrackRefitter.globalMuonTrajectories_cff
process.refittedMuons = TrackingTools.TrackRefitter.globalMuonTrajectories_cff.globalMuons.clone()

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
process.filterGM = cms.EDFilter("FilterGM", muons =  cms.InputTag("globalMuons") )
process.filterL1 = cms.EDFilter("FilterL1", 
  l1MuReadout = cms.InputTag("gtDigis"), 
  ptCut=cms.double(3.99)
)

process.l1RpcTree = cms.EDAnalyzer("L1RpcTreeMaker",

  histoFileName = cms.string("l1RpcHelper.root"),
  treeFileName = cms.string("l1RpcTree.root"),
  detHitCollector = cms.PSet(),
  checkDestSIMU = cms.bool(False),
  onlyBestMuEvents = cms.bool(True),

  bestMuonFinder = cms.PSet(
    muonColl = cms.string("muons"),
    beamSpot = cms.InputTag("offlineBeamSpot"),
    requireInnerTrack = cms.bool(True),
    requireOuterTrack = cms.bool(True),
    requireGlobalTrack = cms.bool(True),
    minPt = cms.double(2.),
    maxTIP = cms.double(0.2),
    maxAbsEta = cms.double(1.6),
    maxChi2Mu = cms.double(2.),
    maxChi2Sa = cms.double(2.),
    maxChi2Tk = cms.double(2.),
    minNumberTrackerHits = cms.int32(0),
    minNumberRpcHits = cms.int32(0),
    minNumberDtCscHits = cms.int32(0),
    minNumberOfMatchedStations = cms.int32(0),
    deltaPhiUnique = cms.double(1.0),
    deltaEtaUnique = cms.double(0.5)
  ),

  detHitDigiGrabber = cms.PSet (),

  linkSynchroGrabber = cms.PSet(
    rawSynchroTag = cms.InputTag("rpcunpacker"),
    writeHistograms = cms.untracked.bool(True),
    deltaR_MuonToDetUnit_cutoff = cms.double(0.3),
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
    l1RpcEmuSource    = cms.InputTag("l1RpcEmulDigis"),
    l1GmtEmuSource    = cms.InputTag("l1GmtEmulDigis"),
    l1RpcSource       = cms.InputTag("gtDigis"),
    l1CscSource       = cms.InputTag("gtDigis"),
    l1DtSource        = cms.InputTag("gtDigis"),
    l1GmtSource       = cms.InputTag("gtDigis") 
  ),
  matcherPSet =  cms.PSet( maxDeltaEta = cms.double(0.4), maxDeltaPhi = cms.double(0.3))
)


#print process.dumpPython();

process.p = cms.Path(
  process.filterL1*
  process.filterGM*
  process.rpcunpacker* 
  process.l1GtUnpack*     
  process.l1RpcEmulDigis* 
  process.l1GmtEmulDigis*
  process.refittedMuons*    
  process.l1RpcTree* 
  process.rpcMonitorRaw* 
  process.rpcFEDIntegrity* 
  process.l1tRpctf* 
  process.l1compare* 
  process.l1demon*    #at the End of Path  
  process.dqmEnv*     #at the End of Path
  process.dqmSaver    #at the End of Path (!!!) 
)
