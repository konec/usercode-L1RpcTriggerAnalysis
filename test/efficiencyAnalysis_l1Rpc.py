import os
import FWCore.ParameterSet.Config as cms

process = cms.Process("Analysis")
data1ChainFileNames = cms.PSet(
  treeFileNames = cms.vstring() )

##Add data
#path = "/afs/cern.ch/cms/L1/rpc/Shift/Crab/"
path = "/afs/cern.ch/work/k/kazana/work/rpc/Shifts/crab_job/"
#datesToAdd = ["2012_06_15/v2_konec/"]
#datesToAdd = ["2012_06_15/v2_konec/","2012_06_15/v1_konec/"]
datesToAdd = ["2012_06_15/all0_l1Rpc_ex_2012A/"]
for date in datesToAdd:
    for fname in os.listdir(path+date):
        if fname=="ROOT":
                for fname1 in os.listdir(path+date+"/"+fname):
                    if fname1.find("efficiencyTree")>-1 and  fname1.find(".root")>-1:
                        data1ChainFileNames.treeFileNames.append(path+date+"/"+fname+"/"+fname1)
print data1ChainFileNames.treeFileNames
##########

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(0))
process.source = cms.Source("EmptySource")


#process.efficiency = cms.EDAnalyzer("EfficiencyAnalysis",
process.efficiency = cms.EDAnalyzer("L1RpcEfficiencyTreeAnalysis",
  data1ChainFileNames,
  histoFileName = cms.string("efficiencyHistos.root"),
  l1Cut = cms.double(15.),
  ptMin = cms.double(8.)
)

process.p = cms.Path(process.efficiency)
