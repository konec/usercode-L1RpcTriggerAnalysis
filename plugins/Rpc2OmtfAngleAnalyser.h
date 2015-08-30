#ifndef Rpc2OmtfAngleAnalyser_H 
#define Rpc2OmtfAngleAnalyser_H 

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class RPCReadOutMapping;
namespace edm { class Event; class EventSetup; }

class Rpc2OmtfAngleAnalyser : public edm::EDAnalyzer {
public:
  Rpc2OmtfAngleAnalyser (const edm::ParameterSet & cfg);
  virtual ~Rpc2OmtfAngleAnalyser(){}
  virtual void beginJob();
  virtual void beginRun(const edm::Run&,  const edm::EventSetup& es);
  virtual void analyze(const edm::Event&, const edm::EventSetup& es);

private:
  std::string lbNameforDetId(const RPCReadOutMapping* cabling, uint32_t rawDetId);
};

#endif

