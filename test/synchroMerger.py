import os
import FWCore.ParameterSet.Config as cms
process = cms.Process("Analysis")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1))
process.source = cms.Source("EmptySource")

process.load("DQMServices.Core.DQM_cfg")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.GlobalTag.globaltag = 'MC_37Y_V5::All'

dataChainFileNames = cms.untracked.vstring()

##Add data
path = "/afs/cern.ch/cms/L1/rpc/Shift/Crab/"
pathTmp = "/tmp/"+os.getenv("USER")+"/__OUT_TXT__/"

datesToAdd = ["05_08_2011/v1/","13_08_2011/v1/","25_08_2011/v1/"]
#datesToAdd = ["09_06_2011/v1/","10_06_2011/v1/","19_06_2011/v1/","22_06_2011/v1","27_06_2011/v1","05_07_2011/v1","01_08_2011/v1/"]
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
                        dataChainFileNames.append(pathTmp+"/"+fname1[:-3])
if pathTmpCreated==1:
    print ""
    print "The following files will be analyzed:"
    print dataChainFileNames
    print ""
else:
    print ""
    print "Didn't find out*.txt files to analyze"
    print
    exit()
##########

process.merger =  cms.EDAnalyzer("LinkSynchroMerger",
  writeHistograms = cms.untracked.bool(True),
  histoFileName = cms.untracked.string("merge.root"),
  linkMonitorPSet = cms.PSet(
    useFirstHitOnly = cms.untracked.bool(True),
    dumpDelays = cms.untracked.bool(True)
  ),
  preFillLinkSynchroFileNames=dataChainFileNames
 # preFillLinkSynchroFileNames= cms.untracked.vstring(
 #   '/afs/cern.ch/cms/L1/rpc/Shift/Crab/05_08_2011/v1/ROOT/out-170722-172791.txt',
 #   '/afs/cern.ch/cms/L1/rpc/Shift/Crab/13_08_2011/v1/ROOT/out-172798-172999.txt',
 #   '/afs/cern.ch/cms/L1/rpc/Shift/Crab/25_08_2011/v1/ROOT/out-173198-173664.txt'
 # )
)

process.MessageLogger = cms.Service("MessageLogger",
    debugModules = cms.untracked.vstring('merger'),
    destinations = cms.untracked.vstring('cout'),
    cout = cms.untracked.PSet( threshold = cms.untracked.string('DEBUG'))
)

process.p = cms.Path(process.merger)
