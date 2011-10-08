#!/usr/bin/env python

import os
import re
import commands
import math

from FWCore.PythonUtilities.LumiList import *
#########################################
#def makeLatestJSON(jsonsPath,aDataSet,runregCfgFile="runregTemplate.cfg"):
def makeLatestJSON(jsonsPath,aDataSet,runRegCfg="runregTemplate.cfg",runRegPy="runregparse.py"):
	##Check last analysed run number
	maxRun = 0
	for fname in os.listdir(jsonsPath):
		jsonFile = jsonsPath+"/"+fname
		jsonList = LumiList(filename = jsonFile)
		runList = jsonList.getRuns()
		for run in runList:
			if int(run)>maxRun:
				maxRun = int(run)
	print "Last analysed run is:",maxRun
	print "Preparinng JSON for runs:",maxRun+1,999999
	file = open(runRegCfg, "r") #Opens the file in read-mode
	text = file.read()
	file.close()
	text1 = text.replace("RunMin=","RunMin=" + str(maxRun+1))
	text2 = text1.replace("DBS_PDS=","DBS_PDS=" + aDataSet) # enable cross check with DBS
	file = open("runreg.cfg", "w")
	file.write(text2)
	file.close()
	##Create the JSON
	os.system(runRegPy)
#	os.system(os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/test/runregparse.py")
	##Rename JSON file to include run range
	jsonFile = "GoodRuns.json"
	jsonList = LumiList(filename = jsonFile)
	runList = jsonList.getRuns()
	minRun = int(runList[0])
	maxRun = int(runList[len(runList)-1])
	jsonFileName = "GoodRuns_"+str(minRun)+"-"+str(maxRun)+".json"
	os.system("mv GoodRuns.json "+jsonsPath+"/"+jsonFileName)
	return jsonsPath+"/"+jsonFileName
#########################################
def countFiles(jsonFile,samplePath):
	jsonList = LumiList(filename = jsonFile)
	runList = jsonList.getRuns()
	base="/castor/cern.ch/cms/"
	counter = 0
	for run in runList:       
		path = base+"/"+samplePath+"/"+run[0:3]+"/"+run[3:len(run)]
                #command = "nsls "+path+" | grep .root | wc -l"
		command1 = "nsls "+path
		if commands.getstatusoutput(command1)[0] != 0:
			print "Skipping run: "+run
			continue
		command = command1+" | grep .root | wc -l"
		files = commands.getstatusoutput(command)[1]
		counter+=int(files)
	return counter
#########################################
	
def prepareCrabCfg(prefix,
		   datasetpath, 
		   jsonFile,
		   nJobs,
		   user_remote_dir,
		   pset,
		   runRegCfg,
		   runRegPy,
		   readCafCfg,
		   readCafSh):
    #prepareCrabCfg(fileName, datasetpath, jsonFile, pset, user_remote_dir,nJobs,prefix="./"):
    ### Create jobs' directory
    os.system("mkdir -p "+prefix+"/"+datasetpath)
    ### Set paths
    topPath =  os.getenv("PWD")+"/"
    fullPrefixPath = commands.getstatusoutput("cd "+topPath+"; cd "+prefix+"; pwd")[1]
    ### Backup software
    os.system("mkdir -p "+prefix+"/"+datasetpath)
    os.system("cd "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/; scram b -j 2; cd -;")
    os.system("cp -r "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/src/ "+prefix+"/")
    os.system("cp -r "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/interface/ "+prefix+"/")
    os.system("cp -r "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/plugins/ "+prefix+"/")
    os.system("cp -r "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/macros/ "+prefix+"/")
    os.system("cp -r "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/test/ "+prefix+"/")
    os.system("cp "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/Build* "+prefix+"/")
    os.system("cp "+pset+" "+prefix+"/"+datasetpath)
    os.system("cp "+pset+" "+prefix+"/")
    os.system("cp "+runRegCfg+" "+prefix+"/"+datasetpath)
    os.system("cp "+runRegPy+" "+prefix+"/"+datasetpath)
    os.system("cp "+readCafCfg+" "+prefix+"/"+datasetpath)
    os.system("cp "+readCafSh+" "+prefix+"/"+datasetpath)
    os.system("cp "+jsonFile+" "+prefix+"/"+datasetpath+"/goodRuns.json")
    ### Prepare the crab.cfg
    os.system("mv "+topPath+"/runreg.cfg "+prefix+"/"+datasetpath)
    file = open(readCafCfg, "r") #Opens the file in read-mode
    text = file.read() #Reads the file and assigns the value to a variable
    #psetFullPath = commands.getstatusoutput("pwd")[1] + "/" + prefix + datasetpath + "/"
    file.close() #Closes the file (read session)
    tmpReadCafCfg = fullPrefixPath+datasetpath+"/Tmp_"+commands.getstatusoutput("basename "+readCafCfg)[1]
    file = open(tmpReadCafCfg, "w")
    ###
    text1 = text.replace("datasetpath =","datasetpath = "+datasetpath)
    text2 = text1.replace("pset =","pset = "+pset)
    text3 = text2.replace("user_remote_dir =","user_remote_dir = /castor/cern.ch/user/"+user_remote_dir)
    text4 = text3.replace("number_of_jobs =","number_of_jobs =  "+str(nJobs))
    text5 = text4.replace("lumi_mask =","lumi_mask = "+fullPrefixPath+datasetpath+"/goodRuns.json")
    file.write(text5)
    file.close() #Closes the file (write session)
    ######
    ###### If you want to submit all jobs at once uncomment this line...
    #os.system("cd "+prefix+datasetpath+"; crab -create -submit -cfg "+tmpReadCafCfg+"; cd -")
    ###### ...and comment out all lines below:
    ######
    command1 = "cd "+prefix+datasetpath
    result1 = os.system("cd "+prefix+datasetpath); # result of 'cd...'
    if result1 != 0:
	    print "ERROR: Cannot switch to directory: "+prefix+datasetpath
	    exit()
    command2 = command1+"; crab -create -cfg "+tmpReadCafCfg
    result2 = os.system(command2); # check result of 'crab -create...'
    if result2 != 0:
	    print "ERROR: Failed to create CRAB jobs using cmd: "+command2
	    exit()
    print "\nNew CRAB jobs have been created in: "
    os.system("ls -drt1 "+prefix+datasetpath+"/crab_*_*_* | tail -1");
    print "To actually submit the jobs do the following:"
    print "  cd "+fullPrefixPath+datasetpath
    print "  crab -submit all"
    print "or:"
    print "  crab -submit 50 -continue      # to submit next 50 jobs"
    print "or:"
    print "  crab -submit 11-20 -continue   # to submit jobs #11...#20"
    print "or:"
    print "  crab -submit 52, -continue     # to submit job #52 (comma is important!)\n"
    print "To resubmit job #33 do the following:"
    print "  crab -status   # this is important"
    print "  crab -get 33,  # this is important"
    print "  crab -resubmit 33,\n"
    
#########################################
#########################################    
################
if __name__ == '__main__':	
	###################
	prefix = "./2011_10_08/"
	version = "v1/"	
	aDataSet = "/ExpressPhysics/Run2011B-Express-v1/FEVT"
	nJobs = 499
	###################
	topPath = os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/test/Crab/"
        analysisPy = topPath+"/synchroAnalysisCrab.py"
	runRegCfg  = topPath+"/runregTemplate.cfg"
	runRegPy   = topPath+"/../runregparse.py"
	readCafCfg = topPath+"/crab_read_CAFData.cfg"
	readCafSh  = topPath+"/readCAFData.sh"
	###################
	jsonsPath = "/afs/cern.ch/cms/L1/rpc/Shift/JSON/"
	#jsonsPath = "/afs/cern.ch/cms/L1/rpc/soft/akalinow/CMSSW_4_2_3_patch2/src/UserCode/L1RpcTriggerAnalysis/test/Crab/JSON/"
	#jsonsPath = "/afs/cern.ch/user/c/cwiok/scratch0/rpc2011/joby_14sep2011/JSON/"
	jsonFile = makeLatestJSON(jsonsPath,aDataSet,runRegCfg,runRegPy) # automatically create a JSON for runs not analyzed yet
	#jsonFile = jsonsPath+"/GoodRuns_175832-176023.json" #In case you want to run with your JSON
	###################
	castorRpcDir = "/u/"+os.getenv("USER")+"/RPCShift/"
	###################
	prepareCrabCfg(prefix+version,
		       aDataSet, 
		       jsonFile,
		       nJobs,
		       castorRpcDir,
		       analysisPy,
		       runRegCfg,
		       runRegPy,
		       readCafCfg,
		       readCafSh)
