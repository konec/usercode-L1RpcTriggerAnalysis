import os
import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1))
process.source = cms.Source("EmptySource")

process.load("DQMServices.Core.DQM_cfg")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
## Define GlobalTag
#process.GlobalTag.globaltag = 'GR_R_52_V9D::All' # for 52X data reprocessing
process.GlobalTag.globaltag = 'GR_R_53_V8::All' # for 53X<533 data reprocessing

def dataChainFileNames():
  outDataChainFileNames = cms.untracked.vstring()
  ## Add data
  path = "/afs/cern.ch/cms/L1/rpc/Shift/Crab/"
  pathTmp = "/tmp/"+os.getenv("USER")+"/__OUT_TXT__/"

  datesToAdd = ["2012_08_06/express2012C/","2012_08_16/express2012C/","2012_08_20/express2012C/"]
  pathTmpCreated = 0
  for date in datesToAdd:
    for fname in os.listdir(path+date):
        if fname=="ROOT":
                for fname1 in os.listdir(path+date+"/"+fname):
                    if fname1.find("out-")>-1 and  fname1.find(".txt.gz")>-1:
                        if pathTmpCreated==0 and os.system("mkdir -p "+pathTmp)!=0:
                            print "ERROR: Can't create temporary directory",pathTmp
                            os.system("rm -f "+pathTmp+"/out*.txt");
                            pathTmpCreated=1
                            exit()
                        pathTmpCreated = 1
                        os.system("\cp -f "+path+"/"+date+"/"+fname+"/"+fname1+" "+pathTmp)
                        os.system("gzip -d "+pathTmp+"/"+fname1)
                        outDataChainFileNames.append(pathTmp+"/"+fname1[:-3])
  if pathTmpCreated==1:
    print ""
    print "The following files will be analyzed:"
    print outDataChainFileNames
    print ""
  else:
    print ""
    print "Didn't find out*.txt files to analyze"
    print
    exit()
  return outDataChainFileNames
##########

process.merger =  cms.EDAnalyzer("LinkSynchroMerger",
  writeHistograms = cms.untracked.bool(True),
  histoFileName = cms.untracked.string("merge.root"),
  linkMonitorPSet = cms.PSet(
    useFirstHitOnly = cms.untracked.bool(True),
    dumpDelays = cms.untracked.bool(True)
  ),
 preFillLinkSynchroFileNames=dataChainFileNames() # for CRAB
 #preFillLinkSynchroFileNames= cms.untracked.vstring(
 #   '/afs/cern.ch/work/c/cwiok/Shift/CMSSW_5_3_3/src/UserCode/L1RpcTriggerAnalysis/test/Crab/out-199021.txt' 
 #) # for testing single files
)

process.MessageLogger = cms.Service("MessageLogger",
    debugModules = cms.untracked.vstring('merger'),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)

process.p = cms.Path(process.merger)
