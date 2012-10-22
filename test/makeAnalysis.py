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
  l1Cut = cms.double(15.),
  ptMin = cms.double(8.)
)

process.p = cms.Path(process.l1RpcAnalysis)
