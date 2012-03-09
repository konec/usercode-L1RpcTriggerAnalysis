#!/usr/bin/env python

import os
import re
import commands
castor_path = "/castor/cern.ch/user/r/rybinska/Feb/efficiency/JPsi_MuOnia_13_2/"
prefix_1 = "loose"
prefix_2 = ""
prefix_3 = "tight"
file_1 = "rfio:/castor/cern.ch/user/r/rybinska/Feb/JPsi_MuOnia_13_2.root"
#file_2 = "rfio:/castor/cern.ch/user/r/rybinska/9911/TnPTree_JPsi_2011B_v1.root" 
#file_3 = "rfio:/castor/cern.ch/user/r/rybinska/9911/Tree_JPsi_Data_tight.root" 
#eta1 = 0
#eta2=0.83
dr2 = 2
dphiq1 = -0.6
drs = [0.5]
#dphiqs=[0.6]
flagRPC = "hasL1RPCptValue"
flagDT = "hasL1DTptValue"
flagCSC = "hasL1CSCptValue"
flagsRPC = ["hasL1RPCMatch",flagRPC+"5",flagRPC+"10",flagRPC+"15",flagRPC+"16",flagRPC+"20",flagRPC+"30"]
flagsDT = ["hasL1DTMatch",flagDT+"5",flagDT+"10",flagDT+"15",flagDT+"16",flagDT+"20",flagDT+"30"]
flagsCSC = ["hasL1CSCMatch",flagCSC+"5",flagCSC+"10",flagCSC+"15",flagCSC+"16",flagCSC+"20",flagCSC+"30"] 
from Tree_Analyzer_JPsi_template import *
#########################################
def processSingleSample(file_in,file_out,flag,dr1,eta1,eta2):
        process.TagProbeFitTreeAnalyzer.InputFileNames = cms.vstring(file_in)
        process.TagProbeFitTreeAnalyzer.OutputFileName = cms.string(file_out)
        process.TagProbeFitTreeAnalyzer.Efficiencies = cms.PSet(
               Eff1 = cms.PSet(
                    EfficiencyCategoryAndState = cms.vstring(flag,"true"),
                    UnbinnedVariables = cms.vstring("mass"),
                    BinnedVariables = cms.PSet(
                        pt = ptBins,
                        #pt = cms.vdouble(15,20,25,30),
                        abseta = cms.vdouble(eta1,eta2),
                        pair_drM2 = cms.vdouble(dr1,dr2),
                        #pair_dphiVtxTimesQ = cms.vdouble(dphiq1,dphiq2),
                    ),
                    BinToPDFmap = cms.vstring("gaussPlusLinear"),
               ),
        )
	###
	out = open('tmpConfig.py','w')
	out.write(process.dumpPython())
	out.close()
	print(process.TagProbeFitTreeAnalyzer)
	os.system("cp tmpConfig.py "+file_out+".py")
	os.system("cmsRun tmpConfig.py > "+file_out+".log")
	os.system("rfcp "+file_out+" "+castor_path+file_out)
	os.system("rm "+file_out+" "+file_out+".log")
#########################################
def processSingleSampleEta(file_in,file_out,flag,dr1,etab):
        process.TagProbeFitTreeAnalyzer.InputFileNames = cms.vstring(file_in)
        process.TagProbeFitTreeAnalyzer.OutputFileName = cms.string(file_out)
        process.TagProbeFitTreeAnalyzer.Efficiencies = cms.PSet(
               Eff1 = cms.PSet(
                    EfficiencyCategoryAndState = cms.vstring(flag,"true"),
                    UnbinnedVariables = cms.vstring("mass"),
                    BinnedVariables = cms.PSet(
                        pt = cms.vdouble(0,1000),
                        eta = etab,
                        pair_drM2 = cms.vdouble(dr1,dr2),
                        #pair_dphiVtxTimesQ = cms.vdouble(dphiq1,dphiq2),
                    ),
                    BinToPDFmap = cms.vstring("gaussPlusLinear"),
               ),
        )
	###
	out = open('tmpConfig.py','w')
	out.write(process.dumpPython())
	out.close()
	print(process.TagProbeFitTreeAnalyzer)
	os.system("cp tmpConfig.py "+file_out+".py")
	os.system("cmsRun tmpConfig.py > "+file_out+".log")
	os.system("rfcp "+file_out+" "+castor_path+file_out)
	os.system("rm "+file_out+" "+file_out+".log")	
####################################################################
#        hasL1RPCMatch 
#        hasL1RPCptValue5,10,16,20,25,30,35,40,50,60,100 
#        hasL1DTMatch
#        hasL1DTptValue5 ,10,16,20,25,30,35,40,50,60,100 
#        abseta = 0 - 1.6
#        pair_dphiVtxTimesQ = -0.6 - 0.6
#        pair_drM2 = 0 - 2
####################################################################

####################################################################


for i in drs:
    
        processSingleSampleEta(file_1,"Efficiency_"+str(i)+"_etabins1_RPC.root","hasL1RPCMatch",i,etaBins1)
        processSingleSampleEta(file_1,"Efficiency_"+str(i)+"_etabins1_DT.root","hasL1DTMatch",i,etaBins1)
        processSingleSampleEta(file_1,"Efficiency_"+str(i)+"_etabins1_CSC.root","hasL1CSCMatch",i,etaBins1)
        processSingleSampleEta(file_1,"Efficiency_"+str(i)+"_etabins2_RPC.root","hasL1RPCMatch",i,etaBins2)
        processSingleSampleEta(file_1,"Efficiency_"+str(i)+"_etabins2_DT.root","hasL1DTMatch",i,etaBins2)
        processSingleSampleEta(file_1,"Efficiency_"+str(i)+"_etabins2_CSC.root","hasL1CSCMatch",i,etaBins2)

        for k in flagsDT:
                processSingleSample(file_1,"Efficiency_"+str(i)+"_"+str(k)+".root",k,i,0.,0.8)
       
        for k in flagsRPC:
                processSingleSample(file_1,"Efficiency_"+str(i)+"_"+str(k)+"_barrel.root",k,i,0.,0.8)
	        processSingleSample(file_1,"Efficiency_"+str(i)+"_"+str(k)+"_overlap.root",k,i,0.8,1.25)
                processSingleSample(file_1,"Efficiency_"+str(i)+"_"+str(k)+"_endcap.root",k,i,1.25,1.6)
	
	for k in flagsCSC:
                processSingleSample(file_1,"Efficiency_"+str(i)+"_"+str(k)+".root",k,i,1.25,1.6)
