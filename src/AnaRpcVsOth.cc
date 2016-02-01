#include "UserCode/L1RpcTriggerAnalysis/interface/AnaRpcVsOth.h"
#include "TProfile.h"
#include "TObjArray.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/MuonObj.h"
#include "DataFormats/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include "DataFormats/L1RpcTriggerAnalysis/interface/L1ObjColl.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Utilities.h"
#include <math.h>
#include <vector>
#include <sstream>

const double AnaRpcVsOth::binsEff[ AnaRpcVsOth::nBinsEff+1 ] = { 1., 5., 12., 16., 30., 100.,1000.};

void AnaRpcVsOth::init(TObjArray& histos)
{
  std::string  base("hRpcVsOth_");
  std::string tag[7]={"AnyPt","MinPt","MaxPt","RanPt","OthPt","RpcPt","AlgPt"};
  std::string reg[3]={"_Bar","_Int","_End"};
  std::string opt[5]={"Mu","RpcPtCut","OthPtCut","RpcNoCut","OthNoCut"};
  for (unsigned int ir=0; ir<3; ++ir) {
    // statistics by algorithms 
    for (unsigned int it=0; it<7; ++it) {
      std::string name=base+"Events"+tag[it]+reg[ir];
      TH1D *h= new TH1D(name.c_str(),name.c_str(), AnaRpcVsOth::nBinsEff, AnaRpcVsOth::binsEff);
      h->SetXTitle("global muon p_{T} [GeV/c]  "); h->SetYTitle("events");
      histos.Add(h);  
      h1[name]=h; 
    }
    // qality for events discarded 
    for (unsigned int it=0; it<7; ++it) {
      std::string name=base+"QualEvLostBy"+tag[it]+reg[ir];
      TH2D *h= new TH2D(name.c_str(),name.c_str(), 4,-0.5,3.5, 8,-0.5,7.5);
      h->SetXTitle("quality RPC"); h->SetYTitle("quality Other");
      histos.Add(h);  
      h2[name]=h; 
    }
   // coarse efficiency
    for (unsigned int io=0; io<5; ++io) {
      std::string name=base+"EffCoarse"+opt[io]+reg[ir];
      TH1D *h= new TH1D(name.c_str(),name.c_str(), AnaRpcVsOth::nBinsEff, AnaRpcVsOth::binsEff);
      h->SetXTitle("global muon p_{T} [GeV/c]  "); h->SetYTitle("events");
      histos.Add(h); h1[name]=h;
    }
  } 
}

double AnaRpcVsOth::maxPt(const std::vector<L1Obj> & l1Objs) const
{
  double result = 0.;
  for (unsigned int i=0; i<l1Objs.size(); i++) if (l1Objs[i].pt > result) result = l1Objs[i].pt;
  return result; 
}

void AnaRpcVsOth::run( const TrackObj *muon, const L1ObjColl *l1Coll)
{
  
  double etaMu = fabs(muon->eta());
  double ptMu  = muon->pt();   
//  if (!muon->isGlobal()) return;

  static double matchingdR = theConfig.getParameter<double>("maxDR"); 
  std::vector<L1Obj> l1Rpcs = l1Coll->l1RpcColl().selectByDeltaR(matchingdR).selectByBx(0,0).getL1Objs();
  std::vector<L1Obj> l1Oths = l1Coll->l1OthColl().selectByDeltaR(matchingdR).selectByBx(0,0).selectByEta(-1.61,1.61).getL1Objs();

  unsigned int iregion;
  if (etaMu < 0.83) iregion = 0;
  else if (etaMu < 1.24) iregion = 1;
  else iregion = 2;  
  std::string reg[3]={"_Bar","_Int","_End"};



  std::string  base("hRpcVsOth_");
  double thr = 15.99;
  //
  // efficiencies
  //
  if (maxPt(l1Rpcs) > 0.)  h1[ base+"EffCoarseRpcNoCut"+reg[iregion] ]->Fill(ptMu);
  if (maxPt(l1Rpcs) > thr) h1[ base+"EffCoarseRpcPtCut"+reg[iregion] ]->Fill(ptMu);
  if (maxPt(l1Oths) > 0.)  h1[ base+"EffCoarseOthNoCut"+reg[iregion] ]->Fill(ptMu);
  if (maxPt(l1Oths) > thr) h1[ base+"EffCoarseOthPtCut"+reg[iregion] ]->Fill(ptMu);
                           h1[ base+"EffCoarseMu"      +reg[iregion] ]->Fill(ptMu);

  //
  // relative histos
  //
  if (l1Rpcs.size() != 1 || l1Oths.size()  != 1 ) return; 

  const L1Obj & rpc =  l1Rpcs[0]; 
  const L1Obj & oth =  l1Oths[0]; 

  double rankpt = 0.;
  if (oth.q > rpc.q) rankpt = oth.pt;
  else if ( rpc.q > oth.q) rankpt = rpc.pt;
  else rankpt = std::max(rpc.pt,oth.pt);  

/*
  double algpt = oth.pt;
  if (rpc.q >=2) algpt = rpc.pt;
  if (rpc.q==1) algpt = std::max(rpc.pt,oth.pt); 
*/

/*
  double algpt = oth.pt;
  if (2*rpc.q+1 > oth.q) algpt = rpc.pt;
  else if (2*rpc.q+1 < oth.q) algpt = oth.pt;
  else algpt = std::max(rpc.pt,oth.pt); 
*/

/*
  double algpt = oth.pt;
  if (rpc.q+2 > oth.q) algpt = rpc.pt;
  else if (2+rpc.q < oth.q) algpt = oth.pt;
  else algpt = std::min(rpc.pt,oth.pt); 
*/
  double algpt = oth.pt;
  if (iregion==0) {
    if (rpc.q>=2) algpt = rpc.pt;
    else if (oth.q>=3) algpt = oth.pt;
    else algpt = std::min( rpc.pt, oth.pt); 
/*
    if (rpc.q>= 2 && oth.q >4) algpt = std::min( rpc.pt, oth.pt);
    else if (rpc.q>=2) algpt = rpc.pt;
    else if (oth.q>=2) algpt = oth.pt;
    else algpt = std::min( rpc.pt, oth.pt); 
*/
/*
    if (rpc.q==3) algpt = rpc.pt;
    else if (rpc.q==2 && oth.q >3)  algpt = std::max( rpc.pt, oth.pt);
    else if (rpc.q==1 && oth.q >4)  algpt = std::min( rpc.pt, oth.pt);
    else if (rpc.q==0 && oth.q >3)  algpt = std::min( rpc.pt, oth.pt);
    else if (oth.q >2) algpt = oth.pt; 
    else     algpt = rpc.pt;
*/
/*
    if (rpc.q==3 && oth.q > 3) algpt = std::max(rpc.pt,oth.pt);
    else if (rpc.q==3) algpt = rpc.pt;
    else if (rpc.q==2 && oth.q >3)  algpt = std::max( rpc.pt, oth.pt);
    else if (rpc.q==1 && oth.q >4)  algpt = std::min( rpc.pt, oth.pt);
    else if (rpc.q==0 && oth.q >3)  algpt = std::min( rpc.pt, oth.pt);
    else if (oth.q >2) algpt = oth.pt; 
    else     algpt = rpc.pt;
*/
  } else if (iregion==1) {
    algpt= oth.pt;
  } else {
    if (oth.q > 1) algpt = std::min( rpc.pt, oth.pt);
    else algpt = rpc.pt;
  }

  double maxpt =  std::max(rpc.pt, oth.pt);
  double minpt =  std::min(rpc.pt, oth.pt);

                     h1[ base+"EventsAnyPt"+reg[iregion] ]->Fill(ptMu);
  if ( minpt  > thr) h1[ base+"EventsMinPt"+reg[iregion] ]->Fill(ptMu);
  if ( maxpt  > thr) h1[ base+"EventsMaxPt"+reg[iregion] ]->Fill(ptMu);
  if ( rankpt > thr) h1[ base+"EventsRanPt"+reg[iregion] ]->Fill(ptMu);
  if ( rpc.pt > thr) h1[ base+"EventsRpcPt"+reg[iregion] ]->Fill(ptMu);
  if ( oth.pt > thr) h1[ base+"EventsOthPt"+reg[iregion] ]->Fill(ptMu);
  if ( algpt  > thr) h1[ base+"EventsAlgPt"+reg[iregion] ]->Fill(ptMu); 
  
  //
  // quality of candidates for events lost
  //
  if (ptMu > 30.) {
    if ( minpt  < thr) h2[ base+"QualEvLostByMinPt"+reg[iregion] ]->Fill(rpc.q, oth.q);
    if ( maxpt  < thr) h2[ base+"QualEvLostByMaxPt"+reg[iregion] ]->Fill(rpc.q, oth.q);
    if ( rpc.pt < thr) h2[ base+"QualEvLostByRpcPt"+reg[iregion] ]->Fill(rpc.q, oth.q);
    if ( oth.pt < thr) h2[ base+"QualEvLostByOthPt"+reg[iregion] ]->Fill(rpc.q, oth.q);
    if ( algpt  < thr) h2[ base+"QualEvLostByAlgPt"+reg[iregion] ]->Fill(rpc.q, oth.q);
  }
}	

