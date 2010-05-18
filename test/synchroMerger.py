import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1))
process.source = cms.Source("EmptySource")

process.load("DQMServices.Core.DQM_cfg")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'MC_36Y_V4::All'

#process.GlobalTag.globaltag = 'START36_V3::All'

process.merger =  cms.EDAnalyzer("LinkSynchroMerger",
  writeHistograms = cms.untracked.bool(True),
  histoFileName = cms.untracked.string("merge.root"),
  linkMonitorPSet = cms.PSet(
    useFirstHitOnly = cms.untracked.bool(True),
    dumpDelays = cms.untracked.bool(True)
  ),
  preFillLinkSynchroFileNames=cms.untracked.vstring(
 '/disk00/work/CMSSW_3_6_0.R2D/jobs/crab_0_100505_190111/res/out.txt'
  )
)


process.MessageLogger = cms.Service("MessageLogger",
    debugModules = cms.untracked.vstring('merger'),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)


process.p = cms.Path(process.merger)
