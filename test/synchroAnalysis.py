import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 
#'file:/disk00/work/data/run132440.muon/208A5482-4D3C-DF11-97EA-00E08178C0FB.root',
#'file:/disk00/work/data/run132440.muon/BC463F24-4A3C-DF11-998A-00E08178C085.root',
#'file:/disk00/work/data/run132440.muon/963F1CB1-443C-DF11-B5F1-003048673F1C.root',
#'file:/disk00/work/data/run132440.muon/86912F13-4C3C-DF11-9CEB-00E0817918BF.root'),
#'file:/disk00/work/data/run133321/E2758439-9749-DF11-B4E1-003048673F2C.root',
 'file:/disk00/work/data/run133321/EC5E5E70-9D49-DF11-A32C-00E081791809.root'),
#'file:/disk00/work/data/run132440.tmf/run132440_158_167.root',
#'file:/disk00/work/data/run132440.tmf/run132440_168_177.root',
#'file:/disk00/work/data/run132440.tmf/run132440_178_187.root',
#'file:/disk00/work/data/run132440.tmf/run132440_188_197.root',
#'file:/disk00/work/data/run132440.tmf/run132440_198_207.root',
#'file:/disk00/work/data/run132440.tmf/run132440_208_217.root',
#'file:/disk00/work/data/run132440.tmf/run132440_218_227.root',
#'file:/disk00/work/data/run132440.tmf/run132440_228_237.root',
#'file:/disk00/work/data/run132440.tmf/run132440_238_247.root',
#'file:/disk00/work/data/run132440.tmf/run132440_248_257.root',
#'file:/disk00/work/data/run132440.tmf/run132440_258_267.root',
#'file:/disk00/work/data/run132440.tmf/run132440_268_277.root',
#'file:/disk00/work/data/run132440.tmf/run132440_278_287.root',
#'file:/disk00/work/data/run132440.tmf/run132440_288_297.root',
#'file:/disk00/work/data/run132440.tmf/run132440_298_307.root',
#'file:/disk00/work/data/run132440.tmf/run132440_308_317.root',
#'file:/disk00/work/data/run132440.tmf/run132440_318_327.root',
#'file:/disk00/work/data/run132440.tmf/run132440_328_337.root',
#'file:/disk00/work/data/run132440.tmf/run132440_338_347.root',
#'file:/disk00/work/data/run132440.tmf/run132440_348_357.root'),
inputCommands=cms.untracked.vstring( 'keep *', 'drop *_hltL1GtObjectMap_*_*')
)

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
process.GlobalTag.globaltag = 'MC_36Y_V4::All'
#process.GlobalTag.globaltag = 'START36_V4::All'
#process.GlobalTag.globaltag = 'GR10_P_V4::All'

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
    #debugModules = cms.untracked.vstring('rpcunpacker','rpcMonitorLinkSynchro'),
    debugModules = cms.untracked.vstring(''),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)

process.load("DQM.L1TMonitor.L1TRPCTF_cfi")
process.l1trpctf.rpctfSource =  cms.InputTag("gtDigis")

process.load("DQM.RPCMonitorClient.RPCFEDIntegrity_cfi")
process.load("DQM.RPCMonitorClient.RPCMonitorRaw_cfi")
process.rpcMonitorRaw.writeHistograms = cms.untracked.bool(True)
process.rpcMonitorRaw.histoFileName = cms.untracked.string("histos1.root")


process.filterBX = cms.EDFilter("FilterBX", beamBX = cms.vuint32(1) )

process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
  vertexCollection = cms.InputTag('offlinePrimaryVertices'),
  minimumNDOF = cms.uint32(4) ,
  maxAbsZ = cms.double(15),	
  maxd0 = cms.double(2)	
)

process.filterL1_GM = cms.EDFilter("Filter_L1_GM", l1MuReadout = cms.InputTag("gtDigis"))

process.linkSynchroAnalysis =  cms.EDAnalyzer("LinkSynchroAnalysis",
  writeHistograms = cms.untracked.bool(True),
  histoFileName = cms.untracked.string("analysis.root"),
  linkMonitorPSet = cms.PSet(
    useFirstHitOnly = cms.untracked.bool(True),
    dumpDelays = cms.untracked.bool(True)
  ),
  ORedSynchroFilters = cms.VPSet(
    cms.PSet( collection = cms.string("globalMuons"),
      minPt = cms.double(2.),  
      beamSpot = cms.InputTag("offlineBeamSpot"),
      maxTIP = cms.double(0.5),
      matchL1RPC = cms.bool(False)
    ),
    cms.PSet( collection = cms.string("generalTracks"),
      minPt = cms.double(2.),    
      beamSpot = cms.InputTag("offlineBeamSpot"),
      maxTIP = cms.double(0.5),
      matchL1RPC = cms.bool(True),
      l1MuReadout = cms.InputTag("gtDigis"),    
      maxDeltaEta = cms.double(0.2),
      maxDeltaPhi = cms.double(0.25)
    )
  )
)

process.p = cms.Path( 
#  process.filterBX*
  process.primaryVertexFilter*
  process.gtDigis*
  process.filterL1_GM* 
  process.muonRPCDigis*
  process.rpcFEDIntegrity*process.rpcMonitorRaw*
  process.linkSynchroAnalysis*
  process.l1trpctf*
  process.dqmEnv*process.dqmSaver)
