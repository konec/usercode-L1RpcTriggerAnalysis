#ifndef OMTF_OMTFROOTReader_H
#define OMTF_OMTFROOTReader_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "L1Trigger/L1TMuon/interface/OMTFConfiguration.h"

#include "TObjArray.h"

class OMTFProcessor;
class OMTFConfiguration;
class OMTFConfigMaker;
class OMTFinputMaker;
class OMTFSorter;

class AnaEff;
class AnaSiMuDistribution;
class XMLConfigWriter;

namespace edm { 
  class Event; 
  class EventSetup; 
}

class OMTFROOTReader: public edm::EDAnalyzer {

public:

  OMTFROOTReader(const edm::ParameterSet & cfg);
  virtual ~OMTFROOTReader();
  virtual void beginJob();
  virtual void beginRun(const edm::Run&,  const edm::EventSetup& es){};
  virtual void analyze(const edm::Event&, const edm::EventSetup& es);
  virtual void endJob();

private:

  std::vector<std::pair<uint32_t, uint32_t> > filterDigis(std::vector<std::pair<uint32_t, uint32_t> > *digSpec);

  void analyseConnections(unsigned int iProcessor,
			  unsigned int iCone);

  TObjArray myHistos;
  edm::ParameterSet theConfig;

  bool dumpResultToXML, dumpGPToXML;
  bool makeConnectionsMaps;

  ////
  XMLConfigWriter *myWriter; 

  ///OMTF objects
  OMTFConfiguration *myOMTFConfig;
  OMTFinputMaker *myInputMaker;
  OMTFSorter *mySorter;
  OMTFProcessor *myOMTF;
  OMTFConfigMaker *myOMTFConfigMaker;

  ///Analysis objects
  AnaEff        *myAnaEff;
  AnaSiMuDistribution *myAnaSiMu;


}; 

#endif
