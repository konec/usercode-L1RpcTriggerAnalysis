#include "FilterOrbit.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
FilterOrbit::FilterOrbit(const edm::ParameterSet& iConfig):
m_minOrbit( iConfig.getParameter<int>("minOrbit") ),
m_maxOrbit( iConfig.getParameter<int>("maxOrbit") ),
m_orbPassed(0)
{
   //now do what ever initialization is needed

}


FilterOrbit::~FilterOrbit()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
bool
FilterOrbit::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   static int orbit;

  orbit = iEvent.orbitNumber();

  bool ret = (orbit>=m_minOrbit && orbit<=m_maxOrbit  );
  if (ret) {
    LogDebug("FilterOrbit") << "Passed: " <<  m_orbPassed++ << std::endl;
  }

  return ret;

}

// ------------ method called once each job just before starting event loop  ------------
void 
FilterOrbit::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
FilterOrbit::endJob() {
}

