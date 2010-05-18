#ifndef FilterOrbit_H
#define FilterOrbit_H
// -*- C++ -*-
//
// Package:    FilterOrbit
// Class:      FilterOrbit
//
/**\class FilterOrbit FilterOrbit.cc MyAna/FilterOrbit/src/FilterOrbit.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Tomasz Maciej Frueboes
//         Created:  Sat Jan 24 15:11:35 CET 2009
// $Id: FilterOrbit.h,v 1.1 2010/01/14 15:29:01 konec Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//
// class declaration
//

class FilterOrbit : public edm::EDFilter {
   public:
      explicit FilterOrbit(const edm::ParameterSet&);
      ~FilterOrbit();

   private:
      virtual void beginJob(const edm::EventSetup&) ;
      virtual bool filter(edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;
      int m_minOrbit;
      int m_maxOrbit;
      int m_orbPassed;
      // ----------member data ---------------------------
};

#endif
