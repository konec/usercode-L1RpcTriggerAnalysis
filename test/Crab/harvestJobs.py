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
    ## Verify that CRAB directory exists
    pathCrabFull = ""
    pathCrabShort = ""
    for fname in os.listdir(prefix+datasetpath):
        if re.search("crab_0_",fname)!=None:
            print fname
            if pathCrabFull == "":
                pathCrabFull = prefix+datasetpath+"/"+fname
                pathCrabShort = fname
            else:
                print "ERROR: More than one CRAB directory exists in",prefix+datasetpath
                print "       Only one is allowed!"
                os.abort()
    ## Unpack and merge the results
    if pathCrabFull != "" :
        #Get min and max number of analysed runs
        jsonFile =  prefix+datasetpath+"/goodRuns.json"
        jsonList = LumiList(filename = jsonFile)
        runList = jsonList.getRuns()
        minRun = int(runList[0])
        maxRun = int(runList[len(runList)-1])
        ## Create TEMP dir
        pathTMP = "/tmp/"+os.getenv("USER")+"/_HADD_TMP_/"
        os.system("rm -rf /tmp/${USER}/_HADD_TMP_")
        os.system("mkdir -p "+pathTMP+prefix+datasetpath)
        pathCrabResults = pathCrabFull+"/res/"
        pathCrabResultsTMP = pathTMP+prefix+datasetpath+"/"+pathCrabShort+"/res/"
        print "CRAB results from:",pathCrabResults," will be unpacked here:",pathCrabResultsTMP
        #Copy CRAB directory to TEMP dir
        os.system("cp -r "+prefix+"/. "+pathTMP+prefix+"/.")
        ## Copy all but .tgz files (if any) 
        ##os.system("cd "+pathCrabResults+";for i in efficiencyTree_*.root rawMonitor_*.root synchroAnalysis_*.root crab_*.xml CMSSW_*.stdout CMSSW_*.stderr ; do cp $i /tmp/${USER}/_HADD_TMP_; done; ls -lart ; cd -")
        ## Extract .tgz files (if any)
        ##os.system("cd "+pathCrabResults+";for i in out_files_*.tgz; do tar -xzvf $i -C /tmp/${USER}/_HADD_TMP_; done; ls -lart ; cd -")
        os.system("cd "+pathCrabResultsTMP+";for i in out_files_*.tgz; do tar -xzvf $i -C "+pathCrabResultsTMP+"; done; ls -lart ; cd -")
        ## Get missing CRAB job outputs (if any)
        ## After this step all out_files_*tgz will disappear from TMP dir
        os.system("cd "+pathTMP+prefix+datasetpath+"; crab -status")
        os.system("cd "+pathTMP+prefix+datasetpath+"; crab -get all")
        ## Get lumi summary report
        ## NOTE: 'crab -status' and 'crab -get all steps' are essential
        ##       The lumiSummary.json is created in the original submit dir...
        os.system("cd "+pathTMP+prefix+datasetpath+"; crab -report")
        ##Files to merge
        jsonFileName = "GoodRuns_"+str(minRun)+"-"+str(maxRun)+".json"
        jsonFileNameLumiSummary = "lumiSummary-"+str(minRun)+"-"+str(maxRun)+".json"
        sampleName  = "efficiencyTree-"+str(minRun)+"-"+str(maxRun)+".root"
        sampleName2 = "efficiencyHelper-"+str(minRun)+"-"+str(maxRun)+".root"
        sampleName3 = "synchroAnalysis-"+str(minRun)+"-"+str(maxRun)+".root"
        sampleName4 = "rawMonitor-"+str(minRun)+"-"+str(maxRun)+".root"
        sampleName6 = "out-"+str(minRun)+"-"+str(maxRun)+".txt"
        print "Doing hadd for ",sampleName
        os.system("cd "+pathCrabResultsTMP+"; hadd /tmp/${USER}/_HADD_TMP_/"+sampleName+" efficiencyTree_*.root")
        print "Doing hadd for ",sampleName2
        os.system("cd "+pathCrabResultsTMP+"; hadd /tmp/${USER}/_HADD_TMP_/"+sampleName2+" efficiencyHelper_*.root")
        print "Doing hadd for ",sampleName3
        os.system("cd "+pathCrabResultsTMP+"; hadd /tmp/${USER}/_HADD_TMP_/"+sampleName3+" synchroAnalysis_*.root")
        print "Doing hadd for ",sampleName4
        os.system("cd "+pathCrabResultsTMP+"; hadd /tmp/${USER}/_HADD_TMP_/"+sampleName4+" rawMonitor_*.root ; ls -lart")
        print "Preparing out.txt"
        os.system("cd "+pathCrabResultsTMP+"; rm -f /tmp/${USER}/_HADD_TMP_/"+sampleName6+"; cat C*.stdout|grep -i lb|grep -i mean >> /tmp/${USER}/_HADD_TMP_/"+sampleName6)
        ##Copy results to TMP destination
        print "Copying final results to ",pathTMP+prefix,"/ROOT"
        os.system("mkdir -p "+pathTMP+prefix+"/ROOT")
        os.system("cp "+prefix+datasetpath+"/goodRuns.json"+" "+pathTMP+prefix+"/ROOT/"+jsonFileName)
        os.system("cp "+pathCrabResults+"/lumiSummary.json"+" "+pathTMP+prefix+"/ROOT/"+jsonFileNameLumiSummary)
        os.system("cp "+pathTMP+sampleName+" "+pathTMP+prefix+"/ROOT/")
        os.system("cp "+pathTMP+sampleName2+" "+pathTMP+prefix+"/ROOT/")
        os.system("cp "+pathTMP+sampleName3+" "+pathTMP+prefix+"/ROOT/")
        os.system("cp "+pathTMP+sampleName4+" "+pathTMP+prefix+"/ROOT/")
        os.system("gzip "+pathTMP+sampleName6+"; cp "+pathTMP+sampleName6+".gz "+pathTMP+prefix+"/ROOT/")
        ##Copy results to final destination
        print "Copying final results to ",prefix,"/ROOT"
        os.system("mkdir -p "+prefix+"/ROOT")
        os.system("cp "+prefix+datasetpath+"/goodRuns.json"+" "+ prefix+"/ROOT/"+jsonFileName)
        os.system("cp "+pathCrabResults+"/lumiSummary.json"+" "+ prefix+"/ROOT/"+jsonFileNameLumiSummary)
        os.system("cp /tmp/${USER}/_HADD_TMP_/"+sampleName+" "+prefix+"/ROOT/")
        os.system("cp /tmp/${USER}/_HADD_TMP_/"+sampleName2+" "+prefix+"/ROOT/")
        os.system("cp /tmp/${USER}/_HADD_TMP_/"+sampleName3+" "+prefix+"/ROOT/")
        os.system("cp /tmp/${USER}/_HADD_TMP_/"+sampleName4+" "+prefix+"/ROOT/")
        os.system("gzip /tmp/${USER}/_HADD_TMP_/"+sampleName6+"; cp /tmp/${USER}/_HADD_TMP_/"+sampleName6+".gz "+prefix+"/ROOT/")
        ##Remove trash from tmp		    
        os.system("rm -rf /tmp/${USER}/_HADD_TMP_/*.root")
        os.system("rm -rf /tmp/${USER}/_HADD_TMP_/out*.txt.gz")
        ##Copy crab files to CASTOR 
        archiveName = (prefix+datasetpath)[2:]
        archiveName = archiveName.replace("/","_")
        archiveDir = archiveName[:-1]+"-"+str(minRun)+"-"+str(maxRun)
        archiveName = archiveDir+".tar.gz"
        os.system("cd "+pathTMP+"; tar -cvzf "+pathTMP+archiveName+" "+pathTMP+prefix+datasetpath+"/*")
        castor2 = castor+"/"+archiveDir+"/"
        print "Copying archive",archiveName,"to CASTOR path: ",castor2
        os.system("rfmkdir "+castor2)
        command = "cd "+pathTMP+"; rfcp "+archiveName+" "+castor2+"; cd -"
        os.system(command)
        print "Copying merged data",sampleName,"to CASTOR path: ",castor2
        command = "cd "+pathTMP+prefix+"/ROOT/; rfcp "+sampleName+" "+castor2+"; cd -"
        os.system(command)
        print "Copying merged data",sampleName2,"to CASTOR path: ",castor2
        command = "cd "+pathTMP+prefix+"/ROOT/; rfcp "+sampleName2+" "+castor2+"; cd -"
        os.system(command)
        print "Copying merged data",sampleName3,"to CASTOR path: ",castor2
        command = "cd "+pathTMP+prefix+"/ROOT/; rfcp "+sampleName3+" "+castor2+"; cd -"
        os.system(command)
        print "Copying merged data",sampleName4,"to CASTOR path: ",castor2
        command = "cd "+pathTMP+prefix+"/ROOT/; rfcp "+sampleName4+" "+castor2+"; cd -"
        os.system(command)
        print "Copying merged data",sampleName6+".gz","to CASTOR path: ",castor2
        command = "cd "+pathTMP+prefix+"/ROOT/; rfcp "+sampleName6+".gz "+castor2+"; cd -"
        os.system(command)
        print "Copying JSON files:",jsonFileName,",",jsonFileNameLumiSummary,"to CASTOR path: ",castor2
        command = "cd "+pathTMP+prefix+"/ROOT/; rfcp "+jsonFileName+" "+jsonFileNameLumiSummary+" "+castor2+"; cd -"
        os.system(command)
        # Remove TEMP DIR
        os.system("rm -rf /tmp/${USER}/_HADD_TMP_")
        print "Done. "+'\033[93m'+"Please remove "+prefix+datasetpath+" directory manualy!"+'\033[0m'
#########################################
####################################################################
if __name__ == '__main__':	
	prefix = "./2012_05_17/"
	version = "mb/"
# 	aDataSet = "/MinimumBias/Run2011A-PromptReco-v5/RECO"
#	aDataSet = "/MinimumBias/Run2011A-PromptReco-v6/RECO"
# 	aDataSet = "/MinimumBias/Run2011B-MuonTrack-PromptSkim-v1/RAW-RECO"
#  	aDataSet = "/SingleMu/Run2011B-WMu-PromptSkim-v1/RAW-RECO"
#	aDataSet = "ExpressPhysics/Run2011B-Express-v1/FEVT"
	aDataSet = "/MinimumBias/Run2012A-PromptReco-v1/RECO"
        castor = "${CASTOR_HOME}/RPCShift/"
        unpackAndMergeFiles(aDataSet,prefix+version,castor)
