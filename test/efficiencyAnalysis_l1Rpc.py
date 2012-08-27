import os
import FWCore.ParameterSet.Config as cms

process = cms.Process("Analysis")

def dataChainFileNames():
  outDataChainFileNames = cms.PSet(
    treeFileNames = cms.vstring() )
  ##Add data
  path = "/afs/cern.ch/cms/L1/rpc/Shift/Crab/"
  datesToAdd = ["2012_08_06/express2012C/","2012_08_16/express2012C/","2012_08_20/express2012C/"]
  for date in datesToAdd:
    for fname in os.listdir(path+date):
        if fname=="ROOT":
                for fname1 in os.listdir(path+date+"/"+fname):
                    if fname1.find("efficiencyTree")>-1 and  fname1.find(".root")>-1:
                        outDataChainFileNames.treeFileNames.append(path+date+"/"+fname+"/"+fname1)
  print outDataChainFileNames.treeFileNames
  return outDataChainFileNames
##########

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(0))
process.source = cms.Source("EmptySource")

process.efficiency = cms.EDAnalyzer("L1RpcEfficiencyTreeAnalysis",
  dataChainFileNames(), # for CRAB
  #treeFileNames = cms.vstring(
  #  '/afs/cern.ch/work/c/cwiok/Shift/CMSSW_5_3_3/src/UserCode/L1RpcTriggerAnalysis/test/Crab/efficiencyTree.root'
  #), # for testing single files
  histoFileName = cms.string("efficiencyHistos.root"),
  l1Cut = cms.double(15.),
  ptMin = cms.double(8.)
)

process.p = cms.Path(process.efficiency)
