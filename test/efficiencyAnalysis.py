import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

data0ChainFileNames = cms.PSet( treeFileNames = cms.vstring( "efficiencyTree.root") )

data2ChainFileNames = cms.PSet(
  treeFileNames = cms.vstring(
"data2/efficiencyTree_10_1.root",
"data2/efficiencyTree_11_1.root",
"data2/efficiencyTree_12_1.root",
"data2/efficiencyTree_13_1.root",
"data2/efficiencyTree_14_1.root",
"data2/efficiencyTree_15_1.root",
"data2/efficiencyTree_16_1.root",
"data2/efficiencyTree_17_1.root",
"data2/efficiencyTree_18_1.root",
"data2/efficiencyTree_19_1.root",
"data2/efficiencyTree_1_1.root",
"data2/efficiencyTree_20_1.root",
"data2/efficiencyTree_21_1.root",
"data2/efficiencyTree_22_1.root",
"data2/efficiencyTree_23_1.root",
"data2/efficiencyTree_24_1.root",
"data2/efficiencyTree_25_1.root",
"data2/efficiencyTree_26_1.root",
"data2/efficiencyTree_27_1.root",
"data2/efficiencyTree_28_1.root",
"data2/efficiencyTree_29_1.root",
"data2/efficiencyTree_2_1.root",
"data2/efficiencyTree_30_1.root",
"data2/efficiencyTree_31_1.root",
"data2/efficiencyTree_32_1.root",
"data2/efficiencyTree_33_1.root",
"data2/efficiencyTree_34_1.root",
"data2/efficiencyTree_35_1.root",
"data2/efficiencyTree_36_1.root",
"data2/efficiencyTree_37_1.root",
"data2/efficiencyTree_38_1.root",
"data2/efficiencyTree_39_1.root",
"data2/efficiencyTree_3_1.root",
"data2/efficiencyTree_40_1.root",
"data2/efficiencyTree_41_1.root",
"data2/efficiencyTree_42_1.root",
"data2/efficiencyTree_43_1.root",
"data2/efficiencyTree_44_1.root",
"data2/efficiencyTree_45_1.root",
"data2/efficiencyTree_46_1.root",
"data2/efficiencyTree_47_1.root",
"data2/efficiencyTree_48_1.root",
"data2/efficiencyTree_49_1.root",
"data2/efficiencyTree_4_1.root",
"data2/efficiencyTree_50_1.root",
"data2/efficiencyTree_52_1.root",
"data2/efficiencyTree_53_1.root",
"data2/efficiencyTree_54_1.root",
"data2/efficiencyTree_5_1.root",
"data2/efficiencyTree_6_1.root",
"data2/efficiencyTree_7_1.root",
"data2/efficiencyTree_8_1.root",
"data2/efficiencyTree_9_1.root"
 ) )

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
"data4/efficiencyTree_28_1.root",
"data4/efficiencyTree_2_1.root",
"data4/efficiencyTree_3_1.root",
"data4/efficiencyTree_4_1.root",
"data4/efficiencyTree_5_1.root",
"data4/efficiencyTree_6_1.root",
"data4/efficiencyTree_7_1.root",
"data4/efficiencyTree_8_1.root",
"data4/efficiencyTree_9_1.root"
 ) )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(0))
process.source = cms.Source("EmptySource")

process.efficiency = cms.EDAnalyzer("EfficiencyAnalysis",
  data4ChainFileNames,
  histoFileName = cms.string("efficiencyHistos.root"),
#  l1Cut = cms.double(0.),
  ptMin = cms.double(0.0)
)

process.p = cms.Path(process.efficiency)
