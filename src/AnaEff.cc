#include "UserCode/L1RpcTriggerAnalysis/interface/AnaEff.h"
#include "TProfile.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include <math.h>
#include <vector>
#include <sstream>

const double AnaEff::ptCuts[ AnaEff::nPtCuts] = { 0., 5., 10., 16., 30., 100.};


AnaEff::AnaEff(TObjArray& histos)
  : debug(false)
{
  hEfficMuPt_D = new TH1D("hEfficMuPt_D","hEfficMuPt_D", L1PtScale::nPtBins, L1PtScale::ptBins); histos.Add(hEfficMuPt_D);
  hEfficRpcNoCut_N = new TH1D("hEfficRpcNoCut_N","hEfficRpcNoCut_N", L1PtScale::nPtBins, L1PtScale::ptBins);  histos.Add(hEfficRpcNoCut_N);
  hEfficRpcPtCut_N = new TH1D("hEfficRpcPtCut_N","hEfficRpcPtCut_N", L1PtScale::nPtBins, L1PtScale::ptBins);  histos.Add(hEfficRpcPtCut_N);

  std::string  base("hEff");
  std::string reg[3]={"_Bar","_Int","_End"};
  std::string opt[2]={"_RpcPtCut","_OthPtCut"};
  for (unsigned int ir=0; ir<3; ++ir) {
    std::string name=base+"_PtDenom"+reg[ir];
    TH1D *h= new TH1D(name.c_str(),name.c_str(), L1PtScale::nPtBins, L1PtScale::ptBins);
    histos.Add(h); hm[name]=h;
    for (unsigned int iopt=0; iopt<2; ++iopt) {
    for (unsigned int icut=0; icut<AnaEff::nPtCuts; ++icut) {
      std::stringstream str;
      str << base << opt[iopt] << ptCuts[icut]<<reg[ir];
      TH1D *h= new TH1D(str.str().c_str(),str.str().c_str(), L1PtScale::nPtBins, L1PtScale::ptBins);
      h->SetXTitle("muon p_{T} [GeV/c]  "); h->SetYTitle("events");
      histos.Add(h); hm[str.str()]=h;
    }
    }
  }
  for (unsigned int icut=0; icut<AnaEff::nPtCuts; ++icut) {
      std::stringstream strEtaDenom; strEtaDenom << base << "_EtaDenom"<< ptCuts[icut];
      TH1D *hD = new TH1D(strEtaDenom.str().c_str(),strEtaDenom.str().c_str(), L1RpcEtaScale::nEtaBins, L1RpcEtaScale::etaBins);
      hD->SetXTitle("muon pseudorapidity"); hD->SetYTitle("events"); histos.Add(hD); hm[strEtaDenom.str()]=hD;
      std::stringstream strEtaCut  ; strEtaCut << base << "_EtaCut"<< ptCuts[icut];
      TH1D *hN = new TH1D(strEtaCut.str().c_str(),strEtaCut.str().c_str(), L1RpcEtaScale::nEtaBins, L1RpcEtaScale::etaBins);
      hN->SetXTitle("muon pseudorapidity"); hN->SetYTitle("events"); histos.Add(hN); hm[strEtaCut.str()]=hN;
  }

}

double AnaEff::maxPt(const std::vector<L1Obj> & l1Objs) const
{
  double result = -1.;
  for (unsigned int i=0; i<l1Objs.size(); i++) if (l1Objs[i].pt > result) result = l1Objs[i].pt;
  return result; 
}

void AnaEff::run( const MuonObj *muon, const L1ObjColl *l1RpcColl, const L1ObjColl *l1OtherColl)
{
  double etaMu = fabs(muon->eta());
  double ptMu  = muon->pt();   
  if (!muon->isGlobal()) return;

//  if (ptMu < 6.) return;
//  if (ptMu > 7.) return;

  //std::vector<L1Obj> l1Rpcs = l1RpcColl->getL1ObjsSelected(true, false, 0., 161., 0,0, -1.6, 1.6,  -1.,7., -1,7 );
  std::vector<L1Obj> l1Rpcs = l1RpcColl->getL1ObjsSelected();
  //std::vector<L1Obj> l1Oths = l1OtherColl->getL1ObjsSelected();
  std::vector<L1Obj> l1Oths = l1OtherColl->getL1ObjsSelected();


  hEfficMuPt_D->Fill(ptMu); 
  if (maxPt(l1Rpcs) > 0.) hEfficRpcNoCut_N->Fill(ptMu);
  if (maxPt(l1Rpcs) >= 15.999) hEfficRpcPtCut_N->Fill(ptMu);

  unsigned int iregion;
  if (etaMu < 0.83) iregion = 0;
  else if (etaMu < 1.24) iregion = 1;
  else iregion = 2;
  std::string reg[3]={"_Bar","_Int","_End"};

  hm["hEff_PtDenom"+reg[iregion]]->Fill(ptMu);
  double epsilon=1.e-5;
  for (unsigned int icut=0; icut < AnaEff::nPtCuts; icut++) { 
    double threshold = AnaEff::ptCuts[icut];
    std::stringstream strEtaDenom;  strEtaDenom  << "hEff_EtaDenom"<<  ptCuts[icut];
    if (ptMu >= threshold) hm[strEtaDenom.str()]->Fill(muon->eta()); 
    if (maxPt(l1Rpcs)+epsilon > threshold) {
       std::stringstream strPt;  strPt  << "hEff_RpcPtCut"<<  ptCuts[icut]<<reg[iregion];
       hm[strPt.str()]->Fill(ptMu);
       std::stringstream strEtaCut;  strEtaCut  << "hEff_EtaCut"<<  ptCuts[icut];
       if (ptMu >= threshold) hm[strEtaCut.str()]->Fill(muon->eta()); 
    }
    if (maxPt(l1Oths)+epsilon > threshold) {
       std::stringstream strPt;  strPt  << "hEff_OthPtCut"<<  ptCuts[icut]<<reg[iregion];
       hm[strPt.str()]->Fill(ptMu);
    }
  }
  


}	
