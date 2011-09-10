#!/usr/bin/env python

import os
import re
import commands
import math

from FWCore.PythonUtilities.LumiList import *
#########################################
def makeLatestJSON(jsonsPath,aDataSet):
	##Check last analysed run number
	maxRun = 0
	runregCfgFile = "runregTemplate.cfg"
	for fname in os.listdir(jsonsPath):
		jsonFile = jsonsPath+"/"+fname
		jsonList = LumiList(filename = jsonFile)
		runList = jsonList.getRuns()
		for run in runList:
			if int(run)>maxRun:
				maxRun = int(run)
	print "Last analysed run is:",maxRun
	print "Preparinng JSON for runs:",maxRun+1,999999
	file = open(runregCfgFile, "r") #Opens the file in read-mode
	text = file.read()
	file.close()
	text1 = text.replace("RunMin=","RunMin=" + str(maxRun+1))
	text2 = text1.replace("DBS_PDS=","DBS_PDS=" + aDataSet) # enable cross check with DBS
	file = open("runreg.cfg", "w")
	file.write(text2)
	file.close()
	##Create the JSON
	os.system("./runregparse.py")
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
def prepareCrabCfg(fileName, datasetpath, jsonFile, pset, user_remote_dir,nJobs,prefix="./"):

    topPath =  os.getenv("PWD")+"/"   
    ###Prepare the crab.cfg
    os.system("mkdir -p "+prefix+datasetpath)
    os.system("cp "+fileName+" "+prefix+datasetpath)
    os.system("cp "+topPath+"/"+pset+" "+prefix+datasetpath)
    os.system("cp "+topPath+"/readCAFData.sh "+prefix+datasetpath)
    os.system("cp "+jsonFile+" "+prefix+datasetpath+"/goodRuns.json")
    os.system("mv "+topPath+"/runreg.cfg "+prefix+datasetpath)
    file = open(fileName, "r") #Opens the file in read-mode
    text = file.read() #Reads the file and assigns the value to a variable
    psetFullPath = commands.getstatusoutput("pwd")[1] + "/" + prefix + datasetpath + "/"
    file.close() #Closes the file (read session)
    file = open(prefix+datasetpath+"/Tmp_"+fileName, "w")
    ###
    text1 = text.replace("datasetpath =","datasetpath = "+datasetpath)
    text2 = text1.replace("pset =","pset = "+ psetFullPath + pset)
    text3 = text2.replace("user_remote_dir =","user_remote_dir = /castor/cern.ch/user/"+user_remote_dir)
    text4 = text3.replace("number_of_jobs =","number_of_jobs =  "+str(nJobs))
    text5 = text4.replace("lumi_mask =","lumi_mask = "+topPath+prefix+datasetpath+"/goodRuns.json")
    file.write(text5)
    file.close() #Closes the file (write session)
    #os.system("cd "+prefix+datasetpath+"; crab -create -submit -cfg Tmp_"+fileName+"; cd -")
    #os.system("cd "+prefix+datasetpath+"; crab -create -cfg Tmp_"+fileName+"; cd -")
    command1 = "cd "+prefix+datasetpath
    result1 = os.system("cd "+prefix+datasetpath); # result of 'cd...'
    if result1 != 0:
	    print "ERROR: Cannot switch to directory: "+prefix+datasetpath
	    exit()
    command2 = command1+"; crab -create -cfg Tmp_"+fileName
    result2 = os.system(command2); # check result of 'crab -create...'
    if result2 != 0:
	    print "ERROR: Failed to create CRAB jobs using cmd: "+command2
	    exit()
    print "\nNew CRAB jobs have been created in: "
    os.system("ls -drt1 "+topPath+prefix+datasetpath+"/crab_*_*_* | tail -1");
    print "To actually submit the jobs do the following:"
    print "  cd "+topPath+prefix+datasetpath
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
	#prefix = "./05_08_2011/"
	prefix = "./25_08_2011/"
	version = "v1/"	
	###Backup software	
	topPath = os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/test/Crab/"
	os.system("mkdir -p "+prefix+version)
	os.system("cd "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/; scram b -j 2; cd -;")
	os.system("cp -r "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/src/ "+prefix+version)
	os.system("cp -r "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/interface/ "+prefix+version)
	os.system("cp -r "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/test/synchroAnalysis_batch.py "+prefix+version)	
	###################
        #aDataSet = "/ExpressPhysics/Run2011A-Express-v5/FEVT"
	aDataSet = "/ExpressPhysics/Run2011A-Express-v6/FEVT"
	jsonsPath = "/afs/cern.ch/cms/L1/rpc/Shift/JSON/"
	#jsonsPath = "/afs/cern.ch/cms/L1/rpc/soft/akalinow/CMSSW_4_2_3_patch2/src/UserCode/L1RpcTriggerAnalysis/test/Crab/JSON/"
	jsonFile = makeLatestJSON(jsonsPath,aDataSet) #Automatically create a JSON with new runs
	#jsonFile = jsonsPath+"/GoodRuns_173198-173664.json" #In case you want to run with your JSON
	nJobs = 499
	###################
	castorRpcDir = "/u/"+os.getenv("USER")+"/RPCShift/"
	#castorRpcDir = "/u/akalinow/CMS/RPCTest/"
	prepareCrabCfg("crab_read_CAFData.cfg",
		       aDataSet, 
		       jsonFile,
		       "synchroAnalysis_batch.py",
		       castorRpcDir,
		       nJobs,
		       prefix+version)                 
