import FWCore.ParameterSet.Config as cms

process = cms.Process("writeVHD")

process.load('Configuration.Geometry.GeometryExtendedPostLS1Reco_cff')
process.load('Configuration.Geometry.GeometryExtendedPostLS1_cff')
process.load("Configuration.StandardSequences.MagneticField_38T_PostLS1_cff")

#process.load("L1TriggerConfig.RPCTriggerConfig.L1RPCConfig_cff")
#process.rpcconf.filedir = cms.untracked.string('L1TriggerConfig/RPCTriggerConfig/test/D_LS1_R1V006__all_12/')
#process.load("L1Trigger.RPCTrigger.RPCConeConfig_cff")
##process.load("L1TriggerConfig.RPCTriggerConfig.RPCConeDefinition_cff")

process.load("L1Trigger.RPCTrigger.RPCConeConfig_cff")
process.load("L1TriggerConfig.RPCTriggerConfig.RPCHsbConfig_cff")
process.l1RPCHsbConfig.hsb0Mask = cms.vint32(3, 3, 3, 3, 3, 3, 3, 3)
process.l1RPCHsbConfig.hsb1Mask = cms.vint32(3, 3, 3, 3, 3, 3, 3, 3)
process.load("L1TriggerConfig.RPCTriggerConfig.RPCBxOrConfig_cff")
process.l1RPCBxOrConfig.lastBX = cms.int32(0)
process.l1RPCBxOrConfig.firstBX = cms.int32(0)
process.load("L1TriggerConfig.RPCTriggerConfig.L1RPCConfig_cff")
process.rpcconf.filedir = cms.untracked.string('UserCode/L1RpcTriggerAnalysis/data/Paterny/PostLS1_v03/')
process.rpcconf.PACsPerTower = cms.untracked.int32(12)


process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff")
process.GlobalTag.globaltag  = '74X_dataRun2_Express_v0' #prompt express



process.source = cms.Source("EmptySource")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
)


process.write = cms.EDAnalyzer("WriteVHDL",
          minTower = cms.int32(-16),
          maxTower = cms.int32(16),
          minSector = cms.int32(0),
          maxSector = cms.int32(11),
          templateName = cms.string("pacTemplate.vhd"),
          outDir =  cms.string("cones4/")
)


process.p1 = cms.Path(process.write)
