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
    process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(10000)
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(True))

process.source = cms.Source(
    'PoolSource',
    fileNames = cms.untracked.vstring('file:/home/akalinow/scratch/CMS/OverlapTrackFinder/Crab/SingleMuFullEtaTestSample/720_FullEta_v1/data/SingleMu_16_p_1_2_TWz.root')
    )

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000))


###PostLS1 geometry used
process.load('Configuration.Geometry.GeometryExtended2015Reco_cff')
process.load('Configuration.Geometry.GeometryExtended2015_cff')
############################
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

###OMTF emulator configuration
####Event Setup Producer
process.load('L1Trigger.L1TMuonOverlap.fakeOmtfParams_cff')
process.esProd = cms.EDAnalyzer("EventSetupRecordDataGetter",
   toGet = cms.VPSet(
      cms.PSet(record = cms.string('L1TMuonOverlapParamsRcd'),
               data = cms.vstring('L1TMuonOverlapParams'))
                   ),
   verbose = cms.untracked.bool(True)
)

process.load('L1Trigger.L1TMuonOverlap.simMuonOverlapDigis_cfi')
#process.simOmtfDigis.srcCSC = cms.InputTag("simCscTriggerPrimitiveDigis","MPCSORTED")
#process.simOmtfDigis.srcDTPh = cms.InputTag("dttfDigis")
#process.simOmtfDigis.srcDTTh = cms.InputTag("dttfDigis")
#process.simOmtfDigis.srcRPC = cms.InputTag("muonRPCDigis")

#process.simOmtfDigis.omtf.patternsXMLFiles = cms.VPSet(
#                                       cms.PSet(patternsXMLFile = cms.FileInPath("L1Trigger/L1OverlapMuonTrackFinder/data/Patterns_ipt4_31_750_4x.xml")),
#                                      )
#process.simOmtfDigis.omtf.configFromXML = cms.bool(True)

###OMTF analyser configuration
process.omtfAnalyser = cms.EDAnalyzer("OMTFAnalyzer",
                                      OMTFCandSrc = cms.InputTag('simOmtfDigis','OMTF'),
                                      GMTCandSrc = cms.InputTag('simGmtDigis'),
                                      g4SimTrackSrc = cms.InputTag('g4SimHits'),
                                      anaEff      = cms.PSet( maxDR= cms.double(9999.)),
                                      multiplyEvents = cms.uint32(1)
                                      )
#process.omtfAnalyser.GMTCandSrc = cms.InputTag('gtDigis')


###Gen level filter configuration
process.MuonEtaFilter = cms.EDFilter("SimTrackEtaFilter",
                                minNumber = cms.uint32(1),
                                src = cms.InputTag("g4SimHits"),
                                #cut = cms.string("momentum.eta<1.24 && momentum.eta>0.83 &&  momentum.pt>1")
                                #cut = cms.string("momentum.eta<0.85 && momentum.eta>0.83 &&  momentum.pt>1")                                
                                cut = cms.string("momentum.pt>1")
                                )

process.GenMuPath = cms.Path(process.MuonEtaFilter)
process.GenMuPath = cms.Path()
##########################################

process.L1TMuonSeq = cms.Sequence(process.esProd + process.simOmtfDigis + process.omtfAnalyser)

process.L1TMuonPath = cms.Path(process.MuonEtaFilter*process.L1TMuonSeq)
#process.L1TMuonPath = cms.Path(process.L1TMuonSeq)

process.schedule = cms.Schedule(process.GenMuPath,process.L1TMuonPath)

