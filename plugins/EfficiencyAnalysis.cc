#include "EfficiencyAnalysis.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/EventObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/TrackObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/L1ObjColl.h"


#include <vector>
#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TProfile.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TGraphErrors.h"


using namespace std;
template <class T> T sqr( T t) {return t*t;}

void EfficiencyAnalysis::beginJob()
{
  TObjArray histos;
  histos.SetOwner();

  // RECO muon kinematics (Reco::MuonCollection "muons", after cuts: track, vertex, pt, eta, RPC,CSC,DT hits)   
  TGraphErrors * hGraphLumi = new TGraphErrors();
  TGraphErrors * hGraphRun = new TGraphErrors(); 

  // variable-size ETA bins corresponding to PAC trigger granularity
  const Int_t nEtaBins=33;
  Double_t EtaBins[nEtaBins+1] = {
    -2.10, -1.97, -1.85, -1.73, -1.61, -1.48, -1.36, -1.24, -1.14, 
    -1.04, -0.93, -0.83, -0.72, -0.58, -0.44, -0.27, -0.07,
    0.07, 0.27, 0.44, 0.58, 0.72, 0.83, 0.93, 1.04, 1.14,
    1.24, 1.36, 1.48, 1.61, 1.73, 1.85, 1.97, 2.10
  };

  // variable-size PT bins corresponding to L1 trigger scale. 
  // Lower bound of bin ipt corresponds to L1Pt(ipt),
  // with exception of ipt=0 and 1 (which should be NAN and 0.)
  const Int_t nPtBins=32;
  Double_t PtBins[nPtBins+1] = {
   0., 0.1, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 6., 7., 8., 10., 
  12., 14., 16., 18., 20., 25., 30., 35., 40., 45., 50., 60., 70., 80., 90., 100., 120., 140., 
  150.   
  };


  TH1D * hMuonPt  = new TH1D("hMuonPt","All global muons Pt;Glb.muon p_{T} [GeV];Muons / bin",nPtBins,PtBins);  histos.Add(hMuonPt);
  TH1D * hMuonEta = new TH1D("hMuonEta","All global muons Eta;Glb.muon #eta;Muons / bin",64, -1.6, 1.6);  histos.Add(hMuonEta);
  TH1D * hMuonPhi = new TH1D("hMuonPhi","All global muons Phi;Glb.muon #phi [rad];Muons / bin",90,-M_PI,M_PI);  histos.Add(hMuonPhi);

  // RECO track kinematics (Reco::TrackCollection "generalTracks", track matched to RECO muon above)
  TH1D * hTrackPt  = new TH1D("hTrackPt","L1Muon-matched track Pt;Track p_{T} [GeV];Tracks / bin",nPtBins,PtBins);  histos.Add(hTrackPt);
  TH1D * hTrackEta = new TH1D("hTrackEta","L1Muon-matched track Eta;Track #eta;Tracks / bin",64, -1.6, 1.6);  histos.Add(hTrackEta);
  TH1D * hTrackPhi = new TH1D("hTrackPhi","L1Muon-matched track Phi;Track #phi [rad];Tracks / bin",90,-M_PI,M_PI);  histos.Add(hTrackPhi);


  // hit histos
  // barrel |eta|<0,8, endcap 1.25<|eta|<1.55 
  TH1D *hEfficDetB_N[6],  *hEfficHitB_N[6], *hEfficDetE_N[3], *hEfficHitE_N[3]; 
  TH1D *hEfficHitDet_D= new TH1D( "hEfficHitDet_D", "hEfficHitDet_D", 64, -1.6, 1.6);  histos.Add( hEfficHitDet_D);
  for (unsigned int i=1; i<=6; ++i) {
    std::stringstream name;
    name.str(""); name<<"hEfficDetB_N"<< i; hEfficDetB_N[i-1]= new TH1D( name.str().c_str(), name.str().c_str(), 64, -1.6, 1.6);  histos.Add( hEfficDetB_N[i-1]);
    name.str(""); name<<"hEfficHitB_N"<< i; hEfficHitB_N[i-1]= new TH1D( name.str().c_str(), name.str().c_str(), 64, -1.6, 1.6);  histos.Add( hEfficHitB_N[i-1]);
    if (i<=3) {
    name.str(""); name <<"hEfficDetE_N"<< i; hEfficDetE_N[i-1]= new TH1D( name.str().c_str(), name.str().c_str(), 64, -1.6, 1.6);  histos.Add( hEfficDetE_N[i-1]);
    name.str(""); name <<"hEfficHitE_N"<< i; hEfficHitE_N[i-1]= new TH1D( name.str().c_str(), name.str().c_str(), 64, -1.6, 1.6);  histos.Add( hEfficHitE_N[i-1]);
    }
  }

  TH1D * hHitsB = new TH1D("hHitsB","Layers with RPC hits matching propagated muon - Barrel;No of layers;Muons",7,-0.5,6.5);  histos.Add(hHitsB);
  TH1D * hHitsE = new TH1D("hHitsE","Layers with RPC hits matching propagated muon - Endcap;No of layers;Muons",4,-0.5,3.5);  histos.Add(hHitsE);
  TH1D * hDetsB_100 = new TH1D("hDetsB_100","Layers with RPCs crossed by propagated muon - Barrel;No of layers;Muons",7,-0.5,6.5);  histos.Add(hDetsB_100);
  TH1D * hDetsE_100 = new TH1D("hDetsE_100","Layers with RPCs crossed by propagated muon - Endcap;No of layers;Muons",4,-0.5,3.5);  histos.Add(hDetsE_100);

  // efficiency for hits
  // barrel |eta|<0,8, endcap 1.25<|eta|<1.55 
  TH1D * hEfficGeom_M = new TH1D("hEfficGeom_M","Propaged muons matching RPC Geom;Glb.muon #eta;Muons /bin", 64, -1.6, 1.6);  histos.Add(hEfficGeom_M);
  TH1D * hEfficGeom_H = new TH1D("hEfficGeom_H","Propaged muons matching RPC Geom;Glb.muon #eta;Muons / bin", 64, -1.6, 1.6);  histos.Add(hEfficGeom_H);
  TH1D * hEfficGeom_D = new TH1D("hEfficGeom_D","RPC triggers;Glb.muon #eta;Muons / bin",64, -1.6, 1.6);  histos.Add(hEfficGeom_D);
  TH1D * hEfficGeom_T = new TH1D("hEfficGeom_T","Propaged muons crossing RPCs;Glb.muon #eta;Muons / bin",64, -1.6, 1.6);  histos.Add(hEfficGeom_T);


  // in order to get numbewr of crossed layers
  TH1D * hEfficChambBar_N = new TH1D("hEfficChambBar_N","Propaged muons matching RPC hits - Barrel;Layer;Muons",6,0.5,6.5); histos.Add(hEfficChambBar_N); 
  TH1D * hEfficChambBar_D = new TH1D("hEfficChambBar_D","Propaged muons crossing RPCs - Barrel;Layer;Muons",6,0.5,6.5); histos.Add(hEfficChambBar_D); 
  TH1D * hEfficChambEnd_N = new TH1D("hEfficChambEnd_N","Propaged muons matching RPC hits - Endcap;Layer;Muons",3,0.5,3.5); histos.Add(hEfficChambEnd_N);
  TH1D * hEfficChambEnd_D = new TH1D("hEfficChambEnd_D","Propaged muons crossing RPCs - Endcap;Layer;Muons",3,0.5,3.5); histos.Add(hEfficChambEnd_D); 

  // efficiency for trigger wrt Mu 
  TH1D * hEfficMu_N =  new TH1D("hEfficMu_N","hEfficMu_N",64, -1.6, 1.6);  histos.Add(hEfficMu_N);
  TH1D * hEfficMu_D =  new TH1D("hEfficMu_D","hEfficMu_D",64, -1.6, 1.6);  histos.Add(hEfficMu_D);

  // efficiency for trigger wrt L1Other
  TH1D * hEfficTk_N =  new TH1D("hEfficTk_N","hEfficTk_N",64, -1.6, 1.6);  histos.Add(hEfficTk_N);
  TH1D * hEfficTk_D  = new TH1D("hEfficTk_D","hEfficTk_D",64, -1.6, 1.6);  histos.Add(hEfficTk_D);

// efficiency for  L1rpc vs Pt 
//  TH1D * hEfficMuPt7_N = new TH1D("hEfficMuPt7_N","hEfficMuPt7_N",50,6.,156.);  histos.Add(hEfficMuPt7_N);
//  TH1D * hEfficMuPt_N = new TH1D("hEfficMuPt_N","hEfficMuPt_N",50,6.,156.);  histos.Add(hEfficMuPt_N);
//  TH1D * hEfficMuPt_D = new TH1D("hEfficMuPt_D","hEfficMuPt_D",50,6.,156.);  histos.Add(hEfficMuPt_D);

   TH1D * hEfficMuPt7_N = new TH1D("hEfficMuPt7_N","hEfficMuPt7_N",nPtBins,PtBins);  histos.Add(hEfficMuPt7_N);
   TH1D * hEfficMuPt_N = new TH1D("hEfficMuPt_N","hEfficMuPt_N",nPtBins,PtBins);  histos.Add(hEfficMuPt_N);
   TH1D * hEfficMuPt_D = new TH1D("hEfficMuPt_D","hEfficMuPt_D",nPtBins,PtBins);  histos.Add(hEfficMuPt_D);

//   // efficiency for  L1rpc vs Pt 
//   TH1D * hEfficTkPt7_N = new TH1D("hEfficTkPt7_N","hEfficTkPt7_N",75,0.,150.);  histos.Add(hEfficTkPt7_N);
//   TH1D * hEfficTkPt_N = new TH1D("hEfficTkPt_N","hEfficTkPt_N",75,0.,150.);  histos.Add(hEfficTkPt_N);
//   TH1D * hEfficTkPt_D = new TH1D("hEfficTkPt_D","hEfficTkPt_D",75,0.,150.);  histos.Add(hEfficTkPt_D);


  // efficiency for  L1rpc vs Pt 
  TH1D * hEfficTkPt7_N = new TH1D("hEfficTkPt7_N","hEfficTkPt7_N",50,5.,55.);  histos.Add(hEfficTkPt7_N);
  TH1D * hEfficTkPt_N = new TH1D("hEfficTkPt_N","hEfficTkPt_N",50,5.,55.);  histos.Add(hEfficTkPt_N);
  TH1D * hEfficTkPt_D = new TH1D("hEfficTkPt_D","hEfficTkPt_D",50,5.,55.);  histos.Add(hEfficTkPt_D);



  TH2D* hDistL1Rpc   = new TH2D("hDistL1Rpc","All L1 RPC candidates (#phi,#eta);L1 RPC #eta;L1 RPC #phi [rad];Muons / bin",
                                nEtaBins,EtaBins,144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(hDistL1Rpc);  
  TH2D* hDistL1Other = new TH2D("hDistL1Other","All L1 DT/CSC candidates (#phi,#eta);L1 obj #eta;L1 obj #phi [rad];DT/CSC muons / bin",
                                nEtaBins,EtaBins,144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(hDistL1Other);

  TH1D* hEffLumi = new TH1D("hEffLumi","Efficiency per LS;Efficiency per LS;No of LS / bin",100,0.,1.); histos.Add(hEffLumi);
  TH1D* hEffRun = new TH1D("hEffRun","Efficiency per run;Efficiency per run;No of runs / bin",100,0.,1.); histos.Add(hEffRun);

  // timing of L1 RPC trigger candidates
  TH1D* hL1RpcBX = new TH1D("hL1RpcBX","First L1 RPC candidate BX wrt L1;First L1 RPC wrt L1 [BX];Muons / BX",5,-2.5,2.5); histos.Add(hL1RpcBX);
  TH1D* hL1RpcBX_all = new TH1D("hL1RpcBX_all","All L1 RPC candidates BX wrt L1;L1 RPC wrt L1 [BX];Muons / BX",5,-2.5,2.5); histos.Add(hL1RpcBX_all);

  TH1D* hL1RpcGBX = new TH1D("hL1RpcGBX","First L1 RPC candidate BX;Event.bx + 1st L1RPC.bx;Muons / BX",4000,-0.5,4000.-0.5); histos.Add(hL1RpcGBX);
  TH1D* hL1RpcGBX_all = new TH1D("hL1RpcGBX_all","All L1 RPC candidates BX;Event.bx + L1RPC.bx;Muons / BX",4000,-0.5,4000.-0.5); histos.Add(hL1RpcGBX_all);
  TH1D* hMuGBX = new TH1D("hMuGBX","Glb.muon BX;Event BX;Muons / BX",4000,-0.5,4000.-0.5); histos.Add(hMuGBX);

  // L1 RPC trigger candidates eta, phi
  TH1D* hL1RpcEta = new TH1D("hL1RpcEta","All L1 RPC candidates #eta;L1 RPC #eta;Muons / bin",nEtaBins,EtaBins); histos.Add(hL1RpcEta);
  TH1D* hL1RpcPhi = new TH1D("hL1RpcPhi","All L1 RPC candidates #phi;L1 RPC #phi [rad];Muons / bin",144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(hL1RpcPhi);

  TH1D* hL1RpcEta_q0 = new TH1D("hL1RpcEta_q0","All L1 RPC candidates #eta, q0;L1 RPC #eta;Muons_{q0} / bin",nEtaBins,EtaBins); histos.Add(hL1RpcEta_q0);
  TH1D* hL1RpcPhi_q0 = new TH1D("hL1RpcPhi_q0","All L1 RPC candidates #phi, q0;L1 RPC #phi [rad];Muons_{q0} / bin",144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(hL1RpcPhi_q0);
  TH1D* hL1RpcEta_q1 = new TH1D("hL1RpcEta_q1","All L1 RPC candidates #eta, q1;L1 RPC #eta;Muons_{q1} / bin",nEtaBins,EtaBins); histos.Add(hL1RpcEta_q1);
  TH1D* hL1RpcPhi_q1 = new TH1D("hL1RpcPhi_q1","All L1 RPC candidates #phi, q1;L1 RPC #phi [rad];Muons_{q1} / bin",144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(hL1RpcPhi_q1);
  TH1D* hL1RpcEta_q2 = new TH1D("hL1RpcEta_q2","All L1 RPC candidates #eta, q2;L1 RPC #eta;Muons_{q2} / bin",nEtaBins,EtaBins); histos.Add(hL1RpcEta_q2);
  TH1D* hL1RpcPhi_q2 = new TH1D("hL1RpcPhi_q2","All L1 RPC candidates #phi, q2;L1 RPC #phi [rad];Muons_{q2} / bin",144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(hL1RpcPhi_q2);
  TH1D* hL1RpcEta_q3 = new TH1D("hL1RpcEta_q3","All L1 RPC candidates #eta, q3;L1 RPC #eta;Muons_{q3} / bin",nEtaBins,EtaBins); histos.Add(hL1RpcEta_q3);
  TH1D* hL1RpcPhi_q3 = new TH1D("hL1RpcPhi_q3","All L1 RPC candidates #phi, q3;L1 RPC #phi [rad];Muons_{q3} / bin",144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(hL1RpcPhi_q3);

  TH2D* h2L1RpcBX   = new TH2D("h2L1RpcBX","All L1 RPC candidates (#eta,#phi);L1 RPC #eta;L1 RPC #phi [rad];Muons / bin",nEtaBins,EtaBins,144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(h2L1RpcBX);
  TH2D* h2L1RpcBX_q0   = new TH2D("h2L1RpcBX_q0","All L1 RPC candidates (#eta,#phi), q0;L1 RPC #eta;L1 RPC #phi [rad];Muons_{q0} / bin",nEtaBins,EtaBins,144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(h2L1RpcBX_q0);
  TH2D* h2L1RpcBX_q1   = new TH2D("h2L1RpcBX_q1","All L1 RPC candidates (#eta,#phi), q1;L1 RPC #eta;L1 RPC #phi [rad];Muons_{q1} / bin",nEtaBins,EtaBins,144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(h2L1RpcBX_q1);
  TH2D* h2L1RpcBX_q2   = new TH2D("h2L1RpcBX_q2","All L1 RPC candidates (#eta,#phi), q2;L1 RPC #eta;L1 RPC #phi [rad];Muons_{q2} / bin",nEtaBins,EtaBins,144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(h2L1RpcBX_q2);
  TH2D* h2L1RpcBX_q3   = new TH2D("h2L1RpcBX_q3","All L1 RPC candidates (#eta,#phi), q3;L1 RPC #eta;L1 RPC #phi [rad];Muons_{q3} / bin",nEtaBins,EtaBins,144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(h2L1RpcBX_q3);

  // L1 RPC trigger candidates at BX<>0 wrt L1
  TH2D* h2L1RpcBX1   = new TH2D("h2L1RpcBX1","All L1 RPC candidates (#eta,#phi) @ BX<>0;L1 RPC #eta;L1 RPC #phi [rad];Muons_{BX<>0} / bin",nEtaBins,EtaBins,144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(h2L1RpcBX1);
  TH2D* h2L1RpcBX1_q0   = new TH2D("h2L1RpcBX1_q0","All L1 RPC candidates (#eta,#phi), q0 @ BX<>0;L1 RPC #eta;L1 RPC #phi [rad];Muons_{q0 BX<>0} / bin",nEtaBins,EtaBins,144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(h2L1RpcBX1_q0);
  TH2D* h2L1RpcBX1_q1   = new TH2D("h2L1RpcBX1_q1","All L1 RPC candidates (#eta,#phi), q1 @ BX<>0;L1 RPC #eta;L1 RPC #phi [rad];Muons_{q1 BX<>0} / bin",nEtaBins,EtaBins,144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(h2L1RpcBX1_q1);
  TH2D* h2L1RpcBX1_q2   = new TH2D("h2L1RpcBX1_q2","All L1 RPC candidates (#eta,#phi), q2 @ BX<>0;L1 RPC #eta;L1 RPC #phi [rad];Muons_{q2 BX<>0} / bin",nEtaBins,EtaBins,144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(h2L1RpcBX1_q2);
  TH2D* h2L1RpcBX1_q3   = new TH2D("h2L1RpcBX1_q3","All L1 RPC candidates (#eta,#phi), q3 @ BX<>0;L1 RPC #eta;L1 RPC #phi [rad];Muons_{q3 BX<>0} / bin",nEtaBins,EtaBins,144,-0.2e-3,2*M_PI-0.2e-3); histos.Add(h2L1RpcBX1_q3);

  // average efficiency per LumiSection
  typedef std::map< std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> > EffLumiMap;
  EffLumiMap effLumiMap;

  // average efficiency per Run
  typedef std::map< unsigned int, std::pair<unsigned int, unsigned int> > EffRunMap;
  EffRunMap effRunMap;

  // define chain
  TChain chain("tL1Rpc");
  std::vector<std::string> treeFileNames = theConfig.getParameter<std::vector<std::string> >("treeFileNames");
  for (std::vector<std::string>::const_iterator it = treeFileNames.begin(); it != treeFileNames.end(); ++it)  chain.Add((*it).c_str() );

  std::vector<bool> *hitBarrel = 0;
  std::vector<bool> *hitEndcap = 0;
  std::vector<unsigned int> *detBarrel = 0;
  std::vector<unsigned int> *detEndcap = 0;

//  std::vector<unsigned int> *validPRCEndcap = 0;
//  std::vector<unsigned int> *validDTEndcap = 0;
//  std::vector<unsigned int> *validCSCEndcap = 0;


  EventObj * event = 0;
  TrackObj * track = 0;
  MuonObj * muon = 0;

  TBranch *bhitBarrel=0;
  TBranch *bhitEndcap=0;
  TBranch *bdetBarrel=0;
  TBranch *bdetEndcap=0;

  L1ObjColl* l1RpcColl = 0;
  L1ObjColl* l1OtherColl = 0;
  
  chain.SetBranchAddress("event",&event);

  chain.SetBranchAddress("muon",&muon);
  chain.SetBranchAddress("track",&track);

  chain.SetBranchAddress("hitBarrel",&hitBarrel, &bhitBarrel);
  chain.SetBranchAddress("hitEndcap",&hitEndcap, &bhitEndcap);
  chain.SetBranchAddress("detBarrel",&detBarrel, &bdetBarrel);
  chain.SetBranchAddress("detEndcap",&detEndcap, &bdetEndcap);

  chain.SetBranchAddress("l1RpcColl",&l1RpcColl);
  chain.SetBranchAddress("l1OtherColl",&l1OtherColl);

  Int_t nentries = (Int_t) chain.GetEntries();
  std::cout <<" ENTRIES: " << nentries << std::endl;
  std::cout <<"CUTS:  pt_reco: "<<theConfig.getParameter<double>("ptMin")
	    <<"  pt_l1Cut "<<theConfig.getParameter<double>("l1Cut")
	    << std::endl;


    std::cout << " WARNING ============= REJECTED RUNS: 147155 146417 146421 ==============" << std::endl; 


  for (int ev=0; ev<nentries; ev++) {
    chain.GetEntry(ev);

      
  // reject wrong runs:
  if(event->run == 147115 || event->run == 146417 || event->run == 146421 ) continue;





    if (effLumiMap.find( std::make_pair(event->run, event->lumi)) == effLumiMap.end()) 
        effLumiMap[ make_pair( event->run, event->lumi)] = make_pair(0,0);
    if (effRunMap.find(event->run) == effRunMap.end()) 
        effRunMap[event->run] = make_pair(0,0);

    if (   !muon->isGlobal() 
        || !muon->isTracker()
        || !muon->isOuter()
        || muon->pt()<theConfig.getParameter<double>("ptMin") 
        || fabs(muon->eta()) > 1.6 
        ) continue;

    hMuGBX->Fill(event->bx);
    std::vector<L1Obj> l1Rpcs = l1RpcColl->getL1ObjsMatched(); 

    //
    // MAIN RPC EFFICIENCY HISTORGAMS
    //
    bool toto = false;
    bool isMuon = (muon->pt() > theConfig.getParameter<double>("ptMin") );
    if (isMuon) {
      //if (fabs(muon.eta) > 0.8) continue;
      //if (fabs(muon.eta) < 1.24) continue;

      hMuonPt->Fill(muon->pt());
      hMuonEta->Fill(muon->eta());
      hMuonPhi->Fill(muon->phi());
      hEfficMu_D->Fill(muon->eta());
      if (l1Rpcs.size()) hEfficMu_N->Fill(muon->eta()); 

      hEfficMuPt_D->Fill(muon->pt());
      if (l1Rpcs.size())  hEfficMuPt_N->Fill(muon->pt());
      if (l1RpcColl->getL1ObjsMatched(theConfig.getParameter<double>("l1Cut")).size()) hEfficMuPt7_N->Fill(muon->pt()); 
    }
    //
    // L1RPC EFFICIENCY AS FUNCTION OF RUN/LUMI
    //
    effLumiMap[make_pair(event->run,event->lumi)].second++;
    effRunMap[event->run].second++;
    if (l1Rpcs.size() > 0) {
      effLumiMap[make_pair(event->run,event->lumi)].first++;
      effRunMap[event->run].first++;
    }
    //(float) rand()/RAND_MAX << std::endl;


    //
    // MUON HITS AND CROSSED DETS ANALYSIS
    //
    if (isMuon) {

      int nHitsB = 0; for (int i=0; i<6; i++) if( hitBarrel->at(i) ) nHitsB++;
      int nHitsBL= 0; for (int i=0; i<4; i++) if( hitBarrel->at(i) ) nHitsBL++;
      int nHitsE = 0; for (int i=0; i<3; i++) if( hitEndcap->at(i) ) nHitsE++;

      int nDetsB = 0;  for (int i=0; i<6; i++) if( detBarrel->at(i) ) nDetsB++;
      int nDetsBL= 0;  for (int i=0; i<4; i++) if( detBarrel->at(i) ) nDetsBL++;
      int nDetsE = 0;  for (int i=0; i<3; i++) if( detEndcap->at(i) ) nDetsE++;

      hEfficHitDet_D->Fill(muon->eta());
      for (int i=0; i<6;++i) {
        if (detBarrel->at(i)) hEfficChambBar_D->Fill(i+1.);
	  if (hitBarrel->at(i)) hEfficChambBar_N->Fill(i+1.);
        if (detBarrel->at(i)) hEfficDetB_N[i]->Fill(muon->eta());
        if (hitBarrel->at(i)) hEfficHitB_N[i]->Fill(muon->eta());
      }
      for (int i=0; i<3;++i) {
        if (detEndcap->at(i)) hEfficChambEnd_D->Fill(i+1.);
	  if (hitEndcap->at(i)) hEfficChambEnd_N->Fill(i+1.);
        if (detEndcap->at(i)) hEfficDetE_N[i]->Fill(muon->eta());
        if (hitEndcap->at(i)) hEfficHitE_N[i]->Fill(muon->eta());
      }

      // pure Barrel
      if (fabs(muon->eta()) < 0.8) {
        hHitsB->Fill(nHitsB);
        hDetsB_100->Fill(nDetsB);
        hEfficGeom_M->Fill(muon->eta());      
        if (nDetsBL>=3 || (nDetsB>=4 && (detBarrel->at(4)||detBarrel->at(5)) ) ) hEfficGeom_D->Fill(muon->eta());
        if (nHitsBL>=3 || (nHitsB>=4 && (hitBarrel->at(4)||hitBarrel->at(5)) ) ) {
          hEfficGeom_H->Fill(muon->eta());
          if (l1Rpcs.size()>0) hEfficGeom_T->Fill(muon->eta());
        } else if (l1Rpcs.size()>0) { toto=true; std::cout <<"nHitsBL: " << nHitsBL<<" nHitsB: "<< nHitsB<< std::endl; }
      }
      // pure Endcap
      if (fabs(muon->eta()) > 1.25  && fabs(muon->eta()) < 1.6) { 
        hHitsE->Fill(nHitsE);
        hDetsE_100->Fill(nDetsE);
        hEfficGeom_M->Fill(muon->eta());      
        if (nDetsE>=3) hEfficGeom_D->Fill(muon->eta());
        if (nHitsE>=3) {
          hEfficGeom_H->Fill(muon->eta());
          if (l1Rpcs.size()) hEfficGeom_T->Fill(muon->eta());
        }
        if (l1Rpcs.size()>0 && nHitsE!=3) { toto=true; std::cout <<"NHITS E: " << nHitsE<< std::endl; }
      }
    }

    //
    // TIMING
    // 
    if (l1Rpcs.size() > 0) {
      int firstBX = 100;
      for (std::vector<L1Obj>::const_iterator it=l1Rpcs.begin(); it!= l1Rpcs.end(); ++it) {

        if ( (it->bx) < firstBX) { firstBX = it->bx; }

        hL1RpcBX_all->Fill(it->bx);
        hL1RpcEta->Fill(it->eta);
        hL1RpcPhi->Fill(it->phi);
        h2L1RpcBX->Fill(it->eta,it->phi);
        hL1RpcGBX_all->Fill(event->bx+it->bx); // global bunch crossing number of L1RPC candidate

        if (it->q == 0){ hL1RpcEta_q0->Fill(it->eta); hL1RpcPhi_q0->Fill(it->phi);h2L1RpcBX_q0->Fill(it->eta,it->phi); }
        if (it->q == 1){ hL1RpcEta_q1->Fill(it->eta); hL1RpcPhi_q1->Fill(it->phi);h2L1RpcBX_q1->Fill(it->eta,it->phi); }
        if (it->q == 2){ hL1RpcEta_q2->Fill(it->eta); hL1RpcPhi_q2->Fill(it->phi);h2L1RpcBX_q2->Fill(it->eta,it->phi); }
        if (it->q == 3){ hL1RpcEta_q3->Fill(it->eta); hL1RpcPhi_q3->Fill(it->phi);h2L1RpcBX_q3->Fill(it->eta,it->phi); }

        // off peak BX-es ?
        if (it->bx){
          h2L1RpcBX1->Fill(it->eta,it->phi);
          if (it->q == 0) h2L1RpcBX1_q0->Fill( it->eta, it->phi);
          if (it->q == 1) h2L1RpcBX1_q1->Fill( it->eta, it->phi);
          if (it->q == 2) h2L1RpcBX1_q2->Fill( it->eta, it->phi);
          if (it->q == 3) h2L1RpcBX1_q3->Fill( it->eta, it->phi);
        }
      }
      // the earliest candidate
      hL1RpcBX->Fill(firstBX);
      hL1RpcGBX->Fill(event->bx+firstBX); // global bunch crossing number of L1RPC candidate

      //############################################
      // TRAP for PRE-FIRED/POST-FIRED RPC TRIGGERS
      //############################################
     
    //  if (firstBX !=0) {
      if (toto) {
        std::cout <<" event: " << ev << std::endl;
        if (firstBX < 0) std::cout<< "RPC Pre-firing! ";
        if (firstBX > 0) std::cout<< "RPC Post-firing! ";
        std::cout<< " Run:"<< event->run
                 << " Evt:"<<event->id
                 << " LumiSec: "<< event->lumi
                 << " Time: "<< event->time
                 << " Orbit: "<< event->orbit
                 << " BX:"<< event->bx
                 << " / "
                 << " size_L1RPC: "<< l1Rpcs.size() 
                 << " first_L1RPC_BX: " << firstBX
                 << std::endl;
      } 
    }


 
    //
    // ADDITIONAL HISTOGRAMS WITH TRACK AS DENOM.
    //
    bool isTrack = (track->pt() > theConfig.getParameter<double>("ptMin") );
    if (isTrack) {
      //if (fabs(track.eta) > 0.8) continue;
      //if (fabs(track.eta) < 1.24) continue;
      hTrackPt->Fill(track->pt());
      hTrackEta->Fill(track->eta());
      hTrackPhi->Fill(track->phi());

      hEfficTk_D->Fill(track->eta());
      if (l1Rpcs.size()) hEfficTk_N->Fill(track->eta()); 

      hEfficTkPt_D->Fill(track->pt());
      if (l1Rpcs.size())     hEfficTkPt_N->Fill(track->pt());
      if (l1RpcColl->getL1ObjsMatched(theConfig.getParameter<double>("l1Cut")).size()) hEfficTkPt7_N->Fill(track->pt()); 
    }

    // L1 RPC candidates
    if (l1Rpcs.size()) hDistL1Rpc->Fill(l1Rpcs[0].eta, l1Rpcs[0].phi); // TODO: why [0] ??
    // L1 DT/CSC candidate
    std::vector<L1Obj> l1Others = l1OtherColl->getL1Objs(); 
    if (l1Others.size()) hDistL1Other->Fill(l1Others[0].eta, l1Others[0].phi); // TODO: why [0] ??

  } // end of event loop


  /*
  //
  // SUMMARIES RUN/LUMI EFFIC INTO GRAPH
  // average efficiency per LumiSection
  hGraphLumi->Set(effLumiMap.size());
  unsigned int iPoint=0;
  for( EffLumiMap::const_iterator im = effLumiMap.begin(); im != effLumiMap.end(); ++im) {
    float eff = 0.;
    if (im->second.second != 0) eff = float(im->second.first)/float(im->second.second);
    float effErr = sqrt( (1-eff)*im->second.first)/im->second.second; 
//     std::cout <<" RUN: "<<im->first.first
//               <<" LS: "<<im->first.second
//               <<" Effic: "<< eff <<" ("<<im->second.first<<"/"<<im->second.second<<")"<<std::endl; 
    hEffLumi->Fill(eff);
    hGraphLumi->SetPoint(iPoint, im->first.first + im->first.second*0.001, eff);
    hGraphLumi->SetPointError(iPoint, 0., effErr);
    iPoint++;
  } 

  */


  // average efficiency per Lumi
  int nPoints = 0; 
  for( EffLumiMap::const_iterator im = effLumiMap.begin(); im != effLumiMap.end(); ++im) {
    //reject wrong runs
    if(im->first.first == 147115. || im->first.first == 146417. || im->first.first == 146421. ) continue;
    
  if (im->second.first != 0) ++nPoints; 
  }
  hGraphLumi->Set(nPoints);

  int  iPoint=0;
  for( EffLumiMap::const_iterator im = effLumiMap.begin(); im != effLumiMap.end(); ++im) {
    //reject wrong runs
     if(im->first.first == 147115. || im->first.first == 146417. || im->first.first == 146421. ) continue;
     
    float eff = 0.;
    if (im->second.first==0 ) continue;
    if (im->second.second != 0) eff = float(im->second.first)/float(im->second.second); 
    float effM1 = float(im->second.first-1)/float(im->second.second);
    float effErr = sqrt( (1-effM1)*std::max((int) im->second.first,1))/im->second.second; 
    hEffLumi->Fill(eff, 1./sqr(effErr));
    hGraphLumi->SetPoint(iPoint, im->first.first + im->first.second*0.001, eff);
    hGraphLumi->SetPointError(iPoint, 0., effErr);
    iPoint++;
  } 

 
  // average efficiency per Run
  nPoints = 0; 
  for( EffRunMap::const_iterator im = effRunMap.begin(); im != effRunMap.end(); ++im){
  //reject wrong runs
    if(im->first == 147115. || im->first == 146417 || im->first == 146421 ) continue;
    if (im->second.first != 0) ++nPoints; 
  }
  hGraphRun->Set(nPoints);

  iPoint=0;
  for( EffRunMap::const_iterator im = effRunMap.begin(); im != effRunMap.end(); ++im) {

    //reject wrong runs
    if(im->first == 147115 || im->first == 146417 || im->first == 146421 ) continue;
  

    float eff = 0.;
    if (im->second.first==0 ) continue;
    if (im->second.second != 0) eff = float(im->second.first)/float(im->second.second); 
    float effM1 = float(im->second.first-1)/float(im->second.second);
    float effErr = sqrt( (1-effM1)*std::max((int) im->second.first,1))/im->second.second; 
    std::cout <<" RUN: "<<im->first
              <<" Effic: "<< eff <<" ("<<im->second.first<<"/"<<im->second.second<<")"<<std::endl; 
    hEffRun->Fill(eff, 1./sqr(effErr));
    hGraphRun->SetPoint(iPoint, im->first, eff);
    hGraphRun->SetPointError(iPoint, 0., effErr);
    iPoint++;
  } 

  //  chain.ResetBranchAddresses(); 
  std::string histoFile = theConfig.getParameter<std::string>("histoFileName");
  TFile f(histoFile.c_str(),"RECREATE");
  histos.Write();
  hGraphLumi->Write("hGraphLumi");
  hGraphRun->Write("hGraphRun");
  f.Close();
 
  cout <<"KUKU"<<endl;
}
