//
// $Id: NearbyMuonsInfo.cc,v 1.6 2011/07/21 01:07:49 botta Exp $
//

/**
  \class    pat::NearbyMuonsInfo NearbyMuonsInfo.h "PhysicsTools/PatAlgos/interface/NearbyMuonsInfo.h"
  \brief    Matcher of reconstructed objects to L1 Muons 
            
  \author   Giovanni Petrucciani
  \version  $Id: NearbyMuonsInfo.cc,v 1.6 2011/07/21 01:07:49 botta Exp $
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
#include "DataFormats/Common/interface/Ptr.h"
#include <functional>

//// Includes for DCA: /////
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"

#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutRecord.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"

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
    private:
        edm::InputTag src_, rawmatcher_;
        PropagateToMuon prop1_, prop2_;

        /// Write a ValueMap<float> in the event
        void writeValueMap(edm::Event &iEvent,
                const edm::Handle<edm::View<reco::Candidate> > & handle,
                const std::vector<float> & values,
                const std::string    & label) const ;

        TFile* hFile;
        TTree *Tree;

        double TagCharge;
        double ProbeCharge;
        double RPC_pt;
        double RPC_eta;
        double RPC_phi;

        double DT_pt;
        double DT_eta;
        double DT_phi;
        
        double CSC_pt;
        double CSC_eta;
        double CSC_phi;

        double L1quality;

        double L1E_pt;
        double L1E_eta;
        double L1E_phi;

        double run;
        double lumi;
        double event;

        double dR_RPC;
        double dR_DT;
        double dR_CSC;
        double dR_MuL1;
        double dPhi_MuL1;
        double dEta_MuL1;

        double dR_L1_RPC;
        double dR_L1_DT;
        double dR_L1_CSC;

        double dEta_L1_RPC;
        double dEta_L1_DT;
        double dEta_L1_CSC;

        double dPhi_L1_RPC;
        double dPhi_L1_DT;
        double dPhi_L1_CSC;

        double TagMu_pt;
        double TagMu_phi;
        double TagMu_eta;
        double ProbeMu_pt;
        double ProbeMu_phi;
        double ProbeMu_eta;

        double TagMu_eta_propM1;
        double TagMu_phi_propM1;
        double ProbeMu_eta_propM1;
        double ProbeMu_phi_propM1;

        double TagMu_eta_propM2;
        double TagMu_phi_propM2;
        double ProbeMu_eta_propM2;
        double ProbeMu_phi_propM2;
        double dphiVtxQ;
        double drVt;
        double dphi_M1;
        double dr_M1;
        double dist_M1;
        double dphi_M2;
        double dr_M2;
        double dist_M2;
        double tag_M1_glob_x;
        double tag_M1_glob_y;
        double tag_M1_glob_z;
        double probe_M1_glob_x;
        double probe_M1_glob_y;
        double probe_M1_glob_z;
        double tag_M2_glob_x;
        double tag_M2_glob_y;
        double tag_M2_glob_z;
        double probe_M2_glob_x;
        double probe_M2_glob_y;
        double probe_M2_glob_z;
        bool DT_fine;
        bool isDT, isRPC,isCSC;

};

NearbyMuonsInfo::NearbyMuonsInfo(const edm::ParameterSet & iConfig) :
    src_(iConfig.getParameter<edm::InputTag>("src")),
    rawmatcher_(iConfig.getParameter<edm::InputTag>("rawmatcher")),
    prop1_(iConfig.getParameter<edm::ParameterSet>("propM1")),
    prop2_(iConfig.getParameter<edm::ParameterSet>("propM2"))
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
    hFile = new TFile("TagProbePairDebug.root","recreate");
    hFile->cd();

    Tree = new TTree("Tree","Tree");

    Tree->Branch("ProbeCharge",&ProbeCharge,"ProbeCharge/D");
    Tree->Branch("TagCharge",&TagCharge,"TagCharge/D");
    Tree->Branch("L1E_pt",&L1E_pt,"L1E_pt/D");
    Tree->Branch("L1E_eta",&L1E_eta,"L1E_eta/D");
    Tree->Branch("L1E_phi",&L1E_phi,"L1E_phi/D");
    Tree->Branch("RPC_pt",&RPC_pt,"RPC_pt/D");
    Tree->Branch("RPC_eta",&RPC_eta,"RPC_eta/D");
    Tree->Branch("RPC_phi",&RPC_phi,"RPC_phi/D");

    Tree->Branch("DT_pt",&DT_pt,"DT_pt/D");
    Tree->Branch("DT_eta",&DT_eta,"DT_eta/D");
    Tree->Branch("DT_phi",&DT_phi,"DT_phi/D");
    
    Tree->Branch("isRPC",&isRPC,"isRPC/B");
    Tree->Branch("isDT",&isDT,"isDT/B");
    Tree->Branch("isCSC",&isCSC,"isCSC/B");

    Tree->Branch("CSC_pt",&CSC_pt,"CSC_pt/D");
    Tree->Branch("CSC_eta",&CSC_eta,"CSC_eta/D");
    Tree->Branch("CSC_phi",&CSC_phi,"CSC_phi/D");

    Tree->Branch("run",&run,"run/D");
    Tree->Branch("lumi",&lumi,"lumi/D");
    Tree->Branch("event",&event,"event/D");
    
    Tree->Branch("dR_L1_RPC",&dR_L1_RPC,"dR_L1_RPC/D");
    Tree->Branch("dR_L1_DT",&dR_L1_DT,"dR_L1_DT/D");
    Tree->Branch("dR_L1_CSC",&dR_L1_CSC,"dR_L1_CSC/D");
    Tree->Branch("dEta_L1_RPC",&dEta_L1_RPC,"dEta_L1_RPC/D");
    Tree->Branch("dEta_L1_DT",&dEta_L1_DT,"dEta_L1_DT/D");
    Tree->Branch("dEta_L1_CSC",&dEta_L1_CSC,"dEta_L1_CSC/D");
    Tree->Branch("dPhi_L1_RPC",&dPhi_L1_RPC,"dPhi_L1_RPC/D");
    Tree->Branch("dPhi_L1_DT",&dPhi_L1_DT,"dPhi_L1_DT/D");
    Tree->Branch("dPhi_L1_CSC",&dPhi_L1_CSC,"dPhi_L1_CSC/D");

    Tree->Branch("dphiVtxQ",&dphiVtxQ,"dphiVtxQ/D");
    Tree->Branch("drVt",&drVt,"drVt/D");
    Tree->Branch("dphi_M1",&dphi_M1,"dphi_M1/D");
    Tree->Branch("dr_M1",&dr_M1,"dr_M1/D");
    Tree->Branch("dist_M1",&dist_M1,"dist_M1/D");
    Tree->Branch("dphi_M2",&dphi_M2,"dphi_M2/D");
    Tree->Branch("dr_M2",&dr_M2,"dr_M2/D");
    Tree->Branch("dist_M2",&dist_M2,"dist_M2/D");
    Tree->Branch("tag_M1_glob_x",&tag_M1_glob_x,"tag_M1_glob_x/D");
    Tree->Branch("tag_M1_glob_y",&tag_M1_glob_y,"tag_M1_glob_y/D");
    Tree->Branch("tag_M1_glob_z",&tag_M1_glob_z,"tag_M1_glob_z/D");
    Tree->Branch("probe_M1_glob_x",&probe_M1_glob_x,"probe_M1_glob_x/D");
    Tree->Branch("probe_M1_glob_y",&probe_M1_glob_y,"probe_M1_glob_y/D");
    Tree->Branch("probe_M1_glob_z",&probe_M1_glob_z,"probe_M1_glob_z/D");
    Tree->Branch("tag_M2_glob_x",&tag_M2_glob_x,"tag_M2_glob_x/D");
    Tree->Branch("tag_M2_glob_y",&tag_M2_glob_y,"tag_M2_glob_y/D");
    Tree->Branch("tag_M2_glob_z",&tag_M2_glob_z,"tag_M2_glob_z/D");
    Tree->Branch("probe_M2_glob_x",&probe_M2_glob_x,"probe_M2_glob_x/D");
    Tree->Branch("probe_M2_glob_y",&probe_M2_glob_y,"probe_M2_glob_y/D");
    Tree->Branch("probe_M2_glob_z",&probe_M2_glob_z,"probe_M2_glob_z/D");


    Tree->Branch("dR_DT",&dR_DT,"dR_DT/D");
    Tree->Branch("dR_CSC",&dR_CSC,"dR_CSC/D");    
    Tree->Branch("dR_MuL1",&dR_MuL1,"dR_MuL1/D");
    Tree->Branch("dPhi_MuL1",&dPhi_MuL1,"dPhi_MuL1/D");
    Tree->Branch("dEta_MuL1",&dEta_MuL1,"dEta_MuL1/D");
    Tree->Branch("dR_RPC",&dR_RPC,"dR_RPC/D");
    Tree->Branch("L1quality",&L1quality,"L1quality/D");


    Tree->Branch("TagMu_pt",&TagMu_pt,"TagMu_pt/D");
    Tree->Branch("TagMu_phi",&TagMu_phi,"TagMu_phi/D");
    Tree->Branch("TagMu_eta",&TagMu_eta,"TagMu_eta/D");
    Tree->Branch("ProbeMu_pt",&ProbeMu_pt,"ProbeMu_pt/D");
    Tree->Branch("ProbeMu_phi",&ProbeMu_phi,"ProbeMu_phi/D");
    Tree->Branch("ProbeMu_eta",&ProbeMu_eta,"ProbeMu_eta/D");
    Tree->Branch("TagMu_eta_propM1",&TagMu_eta_propM1,"TagMu_eta_propM1/D");
    Tree->Branch("TagMu_phi_propM1",&TagMu_phi_propM1,"TagMu_phi_propM1/D");
    Tree->Branch("ProbeMu_eta_propM1",&ProbeMu_eta_propM1,"ProbeMu_eta_propM1/D");
    Tree->Branch("ProbeMu_phi_propM1",&ProbeMu_phi_propM1,"ProbeMu_phi_propM1/D");
    Tree->Branch("TagMu_eta_propM2",&TagMu_eta_propM2,"TagMu_eta_propM2/D");
    Tree->Branch("TagMu_phi_propM2",&TagMu_phi_propM2,"TagMu_phi_propM2/D");
    Tree->Branch("ProbeMu_eta_propM2",&ProbeMu_eta_propM2,"ProbeMu_eta_propM2/D");
    Tree->Branch("ProbeMu_phi_propM2",&ProbeMu_phi_propM2,"ProbeMu_phi_propM2/D");
    Tree->Branch("DT_fine",&DT_fine,"DT_fine/B");

}

void 
NearbyMuonsInfo::produce(edm::Event & iEvent, const edm::EventSetup & iSetup) {
    using namespace edm;
    using namespace std;

    Handle<View<reco::Candidate> > src;
    iEvent.getByLabel(src_, src);
    edm::ESHandle<MagneticField> theMF;
    iSetup.get<IdealMagneticFieldRecord>().get(theMF);

    run = iEvent.id().run();
    lumi = iEvent.luminosityBlock();
    event = iEvent.id().event();

       Handle< L1MuGMTReadoutCollection > hwMuCollection ;
        iEvent.getByLabel( "gtDigis", hwMuCollection ) ;
        vector< L1MuRegionalCand > hwMuRegRPCCands ;
        vector< L1MuGMTReadoutRecord > records = hwMuCollection->getRecords();
	vector< L1MuGMTReadoutRecord >::const_iterator rItr = records.begin();
	vector< L1MuGMTReadoutRecord >::const_iterator rEnd = records.end();
 	//test Regional cands
	/*for( ; rItr != rEnd ; ++rItr ){
                vector< L1MuGMTExtendedCand > tmpCands = rItr->getGMTCands() ;
                vector< L1MuRegionalCand > tmpRegBrlCands = rItr->getBrlRPCCands();
                vector< L1MuRegionalCand > tmpRegFwdCands = rItr->getFwdRPCCands();
                vector< L1MuRegionalCand > tmpRegDTCands = rItr->getDTBXCands();
                vector< L1MuRegionalCand > tmpRegCSCCands = rItr->getCSCCands();                          
                for (vector< L1MuRegionalCand >::const_iterator test=tmpRegBrlCands.begin(); test!=tmpRegBrlCands.end();test++){
			  std::cout<< "Is test empty? "<< !test->empty();
			  if(!test->empty()) cout << "BRL pt:eta:phi:bx="<< test->ptValue()<<":"<<test->etaValue()<<":"<<test->phiValue()<<":"<<test->bx()<<endl; 
		}		
		for (vector< L1MuRegionalCand >::const_iterator test=tmpRegFwdCands.begin(); test!=tmpRegFwdCands.end();test++){
			  std::cout<< !test->empty()<< std::endl;
			  if(!test->empty()) cout << "FWD pt:eta:phi:bx="<< test->ptValue()<<":"<<test->etaValue()<<":"<<test->phiValue()<<":"<<test->bx()<<endl; 
		}
        }*/

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
        dphiVtxQ = d1.charge() * deltaPhi(d1.phi(), d2.phi());
        drVtx[i] = deltaR(d1,d2);
        drVt=deltaR(d1,d2);
        TagCharge = d1.charge();
        ProbeCharge = d2.charge();
        TagMu_pt = d1.pt();
        TagMu_phi = d1.phi();
        TagMu_eta = d1.eta();
        ProbeMu_pt = d2.pt();
        ProbeMu_phi = d2.phi();
        ProbeMu_eta = d2.eta();

        const reco::RecoCandidate *mu1 = dynamic_cast<const reco::RecoCandidate *>(&*d1.masterClone());
        const reco::RecoCandidate *mu2 = dynamic_cast<const reco::RecoCandidate *>(&*d2.masterClone());
        if (mu1 == 0) throw cms::Exception("CorruptData") << "First daughter of candidate is not a ShallowClone of a reco::RecoCandidate\n";
        if (mu2 == 0) throw cms::Exception("CorruptData") << "Second daughter of candidate is not a ShallowClone of a reco::RecoCandidate\n";
        
        const pat::GenericParticle * gp = dynamic_cast<const pat::GenericParticle*>(&*d2.masterClone());
        bool has = gp->hasUserCand("l1MuTkMatch");
        //cout<< has<<endl;
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
					dR_RPC = deltaR(gmt.etaValue(), gmt.phiValue(),reg.etaValue(),reg.phiValue());
					RPC_pt       = reg.ptValue();
					RPC_eta      = reg.etaValue();
					RPC_phi      = reg.phiValue();
				} else {
					RPC_pt       = -999;
					RPC_eta      = -999;
					RPC_phi      = -999;
					dR_RPC       =  999;
					
				}
				//if(!regDT.empty()) cout<<" REGDT pt:eta:phi:bx="<< regDT.ptValue()<<":"<<regDT.etaValue()<<":"<<regDT.phiValue()<<":"<<regDT.bx()<<endl;
				if (!regDT.empty()){
					dR_DT = deltaR(gmt.etaValue(), gmt.phiValue(),regDT.etaValue(),regDT.phiValue());
					DT_pt       = regDT.ptValue();
					DT_eta      = regDT.etaValue();
					DT_phi      = regDT.phiValue();
					DT_fine 	 = regDT.isFineHalo();
				} else { 
					dR_DT = 999;
					DT_pt       = -999;
					DT_eta      = -999;
					DT_phi      = -999;
					DT_fine		  = false;
				}
				//if(!regCSC.empty()) cout<<" REGCSC pt:eta:phi:bx="<< regCSC.ptValue()<<":"<<regCSC.etaValue()<<":"<<regCSC.phiValue()<<":"<<regCSC.bx()<<endl;
				if (!regCSC.empty()){
					dR_CSC = deltaR(gmt.etaValue(), gmt.phiValue(),regCSC.etaValue(),regCSC.phiValue());
					CSC_pt       = regCSC.ptValue();
					CSC_eta      = regCSC.etaValue();
					CSC_phi      = regCSC.phiValue();
				} else { 
					dR_CSC = 999;
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
				dR_RPC       =  999;
				dR_DT        =  999;
				dR_CSC        =  999;
				DT_pt        = -999;
				DT_eta       = -999;
				DT_phi       = -999;
				CSC_pt        = -999;
				CSC_eta       = -999;
				CSC_phi       = -999;
				DT_fine		  = false;
        	}

        } else{
			L1E_pt       = -999;
			L1E_eta      = -999;
			L1E_phi      = -999;
			RPC_pt       = -999;
			RPC_eta      = -999;
			RPC_phi      = -999;
			dR_RPC       =  999;
			dR_DT        =  999;
			dR_CSC        =  999;
			DT_pt        = -999;
			DT_eta       = -999;
			DT_phi       = -999;
			CSC_pt        = -999;
			CSC_eta       = -999;
			CSC_phi       = -999;
			DT_fine		  = false;
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
            TagMu_eta_propM1 = prop1_M1.globalPosition().eta();
            TagMu_phi_propM1 = prop1_M1.globalPosition().phi();
            ProbeMu_eta_propM1 = prop2_M1.globalPosition().eta();
            ProbeMu_phi_propM1 = prop2_M1.globalPosition().phi();


        }
        else {
            TagMu_eta_propM1 = -999;
            TagMu_phi_propM1 = -999;
            ProbeMu_eta_propM1 = -999;
            ProbeMu_phi_propM1 = -999;
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

            TagMu_eta_propM2 = prop1_M2.globalPosition().eta();
            TagMu_phi_propM2 = prop1_M2.globalPosition().phi();
            ProbeMu_eta_propM2 = prop2_M2.globalPosition().eta();
            ProbeMu_phi_propM2 = prop2_M2.globalPosition().phi();


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
                if (gmt.phiValue()+1.25 * 3.14/180.-ProbeMu_phi_propM2<3.14) dPhi = gmt.phiValue()+1.25 * 3.14/180.-ProbeMu_phi_propM2;
                else dPhi = gmt.phiValue()+1.25 * 3.14/180.-ProbeMu_phi_propM2-6.28;
                dPhi_MuL1 = dPhi;
                dEta_MuL1 = gmt.etaValue()-ProbeMu_eta_propM2;
                dR_MuL1 = deltaR(gmt.etaValue(), gmt.phiValue()+1.25 * 3.14/180.,ProbeMu_eta_propM2,ProbeMu_phi_propM2);
            } else {
                dPhi_MuL1 = -999;
                dEta_MuL1 = -999;
                dR_MuL1 = -999;
            }

        }
        else {
            TagMu_eta_propM2 = -999;
            TagMu_phi_propM2 = -999;
            ProbeMu_eta_propM2 = -999;
            ProbeMu_phi_propM2 = -999;
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
