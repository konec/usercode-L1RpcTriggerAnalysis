import FWCore.ParameterSet.Config as cms
import os
import sys
import commands

process = cms.Process("Analysis")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1))


process.load('Configuration.StandardSequences.Services_cff')
#process.load('Configuration.Geometry.GeometryExtendedPostLS1Reco_cff')
#process.load('Configuration.Geometry.GeometryExtendedPostLS1_cff')
###PostLS1 geometry used
process.load('Configuration.Geometry.GeometryExtended2015_cff')
process.load('Configuration.Geometry.GeometryExtended2015Reco_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')


#
# connect custom cabling map
#
process.load("EventFilter.RPCRawToDigi.RPCSQLiteCabling_cfi")
process.RPCCabling.connect = 'sqlite_file:RPCEMap10.db'
process.RPCCabling.toGet[0].tag = 'RPCEMap_v3'
process.es_prefer_RPCEMap = cms.ESPrefer("PoolDBESSource","RPCCabling");


process.angle= cms.EDAnalyzer("OmtfAngleAnalyser")

process.p = cms.Path(process.angle)

