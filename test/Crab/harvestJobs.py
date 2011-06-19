#!/usr/bin/env python

import os
import re
import commands


#########################################
#########################################
def unpackAndMergeFiles(datasetpath, prefix="./"):
    ###
    pwd =  os.getenv("PWD")
    for fname in os.listdir(prefix+datasetpath):
	    print fname
	    if re.search("crab_0",fname)!=None:
		    path = prefix+datasetpath+"/"+fname+"/res/"
		    print path
		    os.listdir(path)
		    os.system("cd "+path+";for i in *.tgz; do tar -xzvf $i -C /tmp/${USER}; done; cd -")
		    sampleName = "efficiencyTree.root"
		    os.system("cd /tmp/${USER}; hadd "+sampleName+" efficiencyTree_*.root")
		    os.system("mkdir -p "+prefix+"/ROOT")                    
		    os.system("mv /tmp/${USER}/"+sampleName +" "+ prefix+"/ROOT")
                    os.system("rm -rf /tmp/${USER}/*.root")
                    os.system("rm -rf /tmp/${USER}/CMSSW*.stdout")
                    os.system("rm -rf /tmp/${USER}/CMSSW*.stderr")
                    os.system("rm -rf /tmp/${USER}/crab*.xml")                    
#########################################
####################################################################
if __name__ == '__main__':	
	prefix = "./10_06_2011/"
	version = "v1/"
        aDataSet = "Run2011A/ExpressPhysics/FEVT/"        
        unpackAndMergeFiles(aDataSet,prefix+version)
        




   
