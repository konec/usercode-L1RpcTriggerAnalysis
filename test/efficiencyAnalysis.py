import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(0))
process.source = cms.Source("EmptySource")

process.efficiency = cms.EDAnalyzer("EfficiencyAnalysis",
  treeFileName =  cms.string("efficiencyTree.root"),
  histoFileName = cms.string("efficiencyHistos.root")
)

process.p = cms.Path(process.efficiency)
