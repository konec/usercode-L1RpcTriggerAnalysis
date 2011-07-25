#!/usr/bin/env python

import os
import re
import commands
import math

from FWCore.PythonUtilities.LumiList import *
#########################################
def makeLatestJSON(jsonsPath):
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
	file = open("runreg.cfg", "w")
	file.write(text1)
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
		command = "nsls "+path+" | grep .root | wc -l"
		files = commands.getstatusoutput(command)[1]
		counter+=int(files)
	return counter 	
#########################################
def prepareCrabCfg(fileName, datasetpath, jsonFile, pset, user_remote_dir,samplePath,nJobs,prefix="./"):

    topPath = "/afs/cern.ch/cms/L1/rpc/Shift/Crab/"   
    ###Prepare the crab.cfg
    os.system("mkdir -p "+prefix+datasetpath)
    os.system("cp "+fileName+" "+prefix+datasetpath)
    os.system("cp "+topPath+pset+" "+prefix+datasetpath)
    os.system("cp "+topPath+"/readCAFData.sh "+prefix+datasetpath)
    os.system("cp "+jsonFile+" "+prefix+datasetpath+"/goodRuns.json")
    os.system("mv "+topPath+"/runreg.cfg "+prefix+datasetpath)
    file = open(fileName, "r") #Opens the file in read-mode
    text = file.read() #Reads the file and assigns the value to a variable
    psetFullPath = commands.getstatusoutput("pwd")[1] + "/" + prefix + datasetpath + "/"
    file.close() #Closes the file (read session)
    file = open(prefix+datasetpath+"/Tmp_"+fileName, "w")
    ##Count number of files
    counter = countFiles(jsonFile,samplePath)
    ##
    grouping = int(counter/nJobs+1)
    text1 = text.replace("script_arguments =","script_arguments = " + samplePath + ", "+str(grouping))
    text2 = text1.replace("pset =","pset = "+ psetFullPath + pset)
    text3 = text2.replace("user_remote_dir =","user_remote_dir = /castor/cern.ch/user/"+user_remote_dir)
    text4 = text3.replace("number_of_jobs =","number_of_jobs =  "+str(nJobs))
    text5 = text4.replace("/castor/cern.ch/user/","/castor/cern.ch/user/"+user_remote_dir)
    text6 = text5.replace("additional_input_files = ","additional_input_files = "+topPath+prefix+datasetpath+"/goodRuns.json")
    file.write(text6)
    file.close() #Closes the file (write session)
    os.system("cd "+prefix+datasetpath+"; crab -create -submit -cfg Tmp_"+fileName+"; cd -")
#########################################
#########################################    
################
if __name__ == '__main__':	
	prefix = "./19_07_2011/"
	version = "v1/"	
	###Backup software	
	topPath = os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/test/Crab/"
	os.system("mkdir -p "+prefix+version)
	os.system("cd "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/; scram b -j 2; cd -;")
	os.system("cp -r "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/src/ "+prefix+version)
	os.system("cp -r "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/interface/ "+prefix+version)
	os.system("cp -r "+os.getenv("CMSSW_BASE")+"/src/UserCode/L1RpcTriggerAnalysis/test/synchroAnalysis_batch.py "+prefix+version)	
	###################
	aDataSet = "Run2011A/ExpressPhysics/FEVT/"
	samplePath = "/store/express/Run2011A/ExpressPhysics/FEVT/Express-v4/000/"
	jsonsPath = "/afs/cern.ch/cms/L1/rpc/Shift/JSON/"
	jsonFile = makeLatestJSON(jsonsPath)               #Automatically create a JSON with new runs
	#jsonFile = jsonsPath+"/GoodRuns_167551-167746.json" #In case you want to run with your JSON
	nJobs = 500
	'''
	###################	
	prepareCrabCfg("crab_read_CAFData.cfg", 
		       aDataSet, 
		       jsonFile,
		       "synchroAnalysis_batch.py",
		       "/u/akalinow/CMS/RPCTest/",
		       samplePath,
		       nJobs,
		       prefix+version)
        '''



   
