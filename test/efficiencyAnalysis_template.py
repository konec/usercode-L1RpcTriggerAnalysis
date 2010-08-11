import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")
#
# EITHER:
#
# get list of input files from a file
# (255 file limit does not apply)
import FWCore.Utilities.FileUtils as FileUtils
mylist = FileUtils.loadListFromFile("_LIST_OF_FILES_")
#
# OR:
#
# include list of input efficiencyTree*.root files by hand
# (maximum 255 files!!!)
#dataChainFileNames = cms.PSet(
#  treeFileNames = cms.vstring(
#"data1/efficiencyTree_0_1.root",
#"data1/efficiencyTree_1_1.root",
#"data1/efficiencyTree_2_1.root",
#"data1/efficiencyTree_3_1.root",
#"data1/efficiencyTree_4_1.root",
#"data2/efficiencyTree_0_1.root",
#"data2/efficiencyTree_1_1.root",
#"data2/efficiencyTree_2_1.root",
#"data2/efficiencyTree_3_1.root"
# ) )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(0))
process.source = cms.Source("EmptySource")

process.efficiency = cms.EDAnalyzer("EfficiencyAnalysis",
  treeFileNames = cms.vstring( *mylist ),
# dataChainFileNames,
  histoFileName = cms.string("efficiencyHistos.root"),
# l1Cut = cms.double(0.),
  ptMin = cms.double(0.0)
)

process.p = cms.Path(process.efficiency)
