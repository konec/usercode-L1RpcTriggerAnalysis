//
// $Id: NearbyMuonsInfo.cc,v 1.2 2012/03/07 10:47:37 rybinska Exp $
//

/**
  \class    pat::NearbyMuonsInfo NearbyMuonsInfo.h "PhysicsTools/PatAlgos/interface/NearbyMuonsInfo.h"
  \brief    Matcher of reconstructed objects to L1 Muons 
            
  \author   Giovanni Petrucciani
  \version  $Id: NearbyMuonsInfo.cc,v 1.2 2012/03/07 10:47:37 rybinska Exp $

  RPC Trigger efficiency analysis
  Author: Katarzyna Romanowska-Rybinska
  Version 1.0 , Date: 7 March 2012
  Application: Distributions of Tag & Probe separation variables, production of TTree used for analysis debugging
*/


#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"
#include <DataFormats/Candidate/interface/CompositeCandidate.h>
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include <DataFormats/MuonReco/interface/Muon.h>
#include <DataFormats/TrackReco/interface/Track.h>
#include <DataFormats/Math/interface/deltaR.h>
#include "DataFormats/Math/interface/deltaPhi.h"
#include <MuonAnalysis/MuonAssociators/interface/PropagateToMuon.h>
#include "DataFormats/PatCandidates/interface/GenericParticle.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/Common/interface/Ptr.h"
#include <functional>
#include "DataFormats/Common/interface/TriggerResults.h"
#include "HLTrigger/HLTfilters/interface/HLTHighLevel.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"

//// Includes for DCA: /////
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutRecord.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"
#include "DataFormats/PatCandidates/interface/PATObject.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TFrame.h>
#include "TTree.h"


using namespace edm;
using namespace std;
//using namespace l1extra;

class NearbyMuonsInfo : public edm::EDProducer {
    public:
      explicit NearbyMuonsInfo(const edm::ParameterSet & iConfig);
      virtual ~NearbyMuonsInfo();

      virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);
      virtual void beginRun(edm::Run & iRun, const edm::EventSetup & iSetup);
      virtual void beginJob();
      virtual void endJob();
      virtual void endRun(edm::Run & iRun, const edm::EventSetup & iSetup);
      int MatchHLT( const pat::Muon & Muon, std::vector<reco::Particle> HLT,double dRmax,double dPtRelmax,double &dR, double &dPhi, double &dEta, double &dPtRel);
    private:
        edm::InputTag src_, muonsrc_, triggerResults_, trigEvent_;
        std::string sortBy_;
        double dRmax_, dPtRelmax_;
        PropagateToMuon prop1_, prop2_;

        /// Write a ValueMap<float> in the event
        void writeValueMap(edm::Event &iEvent,
                const edm::Handle<edm::View<reco::Candidate> > & handle,
                const std::vector<float> & values,
                const std::string    & label) const ;

        TFile* hFile;
        TTree *Tree;
        std::string filename_;

        std::vector< std::pair <std::string,std::string> > Paths;

        struct comparatorStruct {
          bool operator() (std::pair<int, float> i, std::pair<int, float> j) {return (i.second < j.second);}
        } comparator;

///////Event
        double run, lumi, event;

/////// Tag
        double TagCharge;
////////// Tag polar coordinates at vertex
        double Tag_pt, Tag_phi, Tag_eta;
        double TagTk_pt, TagTk_phi, TagTk_eta;
        double TagSA_pt, TagSA_phi, TagSA_eta;
////////// TSOS polar coordinates
        double Tag_eta_propM1, Tag_phi_propM1;
        double Tag_eta_propM2, Tag_phi_propM2;
////////// TSOS cartesian coordinates
        double tag_M1_glob_x, tag_M1_glob_y, tag_M1_glob_z;
        double tag_M2_glob_x, tag_M2_glob_y, tag_M2_glob_z;
//////  Tag's HLT match
////////// Polar coordinates
        double Tag_HLT_pt, Tag_HLT_phi, Tag_HLT_eta;
////////// Tag-HLT Match parameters
        double dR_Tag_HLT, dPhi_Tag_HLT, dEta_Tag_HLT, dPtRel_Tag_HLT;
        //////////Tag-PATHLT match
        double Tag_PATHLT_pt, Tag_PATHLT_phi, Tag_PATHLT_eta;
        double dR_Tag_PATHLT, dPhi_Tag_PATHLT, dEta_Tag_PATHLT, dPtRel_Tag_PATHLT;

/////// Probe
        double ProbeCharge;
////////// Tag polar coordinates at vertex
    	double Probe_pt, Probe_phi, Probe_eta;
        double ProbeMu_pt, ProbeMu_phi, ProbeMu_eta;
        double ProbeSA_pt, ProbeSA_phi, ProbeSA_eta;
////////// TSOS polar coordinates
		double Probe_eta_propM1, Probe_phi_propM1;
		double Probe_eta_propM2, Probe_phi_propM2;
////////// TSOS cartesian coordinates
		double probe_M1_glob_x, probe_M1_glob_y, probe_M1_glob_z;
		double probe_M2_glob_x, probe_M2_glob_y, probe_M2_glob_z;
////////// L1 Match polar coordinates
		double L1E_pt, L1E_eta, L1E_phi;
		double RPC_pt, RPC_eta, RPC_phi;
		double DT_pt, DT_eta, DT_phi;
		double CSC_pt, CSC_eta, CSC_phi;
////////// L1 Match subsystem flags
		double L1quality, DT_fine;
		bool isDT, isRPC,isCSC;
////////// L1Extra-Regional distance
		double dR_L1_RPC, dR_L1_DT, dR_L1_CSC;
		double dEta_L1_RPC, dEta_L1_DT, dEta_L1_CSC;
		double dPhi_L1_RPC, dPhi_L1_DT, dPhi_L1_CSC;
////////// Track-L1 Match parameters
		double dR_MuL1, dPhi_MuL1, dEta_MuL1;
//////  Probe's HLT match
////////// Polar coordinates
		double Probe_HLT_pt, Probe_HLT_phi, Probe_HLT_eta;
////////// Probe-HLT Match parameters
		double dR_Probe_HLT, dPhi_Probe_HLT, dEta_Probe_HLT, dPtRel_Probe_HLT;
////////// Probe-PATHLT Match parameters
		double Probe_PATHLT_pt, Probe_PATHLT_phi, Probe_PATHLT_eta;
		double dR_Probe_PATHLT, dPhi_Probe_PATHLT, dEta_Probe_PATHLT, dPtRel_Probe_PATHLT;


/////// Pair separation variables
		double dphiVtxQ, dr_Vtx;
		double dphi_M1, dr_M1, dist_M1;
		double dphi_M2, dr_M2, dist_M2;
};

NearbyMuonsInfo::NearbyMuonsInfo(const edm::ParameterSet & iConfig) :
    src_(iConfig.getParameter<edm::InputTag>("src")),
    muonsrc_(iConfig.getParameter<edm::InputTag>("muonsrc")),
    triggerResults_(iConfig.getParameter<edm::InputTag>("triggerResults")),
    trigEvent_(iConfig.getParameter<edm::InputTag> ("triggerEvent")),
    sortBy_(iConfig.getParameter<std::string >("sortBy")),
    dRmax_(iConfig.getParameter<double>("dRmax")),
    dPtRelmax_(iConfig.getParameter<double>("dPtRelmax")),
    prop1_(iConfig.getParameter<edm::ParameterSet>("propM1")),
    prop2_(iConfig.getParameter<edm::ParameterSet>("propM2")),
    filename_(iConfig.getParameter<std::string>("filename"))

{
    produces<edm::ValueMap<float> >("DCA");
    produces<edm::ValueMap<float> >("dphiVtxTimesQ");
    produces<edm::ValueMap<float> >("drVtx");
    produces<edm::ValueMap<float> >("drM1");
    produces<edm::ValueMap<float> >("dphiM1");
    produces<edm::ValueMap<float> >("distM1");
    produces<edm::ValueMap<float> >("drM2");
    produces<edm::ValueMap<float> >("dphiM2");
    produces<edm::ValueMap<float> >("distM2");
    produces<edm::ValueMap<float> >("drStaIn");
    produces<edm::ValueMap<float> >("dphiStaIn");

    Paths.push_back(make_pair("HLT_IsoMu15_eta2p1","hltSingleMuIsoL1s14L3IsoFiltered15eta2p1"));
    Paths.push_back(make_pair("HLT_IsoMu15","hltSingleMuIsoL3IsoFiltered15"));
    Paths.push_back(make_pair("HLT_IsoMu20","hltSingleMuL2QualIsoL3IsoFiltered20"));
    Paths.push_back(make_pair("HLT_IsoMu24_eta2p1","hltL3IsoL1sMu14Eta2p1L1f0L2f14QL2IsoL3f24L3IsoFiltered"));
    Paths.push_back(make_pair("HLT_IsoMu24","hltSingleMuL2QualIsoL3IsoFiltered24"));
    Paths.push_back(make_pair("HLT_IsoMu30_eta2p1","hltL3IsoL1sMu14Eta2p1L1f0L2f14QL2IsoL3f30L3IsoFiltered"));
    Paths.push_back(make_pair("HLT_IsoMu34_eta2p1","hltL3IsoL1sMu16Eta2p1L1f0L2f16QL2IsoL3f34L3IsoFiltered"));
    Paths.push_back(make_pair("HLT_IsoMu40_eta2p1","hltL3IsoL1sMu16Eta2p1L1f0L2f16QL2IsoL3f40L3IsoFiltered"));
    Paths.push_back(make_pair("HLT_Mu100_eta2p1","hltL3fL1sMu16Eta2p1L1f0L2f16QL3Filtered100"));
    Paths.push_back(make_pair("HLT_Mu12","hltSingleMu12L3Filtered12"));
    Paths.push_back(make_pair("HLT_Mu15","hltSingleMu15L3Filtered15"));
    Paths.push_back(make_pair("HLT_Mu200_eta2p1","hltL3fL1sMu16Eta2p1L1f0L2f16QL3Filtered200"));
    Paths.push_back(make_pair("HLT_Mu20","hltSingleMu20L3Filtered20"));
    Paths.push_back(make_pair("HLT_Mu24","hltSingleMu24L2QualL3Filtered24"));
    Paths.push_back(make_pair("HLT_Mu30","hltSingleMu30L2QualL3Filtered30"));
    Paths.push_back(make_pair("HLT_Mu40_eta2p1","hltL3fL1sMu16Eta2p1L1f0L2f16QL3Filtered40"));
    Paths.push_back(make_pair("HLT_Mu40","hltSingleMu40L2QualL3Filtered40"));
    Paths.push_back(make_pair("HLT_Mu50_eta2p1","hltL3fL1sMu16Eta2p1L1f0L2f16QL3Filtered50"));
    Paths.push_back(make_pair("HLT_Mu5","hltSingleMu5L3Filtered5"));
    Paths.push_back(make_pair("HLT_Mu60_eta2p1","hltL3fL1sMu16Eta2p1L1f0L2f16QL3Filtered60"));
    Paths.push_back(make_pair("HLT_Mu8","hltSingleMu8L3Filtered8"));
}

NearbyMuonsInfo::~NearbyMuonsInfo(){
  if (hFile!=0) {
    hFile->Close();
    delete hFile;
  }
}

void NearbyMuonsInfo::endRun(edm::Run & iRun, const edm::EventSetup & iSetup) {

}
void NearbyMuonsInfo::endJob() {

    hFile->cd();

    hFile->Write();
}
void 
NearbyMuonsInfo::beginRun(edm::Run & iRun, const edm::EventSetup & iSetup) {
    prop1_.init(iSetup);
    prop2_.init(iSetup);
}

void
NearbyMuonsInfo::beginJob(){
    hFile = new TFile(filename_.c_str(),"recreate");
    hFile->cd();
    Tree = new TTree("Tree","Tree");
///////Event
    Tree->Branch("run",&run,"run/D");
    Tree->Branch("lumi",&lumi,"lumi/D");
    Tree->Branch("event",&event,"event/D");

/////// Tag
    Tree->Branch("TagCharge",&TagCharge,"TagCharge/D");
////////// Tag polar coordinates at vertex
    Tree->Branch("Tag_pt",&Tag_pt,"Tag_pt/D");
    Tree->Branch("Tag_phi",&Tag_phi,"Tag_phi/D");
    Tree->Branch("Tag_eta",&Tag_eta,"Tag_eta/D");
    Tree->Branch("TagTk_pt",&TagTk_pt,"TagTk_pt/D");
    Tree->Branch("TagTk_phi",&TagTk_phi,"TagTk_phi/D");
    Tree->Branch("TagTk_eta",&TagTk_eta,"TagTk_eta/D");
    Tree->Branch("TagSA_pt",&TagSA_pt,"TagSA_pt/D");
    Tree->Branch("TagSA_phi",&TagSA_phi,"TagSA_phi/D");
    Tree->Branch("TagSA_eta",&TagSA_eta,"TagSA_eta/D");
////////// TSOS polar coordinates
    Tree->Branch("Tag_eta_propM1",&Tag_eta_propM1,"Tag_eta_propM1/D");
    Tree->Branch("Tag_phi_propM1",&Tag_phi_propM1,"Tag_phi_propM1/D");
    Tree->Branch("Tag_eta_propM2",&Tag_eta_propM2,"Tag_eta_propM2/D");
    Tree->Branch("Tag_phi_propM2",&Tag_phi_propM2,"Tag_phi_propM2/D");
////////// TSOS cartesian coordinates
    Tree->Branch("tag_M1_glob_x",&tag_M1_glob_x,"tag_M1_glob_x/D");
    Tree->Branch("tag_M1_glob_y",&tag_M1_glob_y,"tag_M1_glob_y/D");
    Tree->Branch("tag_M1_glob_z",&tag_M1_glob_z,"tag_M1_glob_z/D");
    Tree->Branch("tag_M2_glob_x",&tag_M2_glob_x,"tag_M2_glob_x/D");
    Tree->Branch("tag_M2_glob_y",&tag_M2_glob_y,"tag_M2_glob_y/D");
    Tree->Branch("tag_M2_glob_z",&tag_M2_glob_z,"tag_M2_glob_z/D");
//////  Tag's HLT match
////////// Polar coordinates
	Tree->Branch("Tag_HLT_pt",&Tag_HLT_pt,"Tag_HLT_pt/D");
	Tree->Branch("Tag_HLT_phi",&Tag_HLT_phi,"Tag_HLT_phi/D");
	Tree->Branch("Tag_HLT_eta",&Tag_HLT_eta,"Tag_HLT_eta/D");
////////// Tag-HLT Match parameters
	Tree->Branch("dR_Tag_HLT",&dR_Tag_HLT,"dR_Tag_HLT/D");
	Tree->Branch("dPhi_Tag_HLT",&dPhi_Tag_HLT,"dPhi_Tag_HLT/D");
	Tree->Branch("dEta_Tag_HLT",&dEta_Tag_HLT,"dEta_Tag_HLT/D");
	Tree->Branch("dPtRel_Tag_HLT",&dPtRel_Tag_HLT,"dPtRel_Tag_HLT/D");

//////////Tag-PATHLT match
    Tree->Branch("Tag_PATHLT_pt",&Tag_PATHLT_pt,"Tag_PATHLT_pt/D");
    Tree->Branch("Tag_PATHLT_phi",&Tag_PATHLT_phi,"Tag_PATHLT_phi/D");
    Tree->Branch("Tag_PATHLT_eta",&Tag_PATHLT_eta,"Tag_PATHLT_eta/D");
    Tree->Branch("dR_Tag_PATHLT",&dR_Tag_PATHLT,"dR_Tag_PATHLT/D");
    Tree->Branch("dPhi_Tag_PATHLT",&dPhi_Tag_PATHLT,"dPhi_Tag_PATHLT/D");
    Tree->Branch("dEta_Tag_PATHLT",&dEta_Tag_PATHLT,"dEta_Tag_PATHLT/D");
    Tree->Branch("dPtRel_Tag_PATHLT",&dPtRel_Tag_PATHLT,"dPtRel_Tag_PATHLT/D");


/////// Probe
    Tree->Branch("ProbeCharge",&ProbeCharge,"ProbeCharge/D");
////////// Tag polar coordinates at vertex
    Tree->Branch("Probe_pt",&Probe_pt,"Probe_pt/D");
    Tree->Branch("Probe_phi",&Probe_phi,"Probe_phi/D");
    Tree->Branch("Probe_eta",&Probe_eta,"Probe_eta/D");
    Tree->Branch("ProbeMu_pt",&ProbeMu_pt,"ProbeMu_pt/D");
    Tree->Branch("ProbeMu_phi",&ProbeMu_phi,"ProbeMu_phi/D");
    Tree->Branch("ProbeMu_eta",&ProbeMu_eta,"ProbeMu_eta/D");
    Tree->Branch("ProbeSA_pt",&ProbeSA_pt,"ProbeSA_pt/D");
    Tree->Branch("ProbeSA_phi",&ProbeSA_phi,"ProbeSA_phi/D");
    Tree->Branch("ProbeSA_eta",&ProbeSA_eta,"ProbeSA_eta/D");
////////// TSOS polar coordinates
    Tree->Branch("Probe_eta_propM1",&Probe_eta_propM1,"Probe_eta_propM1/D");
    Tree->Branch("Probe_phi_propM1",&Probe_phi_propM1,"Probe_phi_propM1/D");
    Tree->Branch("Probe_eta_propM2",&Probe_eta_propM2,"Probe_eta_propM2/D");
    Tree->Branch("Probe_phi_propM2",&Probe_phi_propM2,"Probe_phi_propM2/D");
////////// TSOS cartesian coordinates
    Tree->Branch("probe_M1_glob_x",&probe_M1_glob_x,"probe_M1_glob_x/D");
    Tree->Branch("probe_M1_glob_y",&probe_M1_glob_y,"probe_M1_glob_y/D");
    Tree->Branch("probe_M1_glob_z",&probe_M1_glob_z,"probe_M1_glob_z/D");
    Tree->Branch("probe_M2_glob_x",&probe_M2_glob_x,"probe_M2_glob_x/D");
    Tree->Branch("probe_M2_glob_y",&probe_M2_glob_y,"probe_M2_glob_y/D");
    Tree->Branch("probe_M2_glob_z",&probe_M2_glob_z,"probe_M2_glob_z/D");
////////// L1 Match polar coordinates
    Tree->Branch("L1E_pt",&L1E_pt,"L1E_pt/D");
    Tree->Branch("L1E_eta",&L1E_eta,"L1E_eta/D");
    Tree->Branch("L1E_phi",&L1E_phi,"L1E_phi/D");
    Tree->Branch("RPC_pt",&RPC_pt,"RPC_pt/D");
    Tree->Branch("RPC_eta",&RPC_eta,"RPC_eta/D");
    Tree->Branch("RPC_phi",&RPC_phi,"RPC_phi/D");
    Tree->Branch("DT_pt",&DT_pt,"DT_pt/D");
    Tree->Branch("DT_eta",&DT_eta,"DT_eta/D");
    Tree->Branch("DT_phi",&DT_phi,"DT_phi/D");
    Tree->Branch("CSC_pt",&CSC_pt,"CSC_pt/D");
    Tree->Branch("CSC_eta",&CSC_eta,"CSC_eta/D");
    Tree->Branch("CSC_phi",&CSC_phi,"CSC_phi/D");
////////// L1 Match subsystem flags
    Tree->Branch("L1quality",&L1quality,"L1quality/D");
    Tree->Branch("isRPC",&isRPC,"isRPC/B");
    Tree->Branch("isDT",&isDT,"isDT/B");
    Tree->Branch("isCSC",&isCSC,"isCSC/B");
    Tree->Branch("DT_fine",&DT_fine,"DT_fine/B");
////////// L1Extra-Regional distance
    Tree->Branch("dR_L1_RPC",&dR_L1_RPC,"dR_L1_RPC/D");
    Tree->Branch("dR_L1_DT",&dR_L1_DT,"dR_L1_DT/D");
    Tree->Branch("dR_L1_CSC",&dR_L1_CSC,"dR_L1_CSC/D");
    Tree->Branch("dEta_L1_RPC",&dEta_L1_RPC,"dEta_L1_RPC/D");
    Tree->Branch("dEta_L1_DT",&dEta_L1_DT,"dEta_L1_DT/D");
    Tree->Branch("dEta_L1_CSC",&dEta_L1_CSC,"dEta_L1_CSC/D");
    Tree->Branch("dPhi_L1_RPC",&dPhi_L1_RPC,"dPhi_L1_RPC/D");
    Tree->Branch("dPhi_L1_DT",&dPhi_L1_DT,"dPhi_L1_DT/D");
    Tree->Branch("dPhi_L1_CSC",&dPhi_L1_CSC,"dPhi_L1_CSC/D");
////////// Track-L1 Match parameters
    Tree->Branch("dR_MuL1",&dR_MuL1,"dR_MuL1/D");
    Tree->Branch("dPhi_MuL1",&dPhi_MuL1,"dPhi_MuL1/D");
    Tree->Branch("dEta_MuL1",&dEta_MuL1,"dEta_MuL1/D");
//////  Probe's HLT match
////////// Polar coordinates
	Tree->Branch("Probe_HLT_pt",&Probe_HLT_pt,"Probe_HLT_pt/D");
	Tree->Branch("Probe_HLT_phi",&Probe_HLT_phi,"Probe_HLT_phi/D");
	Tree->Branch("Probe_HLT_eta",&Probe_HLT_eta,"Probe_HLT_eta/D");
////////// Probe-HLT Match parameters (HLT from AOD collection)
	Tree->Branch("dR_Probe_HLT",&dR_Probe_HLT,"dR_Probe_HLT/D");
	Tree->Branch("dPhi_Probe_HLT",&dPhi_Probe_HLT,"dPhi_Probe_HLT/D");
	Tree->Branch("dEta_Probe_HLT",&dEta_Probe_HLT,"dEta_Probe_HLT/D");
	Tree->Branch("dPtRel_Probe_HLT",&dPtRel_Probe_HLT,"dPtRel_Probe_HLT/D");
////////// Probe-PATHLT Match parameters (Matched)
	Tree->Branch("Probe_PATHLT_pt",&Probe_PATHLT_pt,"Probe_PATHLT_pt/D");
	Tree->Branch("Probe_PATHLT_phi",&Probe_PATHLT_phi,"Probe_PATHLT_phi/D");
	Tree->Branch("Probe_PATHLT_eta",&Probe_PATHLT_eta,"Probe_PATHLT_eta/D");
	Tree->Branch("dR_Probe_PATHLT",&dR_Probe_PATHLT,"dR_Probe_PATHLT/D");
	Tree->Branch("dPhi_Probe_PATHLT",&dPhi_Probe_PATHLT,"dPhi_Probe_PATHLT/D");
	Tree->Branch("dEta_Probe_PATHLT",&dEta_Probe_PATHLT,"dEta_Probe_PATHLT/D");
	Tree->Branch("dPtRel_Probe_PATHLT",&dPtRel_Probe_PATHLT,"dPtRel_Probe_PATHLT/D");

/////// Pair separation variables
    Tree->Branch("dphiVtxQ",&dphiVtxQ,"dphiVtxQ/D"); //(dPhi between Tag and probe at vertex) * TagCharge => if >0 cowboys, if <0 seagulls
    Tree->Branch("dr_Vtx",&dr_Vtx,"dr_Vtx/D"); // dR at vertex
    Tree->Branch("dphi_M1",&dphi_M1,"dphi_M1/D");
    Tree->Branch("dr_M1",&dr_M1,"dr_M1/D");
    Tree->Branch("dist_M1",&dist_M1,"dist_M1/D");
    Tree->Branch("dphi_M2",&dphi_M2,"dphi_M2/D");
    Tree->Branch("dr_M2",&dr_M2,"dr_M2/D");
    Tree->Branch("dist_M2",&dist_M2,"dist_M2/D");
}

int NearbyMuonsInfo::MatchHLT ( const pat::Muon & Muon, std::vector<reco::Particle> HLT,double dRmax,double dPtRelmax,double &dR, double &dPhi, double &dEta, double &dPtRel) {
	int Index = -1;
	dR = 999; dPhi = 999; dEta = 999; dPtRel = 999;
	for (int k = 0; k < static_cast<int>(HLT.size()); k++) {
		if (deltaR(HLT[k],Muon)<dRmax && deltaR(HLT[k],Muon)<dR){
			dR = deltaR(HLT[k],Muon);
			dPhi = deltaPhi(HLT[k],Muon);
			dEta = HLT[k].eta() - Muon.eta();
			dPtRel = fabs(HLT[k].pt()-Muon.pt())/HLT[k].pt();
			Index = k;
		}
   }
   return Index;
}

void 
NearbyMuonsInfo::produce(edm::Event & iEvent, const edm::EventSetup & iSetup) {
    using namespace edm;
    using namespace std;
    using pat::Muon;

    Handle<View<reco::Candidate> > src;
    iEvent.getByLabel(src_, src);
    edm::ESHandle<MagneticField> theMF;
    iSetup.get<IdealMagneticFieldRecord>().get(theMF);

    // Handle to the muon collection
    edm::Handle<std::vector<Muon> > muons;
    iEvent.getByLabel(muonsrc_, muons);

    vector <bool> accept(Paths.size(),false);
    edm::Handle<edm::TriggerResults> triggerResults ;
    iEvent.getByLabel(triggerResults_,triggerResults) ;
    edm::TriggerNames const* trigNames;
    trigNames = &iEvent.triggerNames(*triggerResults);
    for (unsigned int i=0; i<triggerResults->size(); i++) {
    	std::string trigName = trigNames->triggerName(i);
    	for(unsigned n = 0; n < Paths.size();n++){
			if(triggerResults->accept(i)&&trigName.find(Paths[n].first) != std::string::npos ) {
			  accept[n] = true;
			}
    	}
    }

    edm::Handle< trigger::TriggerEvent > triggerEvent;
    iEvent.getByLabel( trigEvent_, triggerEvent );
    const trigger::TriggerObjectCollection & triggerObjColl(triggerEvent->getObjects());
    std::vector<reco::Particle>  HLTMuons;

	for ( size_t ia = 0; ia < triggerEvent->sizeFilters(); ++ ia) {
		std::string fullname = triggerEvent->filterTag(ia).encode();
		std::string name;
		size_t p = fullname.find_first_of(':');
		if ( p != std::string::npos){
			name = fullname.substr(0, p);
		}else{
			name = fullname;
		}
		if ( &triggerObjColl !=0 ) {

			const trigger::Keys & k = triggerEvent->filterKeys(ia);
			const trigger::Vids & v = triggerEvent->filterIds(ia);
			for (unsigned ki = 0; ki !=k.size(); ++ki ){
				if (v[ki]==83){
					for (unsigned u = 0;u < Paths.size();u++){
						if (name == Paths[u].second && accept[u]) {
							bool doubled = false;
							//cout << keys << "Key" << u << "name" << Paths[u].second <<endl;
							unsigned n = HLTMuons.size();
							for (unsigned x=0;x<n;x++){
								double dPt = HLTMuons[n-1].pt() - triggerObjColl[ki].particle().pt();
								//cout << "dPt = "<< dPt<< endl;
								if (dPt<0.001) doubled = true; break;
							}
							if(!doubled){
								/*cout << "   " << ki << " " << v[ki] << "/" << k[ki] << ": "
										<<  name <<"   "<<triggerObjColl[ki].particle().pt() << " " << triggerObjColl[ki].particle().eta()
										<< " " << triggerObjColl[ki].particle().phi() << " " << triggerObjColl[ki].particle().mass()
										<< endl;*/
								HLTMuons.push_back(triggerObjColl[ki].particle());
							}
						}

					}
				}
			}
		}
	}
	//for (unsigned i = 0; i<HLTMuons.size();i++){
		//cout << "HLT pt:eta:phi = "<< HLTMuons[i].pt()<<" : "<<HLTMuons[i].eta()<<" : "<<HLTMuons[i].phi()<<endl;
	//}
	// Match HLT
	vector<double> deltaRs(muons->size(), 999), deltaPhis(muons->size(), 999), deltaEtas(muons->size(), 999), deltaPtRels(muons->size(), 999);
    vector<pair<int, int> > matchPairsSort;
    vector<pair<int, int> > matchPairsFinal; // first is pat::Muon index, second is matched HLT object index in HLTMuons vector
    vector<pair<int, float> > sortHelper;
    vector<bool> lLock(HLTMuons.size(),false);
	for (int i =0; i< static_cast<int>(muons->size());i++){
		const pat::Muon &mu = (*muons)[i];
		int match = MatchHLT(mu, HLTMuons, dRmax_, dPtRelmax_, deltaRs[i], deltaPhis[i], deltaEtas[i], deltaPtRels[i]);
		matchPairsSort.push_back(make_pair(i,match));
		matchPairsFinal.push_back(make_pair(i,-1));
		if (sortBy_ == "deltaR"){
			if (match != -1){
				sortHelper.push_back(make_pair(i,deltaRs[i]));
				//cout<<"Pushed back = (" << i<<", "<<match<<")"<<endl;
			}
			else {
				sortHelper.push_back(make_pair(i,999));
				//cout<<"Pushed back = (" << i<<", "<<999<<")"<<endl;
			}
		}
		if (sortBy_ == "deltaPhi"){
			if (match != -1){
				sortHelper.push_back(make_pair(i,deltaRs[i]));
				//cout<<"Pushed back = (" << i<<", "<<match<<")"<<endl;
			}
			else {
				sortHelper.push_back(make_pair(i,999));
				//cout<<"Pushed back = (" << i<<", "<<999<<")"<<endl;
			}
		}
		if (sortBy_ == "deltaEta"){
			if (match != -1){
				sortHelper.push_back(make_pair(i,deltaEtas[i]));
				//cout<<"Pushed back = (" << i<<", "<<match<<")"<<endl;
			}
			else {
				sortHelper.push_back(make_pair(i,999));
				//cout<<"Pushed back = (" << i<<", "<<999<<")"<<endl;
			}
		}
	}
	std::sort(sortHelper.begin(),sortHelper.end(),comparator);
	for (int j = 0, n = static_cast<int>(sortHelper.size()); j < n; ++j) {
		//cout <<"i = " <<i <<endl;
		int patIndex = matchPairsSort[sortHelper[j].first].first;
		//if (sortHelper[i].second==999) break;
		//cout <<"sortHelper[i].first "<<sortHelper[i].first<<" Reco Index = "<<recoIndex<<" dR = "<< sortHelper[i].second;

		int hltIndex = matchPairsSort[sortHelper[j].first].second;
		if(hltIndex!=-1){
			//cout <<" l1 Index = "<<l1Index<<endl;
			if (!lLock[hltIndex]){
				lLock[hltIndex] = true;
				matchPairsFinal[patIndex].second = hltIndex;
			}
		}
	}


	bool foundProbeMu = false;
	bool ProbeMuIsSA = false;
	bool foundTagHLT = false;
	bool foundProbeHLT = false;

    ///////Event
    run = iEvent.id().run();
    lumi = iEvent.luminosityBlock();
    event = iEvent.id().event();

    size_t n = src->size();
    std::vector<float> dphiVtxTimesQ(n), drVtx(n), DCA(n, -999);
    std::vector<float> drStaIn(n, -999), dphiStaIn(n, -999);
    std::vector<float> drM1(n,    -999),    dphiM1(n, -999), distM1(n, -999);
    std::vector<float> drM2(n,    -999),    dphiM2(n, -999), distM2(n, -999);
    for (size_t i = 0; i < n; ++i) {
        const reco::Candidate & ci = (*src)[i];
        if (ci.numberOfDaughters() != 2) throw cms::Exception("CorruptData") << 
            "NearbyMuonsInfo should be used on composite candidates with two daughters, this one has " << ci.numberOfDaughters() << "\n";
        const reco::Candidate &d1 = *ci.daughter(0), &d2 = *ci.daughter(1);
        dphiVtxTimesQ[i] = d1.charge() * deltaPhi(d1.phi(), d2.phi());
        drVtx[i] = deltaR(d1,d2);
        dphiVtxQ = d1.charge() * deltaPhi(d1.phi(), d2.phi());
        dr_Vtx=deltaR(d1,d2);
        TagCharge = d1.charge();
        ProbeCharge = d2.charge();
        Tag_pt = d1.pt();
        Tag_phi = d1.phi();
        Tag_eta = d1.eta();
        Probe_pt = d2.pt();
        Probe_phi = d2.phi();
        Probe_eta = d2.eta();
        //cout <<"Probe track pt:eta:phi = "<<Probe_pt<<" : "<<Probe_eta<<" : "<<Probe_phi<<endl;

        const reco::RecoCandidate *mu1 = dynamic_cast<const reco::RecoCandidate *>(&*d1.masterClone());
        const reco::RecoCandidate *mu2 = dynamic_cast<const reco::RecoCandidate *>(&*d2.masterClone());
        if (mu1 == 0) throw cms::Exception("CorruptData") << "First daughter of candidate is not a ShallowClone of a reco::RecoCandidate\n";
        if (mu2 == 0) throw cms::Exception("CorruptData") << "Second daughter of candidate is not a ShallowClone of a reco::RecoCandidate\n";
        
        //////////////////////////
        const pat::Muon * tagMuon = dynamic_cast<const pat::Muon*>(&*d1.masterClone());
        const pat::GenericParticle * gp = dynamic_cast<const pat::GenericParticle*>(&*d2.masterClone());
        bool has = gp->hasUserCand("l1MuTkMatch");
        //cout<< has<<endl;

        // Probe-MuonMatching
        //cout<< "Tag muon: pt:eta:phi =  "<< tagMuon->pt()<<":"<<tagMuon->eta()<<":"<< tagMuon->phi()<< endl;
		reco::TrackRef  TagTrackRef = tagMuon->innerTrack();
		reco::TrackRef  TagSARef = tagMuon->outerTrack();
		TagTk_pt = TagTrackRef->pt();
		TagTk_phi = TagTrackRef->phi();
		TagTk_eta = TagTrackRef->eta();
		TagSA_pt = TagSARef->pt();
		TagSA_phi = TagSARef->phi();
		TagSA_eta = TagSARef->eta();
		foundProbeMu = false;
		ProbeMuIsSA = false;
		foundTagHLT = false;
		foundProbeHLT = false;
		bool matchedPATHLT = false;
		bool matchedProbePATHLT = false;
		for (int m =0; m< static_cast<int>(muons->size());m++){
			const pat::Muon &patMuon = (*muons)[m];


			//cout<<"Muon:      pt:eta:phi =  "<< patMuon.pt()<<":"<<patMuon.eta()<<":"<< patMuon.phi()<< endl;
        	if (patMuon.isStandAloneMuon()&&patMuon.isTrackerMuon()){
        		reco::TrackRef  MuonTrackRef = patMuon.innerTrack();
        		reco::TrackRef  MuonSARef = patMuon.outerTrack();
        		//cout<<"Track      ref pt:eta:phi = "<< MuonTrackRef->pt()<<":"<<MuonTrackRef->eta()<<":"<< MuonTrackRef->phi()<< endl;
        		//cout<<"StandAlone ref pt:eta:phi = "<< MuonSARef->pt()<<":"<<MuonSARef->eta()<<":"<< MuonSARef->phi()<< endl;
        	}
			float dPtTag = abs(d1.pt() - patMuon.pt());
			float dPhiTag = abs(d1.phi() - patMuon.phi());
			float dEtaTag = abs(d1.eta() - patMuon.eta());
			if (dPtTag<0.001 && dEtaTag < 0.001 && dEtaTag < 0.001){

				for (unsigned w = 0; w< Paths.size();w++){
					//cout<<"Muon:      pt:eta:phi =  "<< patMuon.pt()<<":"<<patMuon.eta()<<":"<< patMuon.phi()<< " "<<endl;
					pat::TriggerObjectStandAloneCollection mu0tkMatch = patMuon.triggerObjectMatchesByPath(Paths[w].first+"*",true,true);
					if (mu0tkMatch.size()>0&&!matchedPATHLT){
					for (unsigned g=0;g<mu0tkMatch.size();g++){
						//cout << " Match: "<< mu0tkMatch[g].pt()<<":"<<mu0tkMatch[g].eta()<<":"<< mu0tkMatch[g].phi()<<endl;
					}

						//cout<<"Muon:      pt:eta:phi =  "<< patMuon.pt()<<":"<<patMuon.eta()<<":"<< patMuon.phi()<< " "<<Paths[w].second
						//		<< " Match: "<< mu0tkMatch[0].pt()<<":"<<mu0tkMatch[0].eta()<<":"<< mu0tkMatch[0].phi()<<endl;
						matchedPATHLT = true;
						Tag_PATHLT_pt = mu0tkMatch[0].pt();
						Tag_PATHLT_phi = mu0tkMatch[0].phi();
						Tag_PATHLT_eta = mu0tkMatch[0].eta();
						dR_Tag_PATHLT = deltaR(patMuon,mu0tkMatch[0]);
						//cout <<deltaR(patMuon,mu0tkMatch[0])<<endl;
						dPhi_Tag_PATHLT = deltaPhi(patMuon,mu0tkMatch[0]);
						dEta_Tag_PATHLT = patMuon.eta() - mu0tkMatch[0].eta();
						dPtRel_Tag_PATHLT = abs(patMuon.pt() - mu0tkMatch[0].pt())/mu0tkMatch[0].pt();

					}
				}
				if (matchPairsFinal[m].second!=-1){
					Tag_HLT_pt = HLTMuons[matchPairsFinal[m].second].pt();
					Tag_HLT_phi = HLTMuons[matchPairsFinal[m].second].phi();
					Tag_HLT_eta = HLTMuons[matchPairsFinal[m].second].eta();
					dR_Tag_HLT = deltaRs[i];
					dPhi_Tag_HLT = deltaPhis[i];
					dEta_Tag_HLT = deltaEtas[i];
					dPtRel_Tag_HLT = deltaPtRels[i];
					foundTagHLT = true;
				}
			}
        	if (dPtTag > 0.001&&dPhiTag>0.001&&dEtaTag>0.001){
        		if (patMuon.isTrackerMuon()){
        			reco::TrackRef  MuonTrackRef = patMuon.innerTrack();
        			float dPtProbe = abs(d2.pt() - MuonTrackRef->pt());
        			float dPhiProbe = abs(d2.phi() - MuonTrackRef->phi());
        			float dEtaProbe = abs(d2.eta() - MuonTrackRef->eta());
        			//cout << dPt <<" "<<dEta<<" "<<dPhi<<endl;
        			if (dPtProbe < 0.001&&dEtaProbe<0.001&&dPhiProbe<0.001){
    					for (unsigned w = 0; w< Paths.size();w++){
    						//cout<<"Muon:      pt:eta:phi =  "<< patMuon.pt()<<":"<<patMuon.eta()<<":"<< patMuon.phi()<< " "<<endl;
    						pat::TriggerObjectStandAloneCollection mu0tkMatch = patMuon.triggerObjectMatchesByPath(Paths[w].first+"*",true,true);
    						if (mu0tkMatch.size()>0&&!matchedProbePATHLT){
    						//for (unsigned g=0;g<mu0tkMatch.size();g++){
    						//	cout << " Match: "<< mu0tkMatch[g].pt()<<":"<<mu0tkMatch[g].eta()<<":"<< mu0tkMatch[g].phi()<<endl;
    						//}

    							//cout<<"Muon:      pt:eta:phi =  "<< patMuon.pt()<<":"<<patMuon.eta()<<":"<< patMuon.phi()<< " "<<Paths[w].second
    							//		<< " Match: "<< mu0tkMatch[0].pt()<<":"<<mu0tkMatch[0].eta()<<":"<< mu0tkMatch[0].phi()<<endl;
    							matchedProbePATHLT = true;
    							Probe_PATHLT_pt = mu0tkMatch[0].pt();
    							Probe_PATHLT_phi = mu0tkMatch[0].phi();
    							Probe_PATHLT_eta = mu0tkMatch[0].eta();
    							dR_Probe_PATHLT = deltaR(patMuon,mu0tkMatch[0]);
    							dPhi_Probe_PATHLT = deltaPhi(patMuon,mu0tkMatch[0]);
    							dEta_Probe_PATHLT = patMuon.eta() - mu0tkMatch[0].eta();
    							dPtRel_Probe_PATHLT = abs(patMuon.pt() - mu0tkMatch[0].pt())/mu0tkMatch[0].pt();
    						}
    					}
        				if (matchPairsFinal[m].second!=-1){
        					Probe_HLT_pt = HLTMuons[matchPairsFinal[m].second].pt();
        					Probe_HLT_phi = HLTMuons[matchPairsFinal[m].second].phi();
        					Probe_HLT_eta = HLTMuons[matchPairsFinal[m].second].eta();
        					dR_Probe_HLT = deltaRs[i];
        					dPhi_Probe_HLT = deltaPhis[i];
        					dEta_Probe_HLT = deltaEtas[i];
        					dPtRel_Probe_HLT = deltaPtRels[i];
        					foundProbeHLT = true;
        				}
        				ProbeMu_pt = MuonTrackRef->pt();
        				ProbeMu_phi = MuonTrackRef->phi();
        				ProbeMu_eta = MuonTrackRef->eta();
        				foundProbeMu = true;
        				if(patMuon.isStandAloneMuon()){
        					reco::TrackRef  ProbeSARef = patMuon.outerTrack();
            				ProbeSA_pt = ProbeSARef->pt();
            				ProbeSA_phi = ProbeSARef->phi();
            				ProbeSA_eta = ProbeSARef->eta();
            				ProbeMuIsSA = true;
        				}
        			}
        		}
        	}
        }
        if (!foundProbeMu){
			ProbeMu_pt = -999;
			ProbeMu_phi = -999;
			ProbeMu_eta = -999;
        }
        if (!ProbeMuIsSA){
			ProbeSA_pt = -999;
			ProbeSA_phi = -999;
			ProbeSA_eta = -999;
        }
        if (!foundTagHLT){
			Tag_HLT_pt = -999;
			Tag_HLT_phi = -999;
			Tag_HLT_eta = -999;
			dR_Tag_HLT = 999;
			dPhi_Tag_HLT = 999;
			dEta_Tag_HLT = 999;
			dPtRel_Tag_HLT = 999;
		}
        if(!foundProbeHLT){
			Probe_HLT_pt = -999;
			Probe_HLT_phi = -999;
			Probe_HLT_eta = -999;
			dR_Probe_HLT = 999;
			dPhi_Probe_HLT = 999;
			dEta_Probe_HLT = 999;
			dPtRel_Probe_HLT = 999;
		}
		if (!matchedPATHLT){
			Tag_PATHLT_pt = -999;
			Tag_PATHLT_phi = -999;
			Tag_PATHLT_eta = -999;
			dR_Tag_PATHLT = 999;
			dPhi_Tag_PATHLT = 999;
			dEta_Tag_PATHLT = 999;
			dPtRel_Tag_PATHLT = 999;
		}
		if (!matchedProbePATHLT){
			Probe_PATHLT_pt = -999;
			Probe_PATHLT_phi = -999;
			Probe_PATHLT_eta = -999;
			dR_Probe_PATHLT = 999;
			dPhi_Probe_PATHLT = 999;
			dEta_Probe_PATHLT = 999;
			dPtRel_Probe_PATHLT = 999;
		}

        if(has&&!gp->userCand("l1MuTkMatch").isNull()){
        	reco::CandidatePtr l1ptr =  gp->userCand("l1MuTkMatch");
            const l1extra::L1MuonParticle  &l1 = dynamic_cast<const l1extra::L1MuonParticle &>(*l1ptr);
            const L1MuGMTCand & gmt = l1.gmtMuonCand();
            const L1MuRegionalCand & reg = l1.regRPCMuonCand();
            const L1MuRegionalCand & regDT = l1.regDTMuonCand();
            const L1MuRegionalCand & regCSC = l1.regCSCMuonCand();
			L1quality = gmt.quality(); //cout<<"Quality"<<gmt.quality()<<endl;
        	if (gmt.quality()==5|| gmt.quality()==7 ||  gmt.quality()==6){
				L1E_pt       = gmt.ptValue();
				L1E_eta      = gmt.etaValue();
				L1E_phi      = gmt.phiValue();
				isRPC = l1.isRPCCand();
				isDT = l1.isDTCand();
				isCSC = l1.isCSCCand();


				//cout<<" L1E pt:eta:phi:bx="<< gmt.ptValue()<<":"<<gmt.etaValue()<<":"<<gmt.phiValue()<<":"<<gmt.bx()<<endl;
				//if(!reg.empty()) cout<<" REG pt:eta:phi:bx="<< reg.ptValue()<<":"<<reg.etaValue()<<":"<<reg.phiValue()<<":"<<reg.bx()<<endl;
				
				if (!reg.empty()){
					dR_L1_RPC = deltaR(gmt.etaValue(), gmt.phiValue(),reg.etaValue(),reg.phiValue());
					dEta_L1_RPC = gmt.etaValue()-reg.etaValue();
					dPhi_L1_RPC = gmt.phiValue()-reg.phiValue();
					RPC_pt       = reg.ptValue();
					RPC_eta      = reg.etaValue();
					RPC_phi      = reg.phiValue();
				} else {
					RPC_pt       = -999;
					RPC_eta      = -999;
					RPC_phi      = -999;
					dR_L1_RPC       =  999;
					dEta_L1_RPC = 999;
					dPhi_L1_RPC = 999;
					
				}
				//if(!regDT.empty()) cout<<" REGDT pt:eta:phi:bx="<< regDT.ptValue()<<":"<<regDT.etaValue()<<":"<<regDT.phiValue()<<":"<<regDT.bx()<<endl;
				if (!regDT.empty()){
					dR_L1_DT = deltaR(gmt.etaValue(), gmt.phiValue(),regDT.etaValue(),regDT.phiValue());
					dEta_L1_DT = gmt.etaValue()-regDT.etaValue();
					dPhi_L1_DT = gmt.phiValue()-regDT.phiValue();
					DT_pt       = regDT.ptValue();
					DT_eta      = regDT.etaValue();
					DT_phi      = regDT.phiValue();
					DT_fine 	 = regDT.isFineHalo();
				} else { 
					dR_L1_DT = 999;
					dEta_L1_DT = 999;
					dPhi_L1_DT = 999;
					DT_pt       = -999;
					DT_eta      = -999;
					DT_phi      = -999;
					DT_fine		  = false;
				}
				//if(!regCSC.empty()) cout<<" REGCSC pt:eta:phi:bx="<< regCSC.ptValue()<<":"<<regCSC.etaValue()<<":"<<regCSC.phiValue()<<":"<<regCSC.bx()<<endl;
				if (!regCSC.empty()){
					dR_L1_CSC = deltaR(gmt.etaValue(), gmt.phiValue(),regCSC.etaValue(),regCSC.phiValue());
					dEta_L1_CSC = gmt.etaValue()-regCSC.etaValue();
					dPhi_L1_CSC = gmt.phiValue()-regCSC.phiValue();
					CSC_pt       = regCSC.ptValue();
					CSC_eta      = regCSC.etaValue();
					CSC_phi      = regCSC.phiValue();
				} else { 
					dR_L1_CSC = 999;
					dEta_L1_CSC = 999;
					dPhi_L1_CSC = 999;
					CSC_pt       = -999;
					CSC_eta      = -999;
					CSC_phi      = -999;
				}

        	} else{
				L1E_pt       = -999;
				L1E_eta      = -999;
				L1E_phi      = -999;
				RPC_pt       = -999;
				RPC_eta      = -999;
				RPC_phi      = -999;
				dR_L1_RPC    =  999;
				dR_L1_DT     =  999;
				dR_L1_CSC    =  999;
				dEta_L1_RPC  =  999;
				dEta_L1_DT   =  999;
				dEta_L1_CSC  =  999;
				dPhi_L1_RPC  =  999;
				dPhi_L1_DT   =  999;
				dPhi_L1_CSC  =  999;
				DT_pt        = -999;
				DT_eta       = -999;
				DT_phi       = -999;
				CSC_pt       = -999;
				CSC_eta      = -999;
				CSC_phi      = -999;
				DT_fine		 = false;
        	}

        } else{
			L1E_pt       = -999;
			L1E_eta      = -999;
			L1E_phi      = -999;
			RPC_pt       = -999;
			RPC_eta      = -999;
			RPC_phi      = -999;
			dR_L1_RPC    =  999;
			dR_L1_DT     =  999;
			dR_L1_CSC    =  999;
			dEta_L1_RPC  =  999;
			dEta_L1_DT   =  999;
			dEta_L1_CSC  =  999;
			dPhi_L1_RPC  =  999;
			dPhi_L1_DT   =  999;
			dPhi_L1_CSC  =  999;
			DT_pt        = -999;
			DT_eta       = -999;
			DT_phi       = -999;
			CSC_pt       = -999;
			CSC_eta      = -999;
			CSC_phi      = -999;
			DT_fine		 = false;
    	}
	//// Start DCA Calculation //////
	reco::TrackRef tk = mu1->get<reco::TrackRef>();
	reco::TrackRef tk2nd = mu2->get<reco::TrackRef>();
	reco::TransientTrack transMu1(*tk, &(*theMF) );
	reco::TransientTrack transMu2(*tk2nd, &(*theMF) );
	TrajectoryStateClosestToPoint mu1TS = transMu1.impactPointTSCP();
	TrajectoryStateClosestToPoint mu2TS = transMu2.impactPointTSCP();
	if (mu1TS.isValid() && mu2TS.isValid()) {
	  ClosestApproachInRPhi cApp;
	  cApp.calculate(mu1TS.theState(), mu2TS.theState());
	  if (cApp.status()) DCA[i] = cApp.distance();
	}
	//// End DCA Calculation //////

        if (mu1->standAloneMuon().isNonnull()            && mu2->standAloneMuon().isNonnull() &&
            mu1->standAloneMuon().isAvailable()          && mu2->standAloneMuon().isAvailable() &&
            mu1->standAloneMuon()->extra().isAvailable() && mu2->standAloneMuon()->extra().isAvailable()  ) {
            dphiStaIn[i] = deltaPhi(mu1->standAloneMuon()->innerPosition().Phi(), mu2->standAloneMuon()->innerPosition().Phi());
            drStaIn[i]   = hypot(dphiStaIn[i], std::abs(mu1->standAloneMuon()->innerPosition().Eta() - mu2->standAloneMuon()->innerPosition().Eta()));
        }
        // Propagate to station 1
        TrajectoryStateOnSurface prop1_M1 = prop1_.extrapolate(*mu1);
        TrajectoryStateOnSurface prop2_M1 = prop1_.extrapolate(*mu2);
        if (prop1_M1.isValid() && prop2_M1.isValid()) {
            dphiM1[i] = deltaPhi<float>(prop1_M1.globalPosition().phi(), prop2_M1.globalPosition().phi());
            drM1[i]   = hypot(dphiM1[i], std::abs<float>(prop1_M1.globalPosition().eta() - prop2_M1.globalPosition().eta()));
            distM1[i] = (prop1_M1.globalPosition()-prop2_M1.globalPosition()).mag();

            dphi_M1 = deltaPhi<float>(prop1_M1.globalPosition().phi(), prop2_M1.globalPosition().phi());
            dr_M1   = hypot(dphiM1[i], std::abs<float>(prop1_M1.globalPosition().eta() - prop2_M1.globalPosition().eta()));
            dist_M1 = (prop1_M1.globalPosition()-prop2_M1.globalPosition()).mag();
            tag_M1_glob_x= prop1_M1.globalPosition().x();
            tag_M1_glob_y= prop1_M1.globalPosition().y();
            tag_M1_glob_z= prop1_M1.globalPosition().z();
            probe_M1_glob_x= prop2_M1.globalPosition().x();
            probe_M1_glob_y= prop2_M1.globalPosition().y();
            probe_M1_glob_z= prop2_M1.globalPosition().z();
            Tag_eta_propM1 = prop1_M1.globalPosition().eta();
            Tag_phi_propM1 = prop1_M1.globalPosition().phi();
            Probe_eta_propM1 = prop2_M1.globalPosition().eta();
            Probe_phi_propM1 = prop2_M1.globalPosition().phi();


        }
        else {
            Tag_eta_propM1 = -999;
            Tag_phi_propM1 = -999;
            Probe_eta_propM1 = -999;
            Probe_phi_propM1 = -999;
            tag_M1_glob_x= -999;
            tag_M1_glob_y= -999;
            tag_M1_glob_z= -999;
            probe_M1_glob_x= -999;
            probe_M1_glob_y= -999;
            probe_M1_glob_z= -999;
            dphi_M1= -999;
            dr_M1= -999;
            dist_M1= -999;
        }
        // Propagate to station 2
        TrajectoryStateOnSurface prop1_M2 = prop2_.extrapolate(*mu1);
        TrajectoryStateOnSurface prop2_M2 = prop2_.extrapolate(*mu2);
        if (prop1_M2.isValid() && prop2_M2.isValid()) {

            Tag_eta_propM2 = prop1_M2.globalPosition().eta();
            Tag_phi_propM2 = prop1_M2.globalPosition().phi();
            Probe_eta_propM2 = prop2_M2.globalPosition().eta();
            Probe_phi_propM2 = prop2_M2.globalPosition().phi();


            dphiM2[i] = deltaPhi<float>(prop1_M2.globalPosition().phi(), prop2_M2.globalPosition().phi());
            drM2[i]   = hypot(dphiM2[i], std::abs<float>(prop1_M2.globalPosition().eta() - prop2_M2.globalPosition().eta()));
            distM2[i] = (prop1_M2.globalPosition()-prop2_M2.globalPosition()).mag();

            dphi_M2 = deltaPhi<float>(prop1_M2.globalPosition().phi(), prop2_M2.globalPosition().phi());
            dr_M2   = hypot(dphiM2[i], std::abs<float>(prop1_M2.globalPosition().eta() - prop2_M2.globalPosition().eta()));
            dist_M2 = (prop1_M2.globalPosition()-prop2_M2.globalPosition()).mag();

            tag_M2_glob_x= prop1_M2.globalPosition().x();
            tag_M2_glob_y= prop1_M2.globalPosition().y();
            tag_M2_glob_z= prop1_M2.globalPosition().z();
            probe_M2_glob_x= prop2_M2.globalPosition().x();
            probe_M2_glob_y= prop2_M2.globalPosition().y();
            probe_M2_glob_z= prop2_M2.globalPosition().z();
            if(has&&!gp->userCand("l1MuTkMatch").isNull()){
               	reco::CandidatePtr l1ptr =  gp->userCand("l1MuTkMatch");
                const l1extra::L1MuonParticle  &l1 = dynamic_cast<const l1extra::L1MuonParticle &>(*l1ptr);
                const L1MuGMTCand & gmt = l1.gmtMuonCand();
                float dPhi;
                if (gmt.phiValue()+1.25 * 3.14/180.-Probe_phi_propM2<3.14) dPhi = gmt.phiValue()+1.25 * 3.14/180.-Probe_phi_propM2;
                else dPhi = gmt.phiValue()+1.25 * 3.14/180.-Probe_phi_propM2-6.28;
                dPhi_MuL1 = dPhi;
                dEta_MuL1 = gmt.etaValue()-Probe_eta_propM2;
                dR_MuL1 = deltaR(gmt.etaValue(), gmt.phiValue()+1.25 * 3.14/180.,Probe_eta_propM2,Probe_phi_propM2);
            } else {
                dPhi_MuL1 = -999;
                dEta_MuL1 = -999;
                dR_MuL1 = -999;
            }

        }
        else {
            Tag_eta_propM2 = -999;
            Tag_phi_propM2 = -999;
            Probe_eta_propM2 = -999;
            Probe_phi_propM2 = -999;
            tag_M2_glob_x= -999;
            tag_M2_glob_y= -999;
            tag_M2_glob_z= -999;
            probe_M2_glob_x= -999;
            probe_M2_glob_y= -999;
            probe_M2_glob_z= -999;
            dphi_M2= -999;
            dr_M2= -999;
            dist_M2= -999;
            dPhi_MuL1 = -999;
            dEta_MuL1 = -999;
            dR_MuL1 = -999;
        }

        Tree->Fill();
    }

    writeValueMap(iEvent, src, DCA,            "DCA");
    writeValueMap(iEvent, src, dphiVtxTimesQ,  "dphiVtxTimesQ");
    writeValueMap(iEvent, src, drVtx,          "drVtx");
    writeValueMap(iEvent, src, drStaIn,        "drStaIn");
    writeValueMap(iEvent, src, dphiStaIn,      "dphiStaIn");
    writeValueMap(iEvent, src, drM1,           "drM1");
    writeValueMap(iEvent, src, dphiM1,         "dphiM1");
    writeValueMap(iEvent, src, distM1,         "distM1");
    writeValueMap(iEvent, src, drM2,           "drM2");
    writeValueMap(iEvent, src, dphiM2,         "dphiM2");
    writeValueMap(iEvent, src, distM2,         "distM2");
}

void
NearbyMuonsInfo::writeValueMap(edm::Event &iEvent,
        const edm::Handle<edm::View<reco::Candidate> > & handle,
        const std::vector<float> & values,
        const std::string    & label) const 
{
    using namespace edm; 
    using namespace std;
    auto_ptr<ValueMap<float> > valMap(new ValueMap<float>());
    edm::ValueMap<float>::Filler filler(*valMap);
    filler.insert(handle, values.begin(), values.end());
    filler.fill();
    iEvent.put(valMap, label);
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(NearbyMuonsInfo);
