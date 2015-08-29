import os
import FWCore.ParameterSet.Config as cms

process = cms.Process("Analysis")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1))



process.load('Configuration.StandardSequences.Services_cff')
process.load("EventFilter.RPCRawToDigi.RPCSQLiteCabling_cfi")
process.RPCCabling.connect = 'sqlite_file:RPCEMap10.db'



process.l1RpcAnalysis = cms.EDAnalyzer("L1RpcTreeAnalysis",
  treeFileNames = cms.vstring("l1RpcTree.root"),
  histoFileName = cms.string("l1RpcAnalysis.root"),
  anaSynch = cms.PSet(),
)

process.p = cms.Path(process.l1RpcAnalysis)
