#ifndef L1Trigger_L1MuonParticle_h
#define L1Trigger_L1MuonParticle_h
// -*- C++ -*-
//
// Package:     L1Trigger
// Class  :     L1MuonParticle
// 
/**\class L1MuonParticle \file L1MuonParticle.h DataFormats/L1Trigger/interface/L1MuonParticle.h \author Werner Sun

 Description: L1Extra particle class for muon objects, containing Regional candidates for the purpose of RPC Trigger T&P efficiency analysis.
*/
//
// Original Author:  Werner Sun
//         Created:  Sat Jul 15 12:41:07 EDT 2006
// $Id: L1MuonParticle.h,v 1.1 2011/12/14 17:29:28 rybinska Exp $
//
// system include files

// user include files
#include "DataFormats/Candidate/interface/LeafCandidate.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutCollection.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTReadoutRecord.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuGMTExtendedCand.h"
#include "DataFormats/L1GlobalMuonTrigger/interface/L1MuRegionalCand.h"

// forward declarations

namespace l1extra {

   class L1MuonParticle : public reco::LeafCandidate
   {

      public:
	 L1MuonParticle();

	 // Eventually, all L1MuGMTCands will be L1MuGMTExtendedCands,
	 // as soon as dictionaries for them exist in
	 // L1Trigger/GlobalMuonTrigger.
        
	 L1MuonParticle( Charge q,
			 const LorentzVector& p4,
			 const L1MuGMTExtendedCand& aCand,
			 int bx = 0,
			 bool isRPC = 0,
			 bool isDT = 0,
			 bool isCSC = 0 ) ;
         // Use for quality 4, 5 and 6
	 L1MuonParticle( Charge q, 
			 const PolarLorentzVector& p4,
			 const L1MuGMTExtendedCand& aCand,
			 const L1MuRegionalCand& rCand,
			 int bx = 0,
			 bool isRPC = 0,
			 bool isDT = 0,			 			 
			 bool isCSC =0) ;

         // Use for quality 7
	 L1MuonParticle( Charge q, 
			 const PolarLorentzVector& p4,
			 const L1MuGMTExtendedCand& aCand,
			 const L1MuRegionalCand& dCand,
			 const L1MuRegionalCand& rCand,
			 int bx = 0,
			 bool isRPC = 0,			 
			 bool isDT = 0,			 
			 bool isCSC =0) ;			 

	 L1MuonParticle( Charge q, 
			 const PolarLorentzVector& p4,
			 const L1MuGMTExtendedCand& aCand,
			 int bx = 0,
			 bool isRPC = 0,
			 bool isDT = 0,			 			 
			 bool isCSC =0) ;


     // Creates null Ref.
         L1MuonParticle( Charge q,
                         const LorentzVector& p4,
                         bool isolated = false,
                         bool mip = false,
			 bool forward = false,
			 bool rpc = false,
			 unsigned int detector = 0,
			 int bx = 0,
			 bool isRPC = 0,			 
			 bool isDT = 0,			 
			 bool isCSC =0 ) ;

         L1MuonParticle( Charge q,
                         const PolarLorentzVector& p4,
                         bool isolated = false,
                         bool mip = false,
			 bool forward = false,
			 bool rpc = false,
			 unsigned int detector = 0,
			 int bx = 0,
			 bool isRPC = 0,
			 bool isDT = 0,			 			 
			 bool isCSC =0) ;

	 virtual ~L1MuonParticle() {}

	 // ---------- const member functions ---------------------
         bool isIsolated() const
         { return isolated_ ; }

         bool isMip() const
         { return mip_ ; }

	 bool isForward() const
	 { return forward_ ; }

	 bool isRPC() const
	 { return rpc_ ; }

	 const L1MuGMTExtendedCand& gmtMuonCand() const
	 { return cand_ ; }

	 const L1MuRegionalCand& regRPCMuonCand() const
	 { return regRPCcand_ ; }

	 const L1MuRegionalCand& regDTMuonCand() const
	 { return regDTcand_ ; }
	 
	 const L1MuRegionalCand& regCSCMuonCand() const
	 { return regCSCcand_ ; }

	 virtual L1MuonParticle* clone() const
	 { return new L1MuonParticle( *this ) ; }

	 int bx() const
	 { return bx_ ; }
        // quality 5 or 7
	 bool isRPCCand() const
	 { return isRPC_; }
	 // quality 6 or 7, detector 2 or 3
	 bool isDTCand() const
	 { return isDT_; }
	 // quality 6 or 7, detector 4 or 5
	 bool isCSCCand() const
	 { return isCSC_; }

	 // ---------- static member functions --------------------

	 // ---------- member functions ---------------------------
	 void setIsolated( bool isIso )
	 { isolated_ = isIso ; }

	 void setMip( bool isMip )
	 { mip_ = isMip ; }

	 void setForward( bool isForward )
	 { forward_ = isForward ; }

	 void setRPC( bool isRPC )
	 { rpc_ = isRPC ; }
	 
	 void setBx( int bx )
	 { bx_ = bx ; }
	 
	 void setIsRPC( bool isRPC )
	 { isRPC_ = isRPC ; }
	 
	 void setIsDT( bool isDT )
	 { isDT_ = isDT ; }
	 
	 void setIsCSC( bool isCSC )
	 { isCSC_ = isCSC ; }



      private:
	 // L1MuonParticle(const L1MuonParticle&); // stop default

	 // const L1MuonParticle& operator=(const L1MuonParticle&); // stop default

	 // ---------- member data --------------------------------
         bool isolated_ ;
         bool mip_ ;
	 bool forward_ ;
	 bool rpc_ ;
	 L1MuGMTExtendedCand cand_ ;
	 L1MuRegionalCand regRPCcand_;
	 L1MuRegionalCand regDTcand_;
	 L1MuRegionalCand regCSCcand_;
	 int bx_ ;
	 bool isRPC_;
	 bool isDT_;
	 bool isCSC_;
   };
}

#endif
