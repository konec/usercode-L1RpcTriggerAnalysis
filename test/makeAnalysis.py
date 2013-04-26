import os
import FWCore.ParameterSet.Config as cms

process = cms.Process("Analysis")
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1))

process.load("EventFilter.RPCRawToDigi.RPCSQLiteCabling_cfi")
process.RPCCabling.connect = 'sqlite_file:RPCEMap3.db'


process.l1RpcAnalysis = cms.EDAnalyzer("L1RpcTreeAnalysis",
#  treeFileNames = cms.vstring("l1RpcTree_Ex_C.root", "l1RpcTree_Ex_D.root"),
#  treeFileNames = cms.vstring("l1RpcTree_Ex_D.root"),
#  treeFileNames = cms.vstring("l1RpcTree_SM_C.root"),
#  treeFileNames = cms.vstring("l1RpcTree.root"),
#  treeFileNames = cms.vstring("l1RpcTree_CO9.root"),
#  treeFileNames = cms.vstring("l1RpcTree_CO9.root"),
  treeFileNames = cms.vstring("l1RpcTree.root"),
  histoFileName = cms.string("l1RpcAnalysis.root"),
  filterByAnaMuonDistribution = cms.bool(False),
  filterByAnaMenu = cms.bool(True),
  filterByAnaEvent = cms.bool(True),
  anaMuonDistribution = cms.PSet (
    requireAnyMuon = cms.bool(False),
    requireUnique  = cms.bool(False),
    requireOnlyOne = cms.bool(False),
    requireGlobal  = cms.bool(False),
    requireInner   = cms.bool(False),
    requireOuter   = cms.bool(False),
    ptMin  = cms.double(0.),
    etaMax = cms.double(2.1),
    minNumberOfMatchedStations = cms.uint32(0),
    minNumberRpcHits = cms.uint32(0),
    minNumberDtCscHits = cms.uint32(0),
  ),
  anaMenu = cms.PSet(  #OR of confiditions for L1 and HLT
#     acceptL1_OtherThanMu             = cms.bool(False),
#     acceptL1_Mu                      = cms.bool(True),
#     acceptL1_Names                   = cms.vstring("L1_ZeroBias"),
#     acceptHLT_OtherThanMuPhysicsAlCa = cms.bool(False),
#     acceptHLT_L1                     = cms.bool(False),
#     acceptHLT_ZeroBias               = cms.bool(True), 
#     acceptHLT_Physics                = cms.bool(False),
#     acceptHLT_Mu                     = cms.bool(False),

      acceptL1_OtherThanMu             = cms.bool(False),
      acceptL1_Mu                      = cms.bool(True),
      acceptL1_Names                   = cms.vstring("L1_ZeroBias"),
      acceptHLT_OtherThanMuPhysicsAlCa = cms.bool(False),
      acceptHLT_L1                     = cms.bool(True),
      acceptHLT_ZeroBias               = cms.bool(False), 
      acceptHLT_Physics                = cms.bool(False),
      acceptHLT_Mu                     = cms.bool(False),

#     acceptL1_OtherThanMu             = cms.bool(True),
#     acceptL1_Mu                      = cms.bool(True),
#     acceptL1_Names                   = cms.vstring("L1_ZeroBias","L1_SingleMu16"),
#     acceptHLT_OtherThanMuPhysicsAlCa = cms.bool(True),
#     acceptHLT_L1                     = cms.bool(True),
#     acceptHLT_ZeroBias               = cms.bool(True), 
#     acceptHLT_Physics                = cms.bool(True),
#     acceptHLT_Mu                     = cms.bool(True),

#   acceptL1_OtherThanMu             = cms.bool(True),
#   acceptL1_Mu                      = cms.bool(False),
#   acceptL1_Names                   = cms.vstring("L1_ZeroBias"),
#   acceptHLT_OtherThanMuPhysicsAlCa = cms.bool(True),
#   acceptHLT_L1                     = cms.bool(False),
#   acceptHLT_ZeroBias               = cms.bool(True), 
#   acceptHLT_Physics                = cms.bool(False),
#   acceptHLT_Mu                     = cms.bool(False),
  ),
  anaTimingL1 = cms.PSet(
    l1ptCutForDR = cms.double(15.99),
    l1DRCutForpT = cms.double(0.3),
    l1DRCutForCorrTight = cms.double(0.3),
    l1ptCutForCorrTight = cms.double(15.99),
    muptCutForCorrTight = cms.double(0.) 
  ),
  anaEvent =  cms.PSet(
    validBXes = cms.string("validBXes.txt"),
    useValidBX_ONLY = cms.bool(True),
#    useValidBX_NoBxMinus2_ONLY = cms.bool(True),
    skipRuns = cms.vuint32(198346,198372,198485,198487,  #bad Trigger
                           208509, 207875, 207871, 207813, 207790, 207779, 207714, 205086, 204900, #non standard BX structure
                           203836, 203835, 203834, 203833, 203832, 203830, 203777, 203742, 203739,
                           203708, 203709, 201535, 200976, 200961, 200466, 199306, 199282, 199275, 
                           199276, 199103, 198903, 198902, 198901, 198899, 198898, 198603, 198590, 
                           198589, 198588, 198215, 198214, 198213, 198212, 198211, 198210, 198208, 
                           198207, 198202, 198116, 198063, 198050, 198049),
  ),
)

process.p = cms.Path(process.l1RpcAnalysis)
