import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")


process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource", fileNames =  cms.untracked.vstring( 
'file:/disk00/work/data/MuonDPG_skim-May27thSkim_v3/863EAB8A-8D6E-DF11-9EFA-00304867402A.root'),
#'file:/disk00/work/data/run132440.muon/208A5482-4D3C-DF11-97EA-00E08178C0FB.root',
#'file:/disk00/work/data/run132440.muon/BC463F24-4A3C-DF11-998A-00E08178C085.root',
#'file:/disk00/work/data/run132440.muon/963F1CB1-443C-DF11-B5F1-003048673F1C.root',
#'file:/disk00/work/data/run132440.muon/86912F13-4C3C-DF11-9CEB-00E0817918BF.root'),
#'file:/disk00/work/data/run133321/E2758439-9749-DF11-B4E1-003048673F2C.root',
#'file:/disk00/work/data/run133321/EC5E5E70-9D49-DF11-A32C-00E081791809.root'),
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
process.GlobalTag.globaltag = 'GR_R_37X_V5::All'
#process.GlobalTag.globaltag = 'MC_37Y_V5::All'
#process.GlobalTag.globaltag = 'START37_V5::All'
#process.GlobalTag.globaltag = 'DESIGN_37_V5::All'
#process.GlobalTag.globaltag = 'GR10_P_V5::All'

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
    dumpDelays = cms.untracked.bool(False)
  ),
  ORedSynchroFilters = cms.VPSet(
    cms.PSet( collection = cms.string("globalMuons"),
      minPt = cms.double(2.),  
      beamSpot = cms.InputTag("offlineBeamSpot"),
      maxTIP = cms.double(0.5),
      matchL1RPC = cms.bool(False),
      checkUniqueRecHitMatching = cms.bool(True)
    ) ,
    cms.PSet( collection = cms.string("generalTracks"),
      minPt = cms.double(2.),    
      beamSpot = cms.InputTag("offlineBeamSpot"),
      maxTIP = cms.double(0.5),
      matchL1RPC = cms.bool(True),
      checkUniqueRecHitMatching = cms.bool(True),
      l1MuReadout = cms.InputTag("gtDigis"),    
      maxDeltaEta = cms.double(0.2),
      maxDeltaPhi = cms.double(0.25)
    )
  )
)
process.efficiencyTree = cms.EDAnalyzer("EfficiencyTree",
    treeFileName = cms.string("efficiencyTree.root"),
    muonColl = cms.string("muons"),
    trackColl = cms.string("generalTracks"),
    beamSpot = cms.InputTag("offlineBeamSpot"),
    minPt = cms.double(2.),
    maxTIP = cms.double(0.5),
    maxEta = cms.double(1.6),
    l1MuReadout = cms.InputTag("gtDigis"),
    rpcMatcherPSet =  cms.PSet( maxDeltaEta = cms.double(0.8), maxDeltaPhi = cms.double(0.8)), 
    dtcscMatcherPSet = cms.PSet( maxDeltaEta = cms.double(0.1), maxDeltaPhi = cms.double(0.1)), 
)

process.p = cms.Path( 
#  process.filterBX*
  process.primaryVertexFilter*
  process.gtDigis*
  process.filterL1_GM* 
  process.muonRPCDigis*
  process.rpcFEDIntegrity*process.rpcMonitorRaw*
  process.linkSynchroAnalysis*
  process.efficiencyTree)
