#!/usr/bin/env python

import os
import re
import commands
castor_path = "/castor/cern.ch/user/r/rybinska/Feb/efficiency/Z_IsoMu24_2p1_1/"
prefix_1 = "loose"
prefix_2 = ""
prefix_3 = "tight"
file_1 = "rfio:/castor/cern.ch/user/r/rybinska/Feb/Z_IsoMu24_2p1_1.root" 
#file_2 = 
#file_3 = 
#eta1 = 0
#eta2=0.6
flagRPC = "hasL1RPCptValue"
flagDT = "hasL1DTptValue"
flagCSC = "hasL1CSCptValue"
flagsRPC = ["hasL1RPCMatch",flagRPC+"5",flagRPC+"10",flagRPC+"15",flagRPC+"16",flagRPC+"20",flagRPC+"30"]
flagsDT = ["hasL1DTMatch",flagDT+"5",flagDT+"10",flagDT+"15",flagDT+"16",flagDT+"20",flagDT+"30"]
flagsCSC = ["hasL1CSCMatch",flagCSC+"5",flagCSC+"10",flagCSC+"15",flagCSC+"16",flagCSC+"20",flagCSC+"30"] 
fits = ["vpvPlusExpo"]
from Tree_Analyzer_Z_template import *
#########################################
def processSingleSample(file_in,file_out,flag,eta1,eta2):
        process.TagProbeFitTreeAnalyzer.InputFileNames = cms.vstring(file_in)
        process.TagProbeFitTreeAnalyzer.OutputFileName = cms.string(file_out)
        process.TagProbeFitTreeAnalyzer.Efficiencies = cms.PSet(
               Eff1 = cms.PSet(
                    EfficiencyCategoryAndState = cms.vstring(flag,"true"),
                    UnbinnedVariables = cms.vstring("mass"),
                    BinnedVariables = cms.PSet(
                        pt = ptBins,
                        abseta = cms.vdouble(eta1,eta2),
		        pair_drM2 = cms.vdouble(0.5,100),
                    ),
                    BinToPDFmap = cms.vstring("vpvPlusExpo"),
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
def processSingleSampleEta(file_in,file_out,flag,etab):
        process.TagProbeFitTreeAnalyzer.InputFileNames = cms.vstring(file_in)
        process.TagProbeFitTreeAnalyzer.OutputFileName = cms.string(file_out)
        process.TagProbeFitTreeAnalyzer.Efficiencies = cms.PSet(
               Eff1 = cms.PSet(
                    EfficiencyCategoryAndState = cms.vstring(flag,"true"),
                    UnbinnedVariables = cms.vstring("mass"),
                    BinnedVariables = cms.PSet(
                        pt = cms.vdouble(0,1000),
                        eta = etab,
                        pair_drM2 = cms.vdouble(0.5,100),
                    ),
                    BinToPDFmap = cms.vstring("vpvPlusExpo"),
               ),
        )
	###
	out = open('tmpConfig1.py','w')
	out.write(process.dumpPython())
	out.close()
	print(process.TagProbeFitTreeAnalyzer)
	os.system("cp tmpConfig1.py "+file_out+".py")
	os.system("cmsRun tmpConfig1.py > "+file_out+".log")
	os.system("rfcp "+file_out+" "+castor_path+file_out)
	os.system("rm "+file_out+" "+file_out+".log")	


####################################################################
#        hasL1RPCMatch 
#        hasL1RPCptValue5,10,15,20,25,30,35,40,50,60,100 
#        hasL1DTMatch
#        hasL1DTptValue5 ,10,15,20,25,30,35,40,50,60,100 
#        abseta = 0 - 1.6
#        pair_dphiVtxTimesQ = -0.6 - 0.6
#        pair_drM2 = 0 - 2
####################################################################

####################################################################

processSingleSampleEta(file_1,"Efficiency_etabins_RPC_etaBins1.root","hasL1RPCMatch",etaBins1)
processSingleSampleEta(file_1,"Efficiency_etabins_DT_etaBins1.root","hasL1DTMatch",etaBins1)
processSingleSampleEta(file_1,"Efficiency_etabins_CSC_etaBins1.root","hasL1CSCMatch",etaBins1)
processSingleSampleEta(file_1,"Efficiency_etabins_RPC_etaBins2.root","hasL1RPCMatch",etaBins2)
processSingleSampleEta(file_1,"Efficiency_etabins_DT_etaBins2.root","hasL1DTMatch",etaBins2)
processSingleSampleEta(file_1,"Efficiency_etabins_CSC_etaBins2.root","hasL1CSCMatch",etaBins2)

for k in flagsDT:
        processSingleSample(file_1,"Efficiency_"+str(k)+".root",k,0.,0.8) 
                     
for k in flagsRPC:
        processSingleSample(file_1,"Efficiency_"+str(k)+"_barrel.root",k,0.,0.8)
        processSingleSample(file_1,"Efficiency_"+str(k)+"_overlap.root",k,0.8,1.25)
        processSingleSample(file_1,"Efficiency_"+str(k)+"_endcap.root",k,1.25,1.6)
        
for k in flagsCSC:
        processSingleSample(file_1,"Efficiency_"+str(k)+".root",k,1.25,1.6)
