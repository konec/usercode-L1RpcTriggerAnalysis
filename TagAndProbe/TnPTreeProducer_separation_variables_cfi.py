import FWCore.ParameterSet.Config as cms

andStr = " && "
muonMatchStr = "hasUserCand('muToTkMatch') && !userCand('muToTkMatch').isNull() && abs(eta)<2.1"
isGlobalMuonStr = "userCand('muToTkMatch').isGlobalMuon()"
isTrackerMuonStr = "userCand('muToTkMatch').isTrackerMuon()"
isVBTFMuonStr = "userCand('muToTkMatch').globalTrack().normalizedChi2<10"\
            +" && userCand('muToTkMatch').globalTrack().hitPattern().numberOfValidTrackerHits>10"\
            +" && userCand('muToTkMatch').globalTrack().hitPattern().numberOfValidMuonHits>0"\
            +" && userCand('muToTkMatch').globalTrack().hitPattern().numberOfValidPixelHits>0"\
            +" && userCand('muToTkMatch').numberOfMatches>1"\

passIsolationStr = "(userCand('muToTkMatch').chargedHadronIso()+ "\
                   +"userCand('muToTkMatch').neutralHadronIso()+ "\
                   +"userCand('muToTkMatch').photonIso())<0.1*userCand('muToTkMatch').pt"

isHLTMuonStr = "!userCand('muToTkMatch').triggerObjectMatchesByFilter('hltSingleMu9L3Filtered9').empty()"


isL1Match = "hasUserCand('l1MuTkMatch') && !userCand('l1MuTkMatch').isNull()"
isRPCquality2="userCand('l1MuTkMatch').regRPCMuonCand.quality>=2"
isRPCMatch = "(userCand('l1MuTkMatch').gmtMuonCand().quality()==5 || userCand('l1MuTkMatch').gmtMuonCand().quality()==7)"
isRPCOnlyMatch = "userCand('l1MuTkMatch').gmtMuonCand().quality()==5"
isRPCnotEmpty = "!userCand('l1MuTkMatch').regRPCMuonCand.empty()"
isRPCptValue5 = "userCand('l1MuTkMatch').regRPCMuonCand.ptValue>=5"
isRPCptValue10 = "userCand('l1MuTkMatch').regRPCMuonCand.ptValue>=10"
isRPCptValue15 = "userCand('l1MuTkMatch').regRPCMuonCand.ptValue>=15"
isRPCptValue16 = "userCand('l1MuTkMatch').regRPCMuonCand.ptValue>=16"
isRPCptValue20 = "userCand('l1MuTkMatch').regRPCMuonCand.ptValue>=20"
isRPCptValue30 = "userCand('l1MuTkMatch').regRPCMuonCand.ptValue>=30"

isDTnotEmpty = "!userCand('l1MuTkMatch').regDTMuonCand.empty()"
isDTMatch = "((userCand('l1MuTkMatch').gmtMuonCand().quality()==6 || userCand('l1MuTkMatch').gmtMuonCand().quality()==7)&& (userCand('l1MuTkMatch').gmtMuonCand().detector()==2||userCand('l1MuTkMatch').gmtMuonCand().detector()==3))"
isDTOnlyMatch = "userCand('l1MuTkMatch').gmtMuonCand().quality()==6"
isDTptValue5 = "userCand('l1MuTkMatch').regDTMuonCand.ptValue>=5"
isDTptValue10 = "userCand('l1MuTkMatch').regDTMuonCand.ptValue>=10"
isDTptValue15 = "userCand('l1MuTkMatch').regDTMuonCand.ptValue>=15"
isDTptValue16 = "userCand('l1MuTkMatch').regDTMuonCand.ptValue>=16"
isDTptValue20 = "userCand('l1MuTkMatch').regDTMuonCand.ptValue>=20"
isDTptValue30 = "userCand('l1MuTkMatch').regDTMuonCand.ptValue>=30"

isCSCnotEmpty = "!userCand('l1MuTkMatch').regCSCMuonCand.empty()"
isCSCMatch = "((userCand('l1MuTkMatch').gmtMuonCand().quality()==6 || userCand('l1MuTkMatch').gmtMuonCand().quality()==7) && (userCand('l1MuTkMatch').gmtMuonCand().detector()==4||userCand('l1MuTkMatch').gmtMuonCand().detector()==5))"
isCSCOnlyMatch = "userCand('l1MuTkMatch').gmtMuonCand().quality()==6"
isCSCptValue5 = "userCand('l1MuTkMatch').regCSCMuonCand.ptValue>=5"
isCSCptValue10 = "userCand('l1MuTkMatch').regCSCMuonCand.ptValue>=10"
isCSCptValue15 = "userCand('l1MuTkMatch').regCSCMuonCand.ptValue>=15"
isCSCptValue16 = "userCand('l1MuTkMatch').regCSCMuonCand.ptValue>=16"
isCSCptValue20 = "userCand('l1MuTkMatch').regCSCMuonCand.ptValue>=20"
isCSCptValue30 = "userCand('l1MuTkMatch').regCSCMuonCand.ptValue>=30"

tnpTreeJPsiProducer = cms.EDAnalyzer("TagProbeFitTreeProducer",
    # choice of tag and probe pairs, and arbitration
    tagProbePairs = cms.InputTag("tagProbesJPsi"),
    arbitration   = cms.string("OneProbe"), ## that is, use only tags associated to a single probe.
    #massForArbitration =  cms.double(92.5),
     # probe variables
    variables = cms.PSet(
        pt     = cms.string("pt"),
        eta = cms.string("eta"),
        abseta = cms.string("abs(eta)"),
        phi = cms.string("phi")
    ),
    # choice of what defines a 'passing' probe
    flags = cms.PSet(
          hasMuonMatch = cms.string(muonMatchStr),
          isGlobalMuon = cms.string(muonMatchStr+andStr+isGlobalMuonStr),
          isTrackerMuon = cms.string(muonMatchStr+andStr+isGlobalMuonStr+andStr+isTrackerMuonStr),
          passVBTF = cms.string(muonMatchStr+andStr+isGlobalMuonStr+andStr+isTrackerMuonStr+andStr+isVBTFMuonStr),
          passIsolation = cms.string(muonMatchStr+andStr+isGlobalMuonStr+andStr+isTrackerMuonStr+andStr+isVBTFMuonStr+andStr+passIsolationStr),
          passHLT_Mu9 = cms.string(muonMatchStr+andStr+isGlobalMuonStr+andStr+isTrackerMuonStr+andStr+isVBTFMuonStr+andStr+passIsolationStr+andStr+isHLTMuonStr),
           hasL1Match = cms.string(isL1Match),
          hasL1RPCMatch = cms.string(isL1Match+andStr+isRPCnotEmpty+andStr+isRPCMatch),
          hasL1RPCOnlyMatch = cms.string(isL1Match+andStr+isRPCnotEmpty+andStr+isRPCOnlyMatch),
          hasL1RPCptValue5 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue5),
          hasL1RPCptValue10 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue10),
          hasL1RPCptValue15 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue15),
          hasL1RPCptValue20 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue20),
          hasL1RPCptValue30 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue30),

          hasL1DTMatch = cms.string(isL1Match+andStr+isDTnotEmpty+andStr+isDTMatch),
          hasL1DTOnlyMatch = cms.string(isL1Match+andStr+isDTnotEmpty+andStr+isDTOnlyMatch),
          hasL1DTptValue5 = cms.string(isL1Match+andStr+isDTMatch+andStr+isDTnotEmpty+andStr+isDTptValue5),
          hasL1DTptValue10 = cms.string(isL1Match+andStr+isDTMatch+andStr+isDTnotEmpty+andStr+isDTptValue10),
          hasL1DTptValue15 = cms.string(isL1Match+andStr+isDTMatch+andStr+isDTnotEmpty+andStr+isDTptValue15),
          hasL1DTptValue20 = cms.string(isL1Match+andStr+isDTMatch+andStr+isDTnotEmpty+andStr+isDTptValue20),
          hasL1DTptValue30 = cms.string(isL1Match+andStr+isDTMatch+andStr+isDTnotEmpty+andStr+isDTptValue30),
          
          hasL1CSCMatch = cms.string(isL1Match+andStr+isCSCnotEmpty+andStr+isCSCMatch),
          hasL1CSCOnlyMatch = cms.string(isL1Match+andStr+isCSCnotEmpty+andStr+isCSCOnlyMatch),
          hasL1CSCptValue5 = cms.string(isL1Match+andStr+isCSCMatch+andStr+isCSCnotEmpty+andStr+isCSCptValue5),
          hasL1CSCptValue10 = cms.string(isL1Match+andStr+isCSCMatch+andStr+isCSCnotEmpty+andStr+isCSCptValue10),
          hasL1CSCptValue15 = cms.string(isL1Match+andStr+isCSCMatch+andStr+isCSCnotEmpty+andStr+isCSCptValue15),
          hasL1CSCptValue20 = cms.string(isL1Match+andStr+isCSCMatch+andStr+isCSCnotEmpty+andStr+isCSCptValue20),
          hasL1CSCptValue30 = cms.string(isL1Match+andStr+isCSCMatch+andStr+isCSCnotEmpty+andStr+isCSCptValue30),         
          
#          hasL1RPCMatchQ2 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCquality2),
#          hasL1RPCptValue5Q2 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue5+andStr+isRPCquality2),
#          hasL1RPCptValue10Q2 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue10+andStr+isRPCquality2),
#          hasL1RPCptValue15Q2 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue15+andStr+isRPCquality2),
#          hasL1RPCptValue20Q2 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue20+andStr+isRPCquality2),
#          hasL1RPCptValue25Q2 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue25+andStr+isRPCquality2),
#          hasL1RPCptValue30Q2 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue30+andStr+isRPCquality2),
#          hasL1RPCptValue35Q2 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue35+andStr+isRPCquality2),
#          hasL1RPCptValue40Q2 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue40+andStr+isRPCquality2),
#          hasL1RPCptValue50Q2 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue50+andStr+isRPCquality2),
#          hasL1RPCptValue60Q2 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue60+andStr+isRPCquality2),
#          hasL1RPCptValue100Q2 = cms.string(isL1Match+andStr+isRPCMatch+andStr+isRPCnotEmpty+andStr+isRPCptValue60+andStr+isRPCquality2)
    ),
    ## DATA-related info
    addRunLumiInfo = cms.bool(True),
    ## MC-related info
    isMC = cms.bool(False), ## on MC you can set this to true, add some parameters and get extra info in the tree.
    tagVariables = cms.PSet(
       pt  = cms.string('pt'),
       eta = cms.string('eta'),
       phi = cms.string('phi'),
     ),
    tagFlags = cms.PSet(),
    pairVariables = cms.PSet(
        pt = cms.string("pt"),
        dphiVtxTimesQ = cms.InputTag("tagProbeSeparation", "dphiVtxTimesQ"),
        drM1          = cms.InputTag("tagProbeSeparation", "drM1"),
        dphiM1        = cms.InputTag("tagProbeSeparation", "dphiM1"),
        distM1        = cms.InputTag("tagProbeSeparation", "distM1"),
        drM2          = cms.InputTag("tagProbeSeparation", "drM2"),
        dphiM2        = cms.InputTag("tagProbeSeparation", "dphiM2"),
        distM2        = cms.InputTag("tagProbeSeparation", "distM2"),
        drVtx         = cms.InputTag("tagProbeSeparation", "drVtx"),
        dz            = cms.string("daughter(0).vz - daughter(1).vz") 
    ),
    pairFlags = cms.PSet(),
)
