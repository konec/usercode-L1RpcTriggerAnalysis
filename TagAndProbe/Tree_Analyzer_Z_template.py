
import FWCore.ParameterSet.Config as cms
process = cms.Process("TagProbe")
process.load('FWCore.MessageService.MessageLogger_cfi')
process.source = cms.Source("EmptySource")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1) )

# bins definition
ptBins = cms.vdouble(20., 25., 30., 35., 40., 45., 50., 60., 70., 80., 100.)
etaBins = cms.vdouble(-2.1,-1.9,-1.7,-1.61,-1.545,-1.48,-1.42,-1.36,-1.3,-1.24,-1.19,-1.14,-1.04,-0.985,-0.93,-0.88,-0.83,-0.775,-0.72,-0.65,-0.58,-0.51,-0.44,-0.355,-0.27,-0.17,-0.07,\
                                       0.0,0.07,0.17,0.27,0.355,0.44,0.51,0.58,0.65,0.72,0.775,0.83,0.88,0.93,0.985,1.04,1.09,1.14,1.19,1.24,1.3,1.36,1.42,1.48, 1.545,1.61,1.7,1.9,2.1)            
etaBins1 = cms.vdouble(-2.1,-1.9,-1.7,-1.61,-1.545,-1.48,-1.42,-1.36,-1.3,-1.24,-1.19,-1.14,-1.04,-0.985,-0.93,-0.88,-0.83,-0.775,-0.72,-0.65,-0.58,-0.51,-0.44,-0.355,-0.27,-0.17,-0.07,\
                                       0.0,0.07,0.17,0.27,0.355,0.44,0.51,0.58,0.65,0.72,0.775,0.83,0.88,0.93,0.985,1.04,1.09,1.14,1.19,1.24,1.3,1.36,1.42,1.48, 1.545,1.61,1.7,1.9,2.1)
etaBins2 = cms.vdouble(-2.1,-2.0,-1.9,-1.8,-1.7,-1.6,-1.5,-1.4,-1.3,-1.2,-1.1,-1.0,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1,\
                                       0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.1)                       
etaBinsCSC = cms.vdouble(0.9,1.1,1.3,1.5,1.7,1.9,2.1)    
process.TagProbeFitTreeAnalyzer = cms.EDAnalyzer("TagProbeFitTreeAnalyzer",
    InputFileNames = cms.vstring("filename.root"),
    InputDirectoryName = cms.string("tnpTreeZ"),
    InputTreeName = cms.string("fitter_tree"),
    OutputFileName = cms.string("filename.root"),
    #numbrer of CPUs to use for fitting
    NumCPU = cms.uint32(4),
    # specifies wether to save the RooWorkspace containing the data for each bin and
    # the pdf object with the initial and final state snapshots
    SaveWorkspace = cms.bool(True),
    # defines all the real variables of the probes available in the input tree and intended for use in the efficiencies
    Variables = cms.PSet(
         mass = cms.vstring("Tag-Probe Mass", "70", "110", "GeV/c^{2}"),
        pt = cms.vstring("Probe p_{T}", "0", "100", "GeV/c"),
        eta = cms.vstring("Probe #eta", "-0.6", "0.6", ""),
        abseta = cms.vstring("Probe |#eta|", "0", "2.1", ""),
        phi = cms.vstring("Probe #phi", "-3.2", "3.2", ""),
        pair_dphiVtxTimesQ = cms.vstring("d#phi*Q", "-0.6", "0.6", ""),
        pair_drM2 = cms.vstring("Pair dR", "0", "100.", ""),
    ),
    # defines all the discrete variables of the probes available in the input tree and intended for use in the efficiency calculations
    Categories = cms.PSet(
        hasL1RPCMatch = cms.vstring("hasL1RPCMatch","dummy[true=1,false=0]"),
        hasL1RPCptValue5 = cms.vstring("hasL1RPCptValue5","dummy[true=1,false=0]"),
        hasL1RPCptValue10 = cms.vstring("hasL1RPCptValue10","dummy[true=1,false=0]"),
        hasL1RPCptValue15 = cms.vstring("hasL1RPCptValue15","dummy[true=1,false=0]"),
        hasL1RPCptValue16 = cms.vstring("hasL1RPCptValue16","dummy[true=1,false=0]"),
        hasL1RPCptValue20 = cms.vstring("hasL1RPCptValue20","dummy[true=1,false=0]"),
        hasL1RPCptValue30 = cms.vstring("hasL1RPCptValue30","dummy[true=1,false=0]"),
        hasL1DTMatch = cms.vstring("hasL1DTMatch","dummy[true=1,false=0]"),
        hasL1DTptValue5 = cms.vstring("hasL1DTptValue5","dummy[true=1,false=0]"),
        hasL1DTptValue10 = cms.vstring("hasL1DTptValue10","dummy[true=1,false=0]"),
        hasL1DTptValue15 = cms.vstring("hasL1DTptValue15","dummy[true=1,false=0]"),
        hasL1DTptValue16 = cms.vstring("hasL1DTptValue16","dummy[true=1,false=0]"),
        hasL1DTptValue20 = cms.vstring("hasL1DTptValue20","dummy[true=1,false=0]"),
        hasL1DTptValue30 = cms.vstring("hasL1DTptValue30","dummy[true=1,false=0]"),
        hasL1CSCMatch = cms.vstring("hasL1CSCMatch","dummy[true=1,false=0]"),
        hasL1CSCptValue5 = cms.vstring("hasL1CSCptValue5","dummy[true=1,false=0]"),
        hasL1CSCptValue10 = cms.vstring("hasL1CSCptValue10","dummy[true=1,false=0]"),
        hasL1CSCptValue15 = cms.vstring("hasL1CSCptValue15","dummy[true=1,false=0]"),
        hasL1CSCptValue16 = cms.vstring("hasL1CSCptValue16","dummy[true=1,false=0]"),
        hasL1CSCptValue20 = cms.vstring("hasL1CSCptValue20","dummy[true=1,false=0]"),
        hasL1CSCptValue30 = cms.vstring("hasL1CSCptValue30","dummy[true=1,false=0]"),
        
    ),
    # defines all the PDFs that will be available for the efficiency calculations; uses RooFit's "factory" syntax;
    # each pdf needs to define "signal", "backgroundPass","backgroundFail" pdfs, "efficiency[0.9,0,1]" 
    # and "signalFractionInPassing[0.9]" are used for initial values  
    PDFs = cms.PSet(
      gaussPlusExpo2 = cms.vstring(
            "Voigtian::signal(mass, mean[90,80,100], width[2.495], sigma[3,1,20])",
            "RooExponential::backgroundPass(mass, cPass[-0.02,-5,0])",
            "RooExponential::backgroundFail(mass, cFail[-0.02,-5,0])",
            "efficiency[0.9,0,1]",
            "signalFractionInPassing[0.9]"
        ),
      voigtianPlusLinear = cms.vstring(
            "Voigtian::signal(mass, mean[90,80,100], width[2.495], sigma[3,1,20])",
            "Chebychev::backgroundPass(mass, {cPass1[0,-1,1], cPass2[0,-1,1]})",
            "RooExponential::backgroundFail(mass, cFail[-0.02,-5,0])",
            "efficiency[0.9,0,1]",
            "signalFractionInPassing[0.9]"
        ),
      gaussPlusExpo = cms.vstring(
            "Voigtian::signal(mass, mean[90,80,100], width[2.495], sigma[3,1,20])",
            "Exponential::backgroundPass(mass, lp[0,-5,5])",
            "Exponential::backgroundFail(mass, lf[0,-5,5])",
            "efficiency[0.9,0,1]",
            "signalFractionInPassing[0.9]"
        ),
      gaussPlusExponential = cms.vstring(
            "Gaussian::signal(mass, mean[92,90,100], sigma[5,2,15])",
            "RooExponential::backgroundPass(mass, cPass[-0.02,-5,0])",
            "RooExponential::backgroundFail(mass, cFail[-0.02,-5,0])",
            "efficiency[0.9,0,1]",
            "signalFractionInPassing[0.9]"
       ),
      gaussPlusLinear = cms.vstring(
            "Gaussian::signal(mass, mean[92,90,100], sigma[5,2,15])",
            "Chebychev::backgroundPass(mass, cPass[0,-1,1])",
            "Chebychev::backgroundFail(mass, cFail[0,-1,1])",
            "efficiency[0.9,0,1]",
            "signalFractionInPassing[0.9]"
        ),
        gaussPlusQuadratic = cms.vstring(
            "Gaussian::signal(mass, mean[92,90,100], sigma[5,2,15])",
            "Chebychev::backgroundPass(mass, {cPass1[0,-1,1], cPass2[0,-1,1]})",
            "Chebychev::backgroundFail(mass, {cFail1[0,-1,1], cFail2[0,-1,1]})",
            "efficiency[0.9,0,1]",
            "signalFractionInPassing[0.9]"
        ),
        voigtPlusExpo = cms.vstring(
            "Voigtian::signal(mass, mean[90,80,100], width[2.495], sigma[3,1,20])",
            "Exponential::backgroundPass(mass, lp[0,-5,5])",
            "Exponential::backgroundFail(mass, lf[0,-5,5])",
            "efficiency[0.9,0,1]",
            "signalFractionInPassing[0.9]"
        ),
        vpvPlusExpo = cms.vstring(
            "Voigtian::signal1(mass, mean1[90,80,100], width[2.495], sigma1[2,1,3])",
            "Voigtian::signal2(mass, mean2[90,80,100], width,        sigma2[4,2,10])",
            "SUM::signal(vFrac[0.8,0,1]*signal1, signal2)",
            "Exponential::backgroundPass(mass, lp[-0.1,-1,0.1])",
            "Exponential::backgroundFail(mass, lf[-0.1,-1,0.1])",
            "efficiency[0.9,0,1]",
            "signalFractionInPassing[0.9]"
        )
    ),                                            
    Efficiencies = cms.PSet(
        Eff1 = cms.PSet(
            EfficiencyCategoryAndState = cms.vstring("flag1","true"),
            UnbinnedVariables = cms.vstring("mass"),
            BinnedVariables = cms.PSet(
                pt = ptBins,
                abseta = cms.vdouble(0.0, 1.6),                
            ),
            BinToPDFmap = cms.vstring("gaussPlusLinear", "*pt_bin0*", "gaussPlusQuadratic")
        ),
    )
)
process.fitness = cms.Path(process.TagProbeFitTreeAnalyzer)

