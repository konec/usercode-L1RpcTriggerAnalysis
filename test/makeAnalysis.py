import os
import FWCore.ParameterSet.Config as cms

process = cms.Process("Analysis")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1))

process.load("EventFilter.RPCRawToDigi.RPCSQLiteCabling_cfi")
process.RPCCabling.connect = 'sqlite_file:RPCEMap3.db'


process.l1RpcAnalysis = cms.EDAnalyzer("L1RpcTreeAnalysis",
  treeFileNames = cms.vstring('l1RpcTree.root'),
  histoFileName = cms.string("l1RpcAnalysis.root"),
  filterByAnaMuonDistribution = cms.bool(True),
  fillterByAnaMenu = cms.bool(True),
  anaMuonDistribution = cms.PSet (
    ptMin = cms.double(2.),
    minNumberOfMatchedStations = cms.uint32(1),
    minNumberRpcHits = cms.uint32(0),
    minNumberDtCscHits = cms.uint32(0),
  ),
  anaTimingL1 = cms.PSet(
    l1ptCutForDR = cms.double(0.),
    l1DRCutForpT = cms.double(0.1)
  ),
)

process.p = cms.Path(process.l1RpcAnalysis)
