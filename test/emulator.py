import FWCore.ParameterSet.Config as cms

process = cms.Process("rpctest")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

# get list of input files from a file
#(255 file limit does not apply)
import FWCore.Utilities.FileUtils as FileUtils
mylist = FileUtils.loadListFromFile(_LIST_OF_FILES_) 
process.source = cms.Source("PoolSource", 
fileNames = cms.untracked.vstring( *mylist )
#inputCommands=cms.untracked.vstring( 'keep *', 'drop *_hltL1GtObjectMap_*_*')
)

# apply lumi section mask from JSON file
import PhysicsTools.PythonAnalysis.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes
myLumis = LumiList.LumiList(filename=_JSON_FILE_).getCMSSWString().split(',')
process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
process.source.lumisToProcess.extend(myLumis)

#process.source = cms.Source("PoolSource",
#    fileNames = cms.untracked.vstring(
#'/store/express/Run2011A/ExpressPhysics/FEVT/Express-v1/000/160/940/BC9DC877-1253-E011-93B3-001617E30CC2.root'
#))

process.load("L1TriggerConfig.L1GtConfigProducers.L1GtConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerScalesConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuTriggerPtScaleConfig_cff")
process.load("L1TriggerConfig.L1ScalesProducers.L1MuGMTScalesConfig_cff")
process.load("L1TriggerConfig.GMTConfigProducers.L1MuGMTParametersConfig_cff")
process.load("L1TriggerConfig.L1GeometryProducers.l1CaloGeomConfig_cff")
process.load("EventFilter.L1GlobalTriggerRawToDigi.l1GtUnpack_cfi")
process.l1GtUnpack.DaqGtInputTag = cms.InputTag("source")
process.l1GtUnpack.UnpackBxInEvent = cms.int32(-1)

#process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger = cms.Service("MessageLogger",
debugModules = cms.untracked.vstring(' '),
destinations = cms.untracked.vstring('cout'),
cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)

# rpc geometry
process.load("Geometry.MuonNumbering.muonNumberingInitialization_cfi")
process.load("Geometry.MuonCommonData.muonIdealGeometryXML_cfi")
process.load("Geometry.RPCGeometry.rpcGeometry_cfi")

# emulation
process.load("L1TriggerConfig.RPCTriggerConfig.L1RPCConfig_cff")

# Choose proper patterns here!
process.rpcconf.filedir = cms.untracked.string('Patterns_4of6/')
process.rpcconf.PACsPerTower = cms.untracked.int32(12)

process.load("L1TriggerConfig.RPCTriggerConfig.RPCConeDefinition_cff")
process.load("L1TriggerConfig.RPCTriggerConfig.RPCHwConfig_cff")
process.load("L1TriggerConfig.RPCTriggerConfig.RPCBxOrConfig_cff")
process.load("L1TriggerConfig.RPCTriggerConfig.RPCHsbConfig_cff")
                                       #TC  11  0  1  2  3  4  5  6
process.l1RPCHsbConfig.hsb0Mask = cms.vint32(3, 3, 3, 3, 3, 3, 3, 3)
                                       #TC   5  6  7  8  9 10 11  0
process.l1RPCHsbConfig.hsb1Mask = cms.vint32(3, 3, 3, 3, 3, 3, 3, 3)

process.load("L1Trigger.RPCTrigger.l1RpcEmulDigis_cfi")
process.load("EventFilter.RPCRawToDigi.RPCSQLiteCabling_cfi")
process.load("EventFilter.RPCRawToDigi.rpcUnpacker_cfi")
process.l1RpcEmulDigis.RPCTriggerDebug = 1 
process.l1RpcEmulDigis.label = cms.string('rpcunpacker')

#emulator/comparator
process.load("L1Trigger.HardwareValidation.L1HardwareValidation_cff")
process.load("DQM.L1TMonitor.L1TDEMON_cfi")

process.load("L1Trigger.HardwareValidation.L1DummyProducer_cfi")
process.l1dummy.VerboseFlag = 0
process.l1dummy.DO_SYSTEM = [0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0]
process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
    moduleSeeds = cms.PSet(
        l1dummy = cms.untracked.uint32(1762349)
    )
)
process.load("L1Trigger.GlobalMuonTrigger.gmtDigis_cff")
process.gmtDigis.DTCandidates = 'l1GtUnpack:DT'
process.gmtDigis.CSCCandidates = 'l1GtUnpack:CSC'
process.gmtDigis.RPCbCandidates = 'l1RpcEmulDigis:RPCb'
process.gmtDigis.RPCfCandidates = 'l1RpcEmulDigis:RPCf'
process.gmtDigis.MipIsoData = 'l1dummy'


process.l1compare.RPCsourceData = cms.InputTag("l1GtUnpack")
process.l1compare.RPCsourceEmul = cms.InputTag("l1RpcEmulDigis")
process.l1compare.COMPARE_COLLS = cms.untracked.vuint32(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0)
process.l1compare.VerboseFlag   = 0
process.l1compare.DumpMode      = -1


process.load("DQMServices.Core.DQM_cfg")
process.load("DQMServices.Components.DQMEnvironment_cfi")

process.DQMStore = cms.Service("DQMStore")

process.l1demon.disableROOToutput = cms.untracked.bool(False)

process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
  vertexCollection = cms.InputTag('offlinePrimaryVertices'),
  minimumNDOF = cms.uint32(4) ,
  maxAbsZ = cms.double(15),	
  maxd0 = cms.double(2)	
)

process.filterL1_GM = cms.EDFilter("Filter_L1_GM", l1MuReadout = cms.InputTag("gmtDigis"))

process.p = cms.Path(
   process.primaryVertexFilter* 
   process.rpcunpacker*
   process.l1dummy*
   process.l1RpcEmulDigis*
   process.l1GtUnpack*
   process.gmtDigis*
   process.filterL1_GM*
   process.l1compare*
   process.l1demon)

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('emu.root'),
        outputCommands = cms.untracked.vstring(
           'drop *',
           'keep *_offlinePrimaryVertices_*_*',
           'keep *_offlineBeamSpot_*_*',
           'keep *_muons_*_*',
           'keep *_globalMuons_*_*',
           'keep *_tevMuons_*_*',
           'keep *_generalTracks_*_*',
           'keep *_si*Clusters_*_*',
           'keep *_pixelTracks_*_*',
           'keep *_standAloneMuons_*_*',
           'keep *_rpcRecHits_*_*',
           'keep *_gmtDigis_*_*'),
    SelectEvents = cms.untracked.PSet( SelectEvents = cms.vstring('p') )
)
process.e = cms.EndPath(process.out)
