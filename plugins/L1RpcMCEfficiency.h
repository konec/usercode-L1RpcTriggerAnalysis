#ifndef UserCode_konec_L1RpcMCEfficiency_H
#define UserCode_konec_L1RpcMCEfficiency_H

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <vector>
namespace edm { class ParameterSet; class Event; class EventSetup; }
class SimTrack;
class SimVertex;
class L1MuRegionalCand;
class TH1D;



class L1RpcMCEfficiency : public edm::EDAnalyzer {
public:
  explicit L1RpcMCEfficiency(const edm::ParameterSet& conf);
  ~L1RpcMCEfficiency();
  virtual void beginJob();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();

private:
  static void print(const SimTrack & track);
  static void print(const SimVertex & vertex);
  static void print(const L1MuRegionalCand & l1mu);
  static double phiValue(const int iseg);
  float effError(float num, float denom);

private:
  std::map<std::string, TH1D*> theHistos;
  edm::Service<TFileService> theFileService;

  typedef std::vector<uint32_t> PtCutCodes;
  PtCutCodes ptCutCodes;
  float theSimpleEffNum, theSimpleEffDen;
};

#endif 
