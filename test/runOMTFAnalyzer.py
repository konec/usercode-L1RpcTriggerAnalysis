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
    fileNames = cms.untracked.vstring('file:/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/JPsi_21kEvents.root')
    #fileNames = cms.untracked.vstring('file:/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEtaTestSample/720_FullEta_v1/data/SingleMu_16_p_1_2_TWz.root')
    #fileNames = cms.untracked.vstring('file:/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEtaTestSample/720_FullEta_v1/data//SingleMu_16_m_1_2_hCg.root') 
    #fileNames = cms.untracked.vstring('file:/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEtaTestSample/720_FullEta_v1/data//SingleMu_4_p_1_2_EFs.root', 
    #                                  'file:/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEtaTestSample/720_FullEta_v1/data//SingleMu_4_p_2_2_3b1.root')
    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(20000))

'''
path = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEtaTestSample/720_FullEta_v1/data/"
command = "ls "+path
fileList = commands.getoutput(command).split("\n")

process.source.fileNames =  cms.untracked.vstring()
for aFile in fileList:
    process.source.fileNames.append('file:'+path+aFile)
'''

###PostLS1 geometry used
process.load('Configuration.Geometry.GeometryExtendedPostLS1Reco_cff')
process.load('Configuration.Geometry.GeometryExtendedPostLS1_cff')
############################
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

path = os.environ['CMSSW_BASE']+"/src/UserCode/OMTFSimulation/data/"


process.load('L1Trigger.L1TMuon.L1TMuonTriggerPrimitiveProducer_cfi')

path1 = "/home/akalinow/scratch/CMS/OverlapTrackFinder/Emulator/CMSSW_7_2_1/src/UserCode/OMTFSimulation/test/"
###OMTF emulator configuration
process.omtfEmulator = cms.EDProducer("OMTFProducer",
                                      TriggerPrimitiveSrc = cms.InputTag('L1TMuonTriggerPrimitives'),
                                      dumpResultToXML = cms.bool(False),                                     
                                      dumpGPToXML = cms.bool(False),                                     
                                      makeConnectionsMaps = cms.bool(False),
                                      dropRPCPrimitives = cms.bool(False),                                    
                                      dropDTPrimitives = cms.bool(False),                                    
                                      dropCSCPrimitives = cms.bool(False),                                        
                                      omtf = cms.PSet(

        configXMLFile = cms.string(path+"hwToLogicLayer_721_5760.xml"),
        patternsXMLFiles = cms.vstring(path+"Patterns_ipt4_31_5760.xml"),
        )
                                      )

###OMTF analyser configuration
process.omtfAnalyser = cms.EDAnalyzer("OMTFAnalyzer",
                                      OMTFCandSrc = cms.InputTag('omtfEmulator','OMTF'),
                                      GMTCandSrc = cms.InputTag('simGmtDigis'),
                                      g4SimTrackSrc = cms.InputTag('g4SimHits'),
                                      anaEff      = cms.PSet( maxDR= cms.double(9999.)),
                                      )

###Gen level filter configuration
process.MuonEtaFilter = cms.EDFilter("SimTrackEtaFilter",
                                minNumber = cms.uint32(1),
                                src = cms.InputTag("g4SimHits"),
                                cut = cms.string("momentum.eta<1.24 && momentum.eta>0.83 &&  momentum.pt>1")
                                )

process.GenMuPath = cms.Path(process.MuonEtaFilter)
##########################################



process.L1TMuonSeq = cms.Sequence( process.L1TMuonTriggerPrimitives+ 
                                   process.omtfEmulator+process.omtfAnalyser)

process.L1TMuonPath = cms.Path(process.MuonEtaFilter*process.L1TMuonSeq)


process.out = cms.OutputModule("PoolOutputModule", 
   fileName = cms.untracked.string("Test.root"),
                               SelectEvents = cms.untracked.PSet(
        SelectEvents = cms.vstring('GenMuPath')
        )
                               )

process.output_step = cms.EndPath(process.out)

process.schedule = cms.Schedule(process.GenMuPath,process.L1TMuonPath)
process.schedule.extend([process.output_step])
