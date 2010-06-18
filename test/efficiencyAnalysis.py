import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

data0ChainFileNames = cms.PSet( treeFileNames = cms.vstring( "efficiencyTree.root") )

data4ChainFileNames = cms.PSet(
  treeFileNames = cms.vstring(
"data4/efficiencyTree_10_1.root",
"data4/efficiencyTree_11_1.root",
"data4/efficiencyTree_12_1.root",
"data4/efficiencyTree_13_1.root",
"data4/efficiencyTree_14_1.root",
"data4/efficiencyTree_15_1.root",
"data4/efficiencyTree_16_1.root",
"data4/efficiencyTree_17_1.root",
"data4/efficiencyTree_18_1.root",
"data4/efficiencyTree_19_1.root",
"data4/efficiencyTree_1_1.root",
"data4/efficiencyTree_20_1.root",
"data4/efficiencyTree_21_1.root",
"data4/efficiencyTree_22_1.root",
"data4/efficiencyTree_23_1.root",
"data4/efficiencyTree_24_1.root",
"data4/efficiencyTree_25_1.root",
"data4/efficiencyTree_26_1.root",
"data4/efficiencyTree_27_1.root",
"data4/efficiencyTree_28_1.root"
 ) )

data1ChainFileNames = cms.PSet(
  treeFileNames = cms.vstring(
"data1/efficiencyTree_10_1.root",
"data1/efficiencyTree_11_1.root",
"data1/efficiencyTree_12_1.root",
"data1/efficiencyTree_13_1.root",
"data1/efficiencyTree_14_1.root",
"data1/efficiencyTree_15_1.root",
"data1/efficiencyTree_16_1.root",
"data1/efficiencyTree_17_1.root",
"data1/efficiencyTree_18_1.root",
"data1/efficiencyTree_19_1.root",
"data1/efficiencyTree_1_1.root",
"data1/efficiencyTree_20_1.root",
"data1/efficiencyTree_21_1.root",
"data1/efficiencyTree_22_1.root",
"data1/efficiencyTree_23_1.root",
"data1/efficiencyTree_24_1.root",
"data1/efficiencyTree_25_1.root",
"data1/efficiencyTree_26_1.root",
"data1/efficiencyTree_27_1.root",
"data1/efficiencyTree_28_1.root",
"data1/efficiencyTree_2_1.root",
"data1/efficiencyTree_3_1.root",
"data1/efficiencyTree_4_1.root",
"data1/efficiencyTree_5_1.root",
"data1/efficiencyTree_6_1.root",
"data1/efficiencyTree_7_1.root",
"data1/efficiencyTree_8_1.root",
"data1/efficiencyTree_9_1.root",
"data2/efficiencyTree_1_1.root",
"data2/efficiencyTree_2_1.root",
"data2/efficiencyTree_3_1.root",
"data2/efficiencyTree_4_1.root",
"data3/efficiencyTree_1_1.root"
 ) )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(0))
process.source = cms.Source("EmptySource")

process.efficiency = cms.EDAnalyzer("EfficiencyAnalysis",
  data1ChainFileNames,
  histoFileName = cms.string("efficiencyHistos.root"),
#  l1Cut = cms.double(0.),
  ptMin = cms.double(0.0)
)

process.p = cms.Path(process.efficiency)
