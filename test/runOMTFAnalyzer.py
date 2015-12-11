import FWCore.ParameterSet.Config as cms
process = cms.Process("OMTFEmulation")
import os
import sys
import commands

verbose = False

process.load("FWCore.MessageLogger.MessageLogger_cfi")

if verbose: 
    process.MessageLogger = cms.Service("MessageLogger",
       suppressInfo       = cms.untracked.vstring('AfterSource', 'PostModule'),
       destinations   = cms.untracked.vstring(
                                             'detailedInfo'
                                             ,'critical'
                                             ,'cout'
                    ),
       categories = cms.untracked.vstring(
                                        'CondDBESSource'
                                        ,'EventSetupDependency'
                                        ,'Geometry'
                                        ,'MuonGeom'
                                        ,'GetManyWithoutRegistration'
                                        ,'GetByLabelWithoutRegistration'
                                        ,'Alignment'
                                        ,'SiStripBackPlaneCorrectionDepESProducer'
                                        ,'SiStripLorentzAngleDepESProducer'
                                        ,'SiStripQualityESProducer'
                                        ,'TRACKER'
                                        ,'HCAL'
        ),
       critical       = cms.untracked.PSet(
                        threshold = cms.untracked.string('ERROR') 
        ),
       detailedInfo   = cms.untracked.PSet(
                      threshold  = cms.untracked.string('INFO'), 
                      CondDBESSource  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                      EventSetupDependency  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                      Geometry  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                      MuonGeom  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                      Alignment  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                      GetManyWithoutRegistration  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                      GetByLabelWithoutRegistration  = cms.untracked.PSet (limit = cms.untracked.int32(0) ) 

       ),
       cout   = cms.untracked.PSet(
                threshold  = cms.untracked.string('INFO'), 
                CondDBESSource  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                EventSetupDependency  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                Geometry  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                MuonGeom  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                Alignment  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                GetManyWithoutRegistration  = cms.untracked.PSet (limit = cms.untracked.int32(0) ), 
                GetByLabelWithoutRegistration  = cms.untracked.PSet (limit = cms.untracked.int32(0) ) 
                ),
                                        )

if not verbose:
    process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(50000)
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

process.source = cms.Source(
    'PoolSource',
    #fileNames = cms.untracked.vstring('file:/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/DYToMuMu_M-50_Tune4C_13TeV-pythia8//Fall13dr-tsg_PU20bx25_POSTLS162_V2-v1/GEN-SIM-RAW/data/DYToMuMu_M-50_Tune4C_13TeV-pythia8_10_1_Khx.root')
    fileNames = cms.untracked.vstring('file:/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEtaTestSample/750_FullEta_v2/data//SingleMu_4_p_1_1_gOq.root') 
    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000))


path = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/WToMuNu_Tune4C_13TeV-pythia8/Fall13dr-tsg_PU40bx50_POSTLS162_V2-v1/GEN-SIM-RAW/data/"
command = "ls "+path
fileList = commands.getoutput(command).split("\n")

'''
process.source.fileNames =  cms.untracked.vstring()
for aFile in fileList:
    process.source.fileNames.append('file:'+path+aFile)
print  process.source.fileNames   
'''

###PostLS1 geometry used
process.load('Configuration.Geometry.GeometryExtended2015_cff')
process.load('Configuration.Geometry.GeometryExtended2015Reco_cff')
############################
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

###OMTF emulator configuration
process.load('L1Trigger.L1OverlapMuonTrackFinder.OMTFProducer_cfi')

###OMTF analyser configuration
process.omtfAnalyser = cms.EDAnalyzer("OMTFAnalyzer",
                                      OMTFCandSrc = cms.InputTag('omtfEmulator','OMTF'),
                                      GMTCandSrc = cms.InputTag('simGmtDigis'),
                                      genPartSrc = cms.InputTag('genParticles'),
                                      g4SimTrackSrc = cms.InputTag('g4SimHits'),
                                      anaEff      = cms.PSet( maxDR= cms.double(9999.)),
                                      multiplyEvents = cms.uint32(1)
                                      )

###Gen level filter configuration
process.MuonEtaFilter = cms.EDFilter("SimTrackEtaFilter",
                                minNumber = cms.uint32(1),
                                src = cms.InputTag("g4SimHits"),
                                cut = cms.string("momentum.eta<1.24 && momentum.eta>0.83 && momentum.pt>1 && abs(type)==13")
                                )

process.GenMuPath = cms.Path(process.MuonEtaFilter)
##########################################

process.L1TMuonSeq = cms.Sequence( process.L1TMuonTriggerPrimitives+ 
                                   process.omtfEmulator+process.omtfAnalyser)

process.L1TMuonPath = cms.Path(process.MuonEtaFilter*process.L1TMuonSeq)

process.schedule = cms.Schedule(process.GenMuPath,process.L1TMuonPath)

