#!/usr/bin/env python

import os
import re
import commands

from FWCore.PythonUtilities.LumiList import *
#########################################
#########################################
def unpackAndMergeFiles(datasetpath,prefix,castor):
    ###
    pwd =  os.getenv("PWD")
    minRun = 0
    maxRun = 0
    sampleName = "None"
    for fname in os.listdir(prefix+datasetpath):
	    print fname
	    if re.search("crab_0",fname)!=None:
		    path = prefix+datasetpath+"/"+fname+"/res/"
		    print path
		    #Get min and max number of analysed runs
		    jsonFile =  prefix+datasetpath+"/goodRuns.json"
		    jsonList = LumiList(filename = jsonFile)
		    runList = jsonList.getRuns()
		    minRun = int(runList[0])
		    maxRun = int(runList[len(runList)-1])
		    ##Unpack and merge results
		    os.system("cd "+path+";for i in *.tgz; do tar -xzvf $i -C /tmp/${USER}; done; cd -")
		    sampleName = "efficiencyTree-"+str(minRun)+"-"+str(maxRun)+".root"
		    os.system("cd /tmp/${USER}; hadd "+sampleName+" efficiencyTree_*.root")
		    ##Copy results to final destination
		    os.system("mkdir -p "+prefix+"/ROOT")                    		   
		    jsonFileName = "GoodRuns_"+str(minRun)+"-"+str(maxRun)+".json"
		    os.system("cp "+prefix+datasetpath+"/goodRuns.json"+" "+ prefix+"/ROOT/"+jsonFileName)
		    os.system("mv /tmp/${USER}/"+sampleName +" "+ prefix+"/ROOT/")
		    ##Remove trash from tmp		    
                    os.system("rm -rf /tmp/${USER}/*.root")
                    os.system("rm -rf /tmp/${USER}/CMSSW*.stdout")
                    os.system("rm -rf /tmp/${USER}/CMSSW*.stderr")
                    os.system("rm -rf /tmp/${USER}/crab*.xml")
    ##Copy crab files to CASTOR
    archiveName = (prefix+datasetpath)[2:]
    archiveName = archiveName.replace("/","_")
    archiveName = archiveName[:-1]+"-"+str(minRun)+"-"+str(maxRun)+".tar.gz"
    os.system("tar -cvzf /tmp/${USER}/"+archiveName+" "+prefix+datasetpath+"/*")
    print "Copying archive",archiveName,"to CASTOR path: ",castor
    os.system("rfmkdir "+castor)
    command = "cd /tmp/${USER}; rfcp "+archiveName+" "+castor+"; cd -"
    os.system(command)
    print "Copying merged data",sampleName,"to CASTOR path: ",castor
    command = "cd "+prefix+"/ROOT/; rfcp "+sampleName+" "+castor+"; cd -"
    os.system(command)
    print "Done. "+'\033[93m'+"Please remove "+prefix+datasetpath+" directory manualy!"+'\033[0m'
#########################################
####################################################################
if __name__ == '__main__':	
	prefix = "./05_07_2011/"
	version = "v1/"
        aDataSet = "Run2011A/ExpressPhysics/FEVT/"
        castor = "${CASTOR_HOME}/RPCShift/"
        unpackAndMergeFiles(aDataSet,prefix+version,castor)
        




   
