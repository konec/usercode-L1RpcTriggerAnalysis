//
// $Id: L1MuonMatcher.cc,v 1.4 2011/03/31 09:59:33 gpetrucc Exp $
//

/**
  \class    pat::L1MuonMatcher L1MuonMatcher.h "MuonAnalysis/MuonAssociators/interface/L1MuonMatcher.h"
  \brief    Matcher of reconstructed objects to L1 Muons 
            
  \author   Giovanni Petrucciani
  \version  $Id: L1MuonMatcher.cc,v 1.4 2011/03/31 09:59:33 gpetrucc Exp $
*/
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/Ptr.h"
#include "DataFormats/Common/interface/View.h"

#include "MuonAnalysis/MuonAssociators/interface/L1MuonMatcherAlgo.h"

#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "CommonTools/UtilAlgos/interface/MasterCollectionHelper.h"
#include "CommonTools/UtilAlgos/interface/MatchByDR.h"
#include "CommonTools/UtilAlgos/interface/DeltaR.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <algorithm>

namespace pat {
  class L1MuonMatcher : public edm::EDProducer {
    public:
      explicit L1MuonMatcher(const edm::ParameterSet & iConfig);
      virtual ~L1MuonMatcher() { }

      virtual void produce(edm::Event & iEvent, const edm::EventSetup & iSetup);

      virtual void beginRun(edm::Run & iRun, const edm::EventSetup & iSetup);

    private:
      typedef pat::TriggerObjectStandAlone           PATPrimitive;
      typedef pat::TriggerObjectStandAloneCollection PATPrimitiveCollection;
      typedef pat::TriggerObjectStandAloneMatch      PATTriggerAssociation;

      L1MuonMatcherAlgo matcher_;

      /// Labels for input collections
      edm::InputTag reco_, l1_;

      /// Labels to set as filter names in the output
      std::string labelL1_, labelProp_;

      /// Write out additional info as ValueMaps
      bool writeExtraInfo_, resolveAmbiguitiesByMatchQuality_;
      std::string sortByRes_;

      /// Store extra information in a ValueMap
      template<typename Hand, typename T>
      void storeExtraInfo(edm::Event &iEvent, 
                     const Hand & handle,
                     const std::vector<T> & values,
                     const std::string    & label) const ;

      struct comparatorStruct {
        bool operator() (std::pair<size_t, float> i, std::pair<size_t, float> j) {return (i.second < j.second);}
      } comparator;
  };

}

pat::L1MuonMatcher::L1MuonMatcher(const edm::ParameterSet & iConfig) :
    matcher_(iConfig),
    reco_(iConfig.getParameter<edm::InputTag>("src")),
    l1_(iConfig.getParameter<edm::InputTag>("matched")),
    labelL1_(iConfig.getParameter<std::string>("setL1Label")),
    labelProp_(iConfig.getParameter<std::string>("setPropLabel")),
    writeExtraInfo_(iConfig.getParameter<bool>("writeExtraInfo")),
    resolveAmbiguitiesByMatchQuality_(iConfig.getParameter<bool>("resolveAmbiguitiesByMatchQuality")),
    sortByRes_(iConfig.getParameter<std::string>("sortByRes"))
{
    produces<PATPrimitiveCollection>("l1muons");        // l1 in PAT format
    produces<PATPrimitiveCollection>("propagatedReco"); // reco to muon station 2
    produces<PATTriggerAssociation>("propagatedReco");  // asso reco to propagated reco
    produces<PATTriggerAssociation>();                  // asso reco to l1
    if (writeExtraInfo_) {
        produces<edm::ValueMap<float> >("deltaR");
        produces<edm::ValueMap<float> >("deltaPhi");
        produces<edm::ValueMap<float> >("deltaEta");
        produces<edm::ValueMap<int>   >("quality");
        produces<edm::ValueMap<int>   >("bx");
        produces<edm::ValueMap<int>   >("isolated");
        produces<edm::ValueMap<reco::CandidatePtr> >();
        produces<edm::ValueMap<reco::CandidatePtr> >("l1ToReco");
    }
}

void 
pat::L1MuonMatcher::produce(edm::Event & iEvent, const edm::EventSetup & iSetup) {
    using namespace edm;
    using namespace std;
    //cout << "BEGIN EVENT, coll: "<<reco_<<endl;

    Handle<View<reco::Candidate> > reco;

    Handle<vector<l1extra::L1MuonParticle> > l1s;

    iEvent.getByLabel(reco_, reco);
    iEvent.getByLabel(l1_, l1s);
    //cout << "RECO size = "<<reco->size()<<endl;
    auto_ptr<PATPrimitiveCollection> propOut(new PATPrimitiveCollection());
    auto_ptr<PATPrimitiveCollection> l1Out(new PATPrimitiveCollection());
    std::vector<edm::Ptr<reco::Candidate> > l1rawMatches(reco->size());
    std::vector<TrajectoryStateOnSurface> props(reco->size());
    vector<int> isSelected(l1s->size(), -1);
    vector<bool> lLock(l1s->size(),false);

    std::vector<edm::Ptr<reco::Candidate> > whichRecoMatch(l1s->size());
    vector<int>   propMatches(reco->size(), -1);
    vector<int>   fullMatches(reco->size(), -1);
    vector<float> deltaRs(reco->size(), 999), deltaPhis(reco->size(), 999), deltaEtas(reco->size(), 999);
    vector<int>   quality(reco->size(),   0), bx(reco->size(), -999), isolated(reco->size(), -999);
    if(!resolveAmbiguitiesByMatchQuality_){
		for (int i = 0, n = reco->size(); i < n; ++i) {
			//cout <<"Begin Reco" <<endl;
			TrajectoryStateOnSurface propagated;
			const reco::Candidate &mu = (*reco)[i];
			int match = matcher_.match(mu, *l1s, deltaRs[i], deltaPhis[i], deltaEtas[i], propagated);
			//cout << "Old: i = "<<i<<" match = "<< match<< " deltaRs[i] = "<< deltaRs[i] << " deltaPhis[i] = " <<deltaPhis[i] << " Prop Valid? "<< propagated.isValid() <<endl;
			if (propagated.isValid()) {
				//cout <<"Begin Prop" <<endl;
				GlobalPoint pos = propagated.globalPosition();
				propMatches[i] = propOut->size();
				propOut->push_back(PATPrimitive(math::PtEtaPhiMLorentzVector(mu.pt(), pos.eta(), pos.phi(), mu.mass())));
				propOut->back().addFilterLabel(labelProp_);
				propOut->back().setCharge(mu.charge());
				//cout << "Prop pt:eta:phi = "<<mu.pt() <<" : "<<pos.eta()<<" : "<<pos.phi()<<endl;
			}
			if (match != -1) {
				//cout <<"Begin Match" <<endl;
				const l1extra::L1MuonParticle & l1 = (*l1s)[match];
				whichRecoMatch[match] = reco->ptrAt(i);
				if (isSelected[match] == -1) { // copy to output if needed
					isSelected[match] = l1Out->size();
					l1Out->push_back(PATPrimitive(l1.polarP4()));
					l1Out->back().addFilterLabel(labelL1_);
					l1Out->back().setCharge(l1.charge());
				}
				fullMatches[i] = isSelected[match]; // index in the output collection
				const L1MuGMTCand & gmt = l1.gmtMuonCand();
				quality[i]  = gmt.quality();
				bx[i]       = gmt.bx();
				isolated[i] = gmt.isol();
				//cout << "Match pt:eta:phi = "<<l1.pt() <<" : "<<l1.eta()<<" : "<<l1.phi()<<endl;
				l1rawMatches[i] = edm::Ptr<reco::Candidate>(l1s, size_t(match));

			}
		}
	}
	if(resolveAmbiguitiesByMatchQuality_){

	    vector<pair<size_t, int> > matchPairsSort;
	    vector<pair<size_t, int> > matchPairsFinal;
	    vector<pair<size_t, float> > sortHelper;

		for (unsigned i = 0, n = reco->size(); i < n; ++i) {
			//cout <<"Begin Reco" <<endl;
			//matchPairsFinal.push_back(make_pair(i,-1));
			TrajectoryStateOnSurface propagated;
			const reco::Candidate &mu = (*reco)[i];
			int match = matcher_.match(mu, *l1s, deltaRs[i], deltaPhis[i], deltaEtas[i], propagated);

			if (propagated.isValid()) {
	            GlobalPoint pos = propagated.globalPosition();
	            propMatches[i] = propOut->size();
	            propOut->push_back(PATPrimitive(math::PtEtaPhiMLorentzVector(mu.pt(), pos.eta(), pos.phi(), mu.mass())));
	            propOut->back().addFilterLabel(labelProp_);
	            propOut->back().setCharge(mu.charge());
	            //cout << "Prop pt:eta:phi = "<<mu.pt() <<" : "<<pos.eta()<<" : "<<pos.phi()<<endl;
			}
			matchPairsSort.push_back(make_pair(i,match));
			matchPairsFinal.push_back(make_pair(i,-1));

			if (sortByRes_ == "deltaR"){
				if (match != -1){
					sortHelper.push_back(make_pair(i,deltaRs[i]));
					//cout<<"Pushed back = (" << i<<", "<<match<<")"<<endl;
				}
				else {
					sortHelper.push_back(make_pair(i,999));
					//cout<<"Pushed back = (" << i<<", "<<999<<")"<<endl;
				}
			}
			if (sortByRes_ == "deltaPhi"){
				if (match != -1){
					sortHelper.push_back(make_pair(i,deltaRs[i]));
					//cout<<"Pushed back = (" << i<<", "<<match<<")"<<endl;
				}
				else {
					sortHelper.push_back(make_pair(i,999));
					//cout<<"Pushed back = (" << i<<", "<<999<<")"<<endl;
				}
			}
			if (sortByRes_ == "deltaEta"){
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
		/*cout << "matchPairsSort size = "<< matchPairsSort.size() <<"  Content: ";
		for (unsigned a = 0; a < matchPairsSort.size();a++) cout << " ("<<matchPairsSort[a].first <<", " <<matchPairsSort[a].second<<") "<<endl;
		cout << "sortHelper size = "<< sortHelper.size() <<"  Content: ";
		for (unsigned a = 0; a < sortHelper.size();a++) cout << " ("<<sortHelper[a].first <<", " <<sortHelper[a].second<<") "<<endl;
		cout << "matchPairsFinal size = "<< matchPairsFinal.size() <<"  Content: ";
		for (unsigned a = 0; a < matchPairsFinal.size();a++) cout << " ("<<matchPairsFinal[a].first <<", " <<matchPairsFinal[a].second<<") "<<endl;
		*/

		std::sort(sortHelper.begin(),sortHelper.end(),comparator);

		//cout << "After sort: sortHelper size = "<< sortHelper.size() <<"  Content: ";
		//for (unsigned a = 0; a < sortHelper.size();a++) cout << " ("<<sortHelper[a].first <<", " <<sortHelper[a].second<<") "<<endl;

		for (unsigned i = 0, n = sortHelper.size(); i < n; ++i) {
			//cout <<"i = " <<i <<endl;
			int recoIndex = matchPairsSort[sortHelper[i].first].first;
			//if (sortHelper[i].second==999) break;
			//cout <<"sortHelper[i].first "<<sortHelper[i].first<<" Reco Index = "<<recoIndex<<" dR = "<< sortHelper[i].second;

			int l1Index = matchPairsSort[sortHelper[i].first].second;
			if(l1Index!=-1){
				//cout <<" l1 Index = "<<l1Index<<endl;
				if (!lLock[l1Index]){
					lLock[l1Index] = true;
					matchPairsFinal[recoIndex].second = l1Index;
				}
			}
 		}
		//cout <<"End Match" <<endl;
		for (unsigned i = 0, n = matchPairsFinal.size(); i < n; ++i) {
			int recoIndex = matchPairsFinal[i].first;
			int l1Index = matchPairsFinal[i].second;
			if (l1Index != -1) {
				const l1extra::L1MuonParticle & l1 = (*l1s)[l1Index];
				whichRecoMatch[l1Index] = reco->ptrAt(recoIndex);
				isSelected[l1Index] = l1Out->size();
				l1Out->push_back(PATPrimitive(l1.polarP4()));
				l1Out->back().addFilterLabel(labelL1_);
				l1Out->back().setCharge(l1.charge());
				fullMatches[recoIndex] = isSelected[l1Index]; // index in the output collection
				const L1MuGMTCand & gmt = l1.gmtMuonCand();
				quality[recoIndex]  = gmt.quality();
				bx[recoIndex]       = gmt.bx();
				isolated[recoIndex] = gmt.isol();
				l1rawMatches[recoIndex] = edm::Ptr<reco::Candidate>(l1s, size_t(l1Index));
				//cout << "Match pt:eta:phi = "<<l1.pt() <<" : "<<l1.eta()<<" : "<<l1.phi()<<endl;
			} else{
				deltaRs[recoIndex] = 999;
				deltaPhis[recoIndex] = 999;
			}
			//cout << "New: i = "<<i<<" match = "<< l1Index<< " deltaRs[i] = "<< deltaRs[i] << "deltaPhis[i] = " <<deltaPhis[i] <<endl;

		}
	}
    OrphanHandle<PATPrimitiveCollection> l1Done = iEvent.put(l1Out, "l1muons");
    OrphanHandle<PATPrimitiveCollection> propDone = iEvent.put(propOut, "propagatedReco");

    auto_ptr<PATTriggerAssociation> propAss(new PATTriggerAssociation(propDone));
    PATTriggerAssociation::Filler propFiller(*propAss);
    propFiller.insert(reco, propMatches.begin(), propMatches.end());
    propFiller.fill();
    iEvent.put(propAss, "propagatedReco");

    auto_ptr<PATTriggerAssociation> fullAss(new PATTriggerAssociation(  l1Done));
    PATTriggerAssociation::Filler fullFiller(*fullAss);
    fullFiller.insert(reco, fullMatches.begin(), fullMatches.end());
    fullFiller.fill();
    iEvent.put(fullAss);

    if (writeExtraInfo_) {
        storeExtraInfo(iEvent, reco, deltaRs,   "deltaR");
        storeExtraInfo(iEvent, reco, deltaPhis, "deltaPhi");
        storeExtraInfo(iEvent, reco, deltaPhis, "deltaEta");
        storeExtraInfo(iEvent, reco, bx,        "bx");
        storeExtraInfo(iEvent, reco, isolated,  "isolated");
        storeExtraInfo(iEvent, reco, quality,   "quality");
        storeExtraInfo(iEvent, reco, l1rawMatches,   "");
        storeExtraInfo(iEvent, l1s,  whichRecoMatch, "l1ToReco");
    }
}

template<typename Hand, typename T>
void
pat::L1MuonMatcher::storeExtraInfo(edm::Event &iEvent,
                     const Hand & handle,
                     const std::vector<T> & values,
                     const std::string    & label) const {
    using namespace edm; using namespace std;
    auto_ptr<ValueMap<T> > valMap(new ValueMap<T>());
    typename edm::ValueMap<T>::Filler filler(*valMap);
    filler.insert(handle, values.begin(), values.end());
    filler.fill();
    iEvent.put(valMap, label);
}


void 
pat::L1MuonMatcher::beginRun(edm::Run & iRun, const edm::EventSetup & iSetup) {
    matcher_.init(iSetup);
}


#include "FWCore/Framework/interface/MakerMacros.h"
using namespace pat;
DEFINE_FWK_MODULE(L1MuonMatcher);
