#!/bin/sh



cat >> pset.py << @EOF


import os, commands
from FWCore.PythonUtilities.LumiList import *

process.source.fileNames = cms.untracked.vstring()
jsonFile = "goodRuns.json"
jsonList = LumiList(filename = jsonFile)
runList = jsonList.getRuns()
base="/castor/cern.ch/cms/"
samplePath = "$2"
counter = 0
for run in runList:
        path = base+"/"+samplePath+"/"+run[0:3]+"/"+run[3:len(run)]
	command = "nsls "+path+" | grep .root"
	files = commands.getstatusoutput(command)[1]
	for file in files.split("\n"):
		if counter<$1*$3+1 and counter>=($1-1)*$3+1:
		    path = samplePath+"/"+run[0:3]+"/"+run[3:len(run)]
		    process.source.fileNames.append(str(path)+"/"+str(file))
		    print str(path)+"/"+str(file)
		counter+=1
       
#print process.source.fileNames
@EOF

echo "------------------------"	
echo "Parameters: " $1 $2 $3
echo "CMSSW_BASE: " $CMSSW_BASE
#python pset.py
cmsRun pset.py
