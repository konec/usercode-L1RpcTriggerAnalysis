import os
import FWCore.ParameterSet.Config as cms

process = cms.Process("Analysis")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1))

process.load("EventFilter.RPCRawToDigi.RPCSQLiteCabling_cfi")
process.RPCCabling.connect = 'sqlite_file:RPCEMap3.db'


process.l1RpcAnalysis = cms.EDAnalyzer("L1RpcTreeAnalysis",
  treeFileNames = cms.vstring("l1RpcTree_EX_C.root", "l1RpcTree_EX_D.root"),
  histoFileName = cms.string("l1RpcAnalysis.root"),
  filterByAnaMuonDistribution = cms.bool(True),
  fillterByAnaMenu = cms.bool(True),
  skipRuns = cms.vuint32(198346,198372,198485,198487),
  anaMuonDistribution = cms.PSet (
    ptMin  = cms.double(0.),
    etaMax = cms.double(1.6),
    minNumberOfMatchedStations = cms.uint32(2),
    minNumberRpcHits = cms.uint32(0),
    minNumberDtCscHits = cms.uint32(1),
  ),
  anaMenu = cms.PSet(  #OR of confiditions for L1 and HLT
    acceptL1_OtherThanMu            = cms.bool(True),
    acceptL1_Mu                     = cms.bool(True),
#    acceptL1_Names                  = cms.vstring("L1_SingleMu16"),
#   acceptL1_Names                  = cms.vstring("L1_ZeroBias","L1_SingleMu16"),
    acceptHLT_OtherThanMuAndPhysics = cms.bool(True),
    acceptHLT_Physics               = cms.bool(True),
    acceptHLT_Mu                    = cms.bool(True),
    acceptHLT_ZeroBias              = cms.bool(True) 
  ),
  anaTimingL1 = cms.PSet(
    l1ptCutForDR = cms.double(16.),
    l1DRCutForpT = cms.double(0.1),
    l1DRCutForCorrTight = cms.double(0.1),
    l1ptCutForCorrTight = cms.double(16.),
    muptCutForCorrTight = cms.double(16.) 
  ),
)

process.p = cms.Path(process.l1RpcAnalysis)
