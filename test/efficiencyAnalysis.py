import os
import FWCore.ParameterSet.Config as cms

process = cms.Process("Analysis")
data1ChainFileNames = cms.PSet(
  treeFileNames = cms.vstring() )

##Add data
path = "/afs/cern.ch/cms/L1/rpc/Shift/Crab/"
datesToAdd = ["09_06_2011/v1/","10_06_2011/v1/","19_06_2011/v1/","22_06_2011/v1","27_06_2011/v1","05_07_2011/v1"] #Will used date from those directories
for date in datesToAdd:
    for fname in os.listdir(path+date):
        if fname=="ROOT":
                for fname1 in os.listdir(path+date+"/"+fname):
                    if fname1.find(".root")>-1:
                        data1ChainFileNames.treeFileNames.append(path+date+"/"+fname+"/"+fname1)
print data1ChainFileNames.treeFileNames
##########

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(0))
process.source = cms.Source("EmptySource")

process.efficiency = cms.EDAnalyzer("EfficiencyAnalysis",
  data1ChainFileNames,
  histoFileName = cms.string("efficiencyHistos.root"),
  l1Cut = cms.double(15.),
  ptMin = cms.double(15.)
)

process.p = cms.Path(process.efficiency)
