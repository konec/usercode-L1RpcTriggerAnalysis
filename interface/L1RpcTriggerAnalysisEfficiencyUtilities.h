#ifndef L1RpcTriggerAnalysisEfficiencyUtilities_H
#define L1RpcTriggerAnalysisEfficiencyUtilities_H

/** \namespace L1RpcTriggerAnalysisEfficiencyUtilities
 * \author: Konrad Nesteruk 
 *  Small utility funcions used for Efficirency analysis 
 */

#include <string>


namespace L1RpcTriggerAnalysisEfficiencyUtilities {

  class Key {
  public:
    Key(int etaCode, unsigned int ptCode){ieta = etaCode; ipt = ptCode;};
    Key(int etaCode){ieta = etaCode; ipt = 100;};
    std::string name();
  private:
    int ieta, ipt;
  };


  class PtScale {
  public:
    static unsigned int ptCode(float ptValue);
    static float ptValue(unsigned int ptCode);
    static const double* xbins() {return xBins;}
    static unsigned int nbins() { return n_binspt; }
  private:
    static const unsigned int n_binspt = 32;
    static double xBins[n_binspt + 1];
  };


  class EtaScale {
  public:
    static unsigned int etaCode(float etaValue);
    static float etaValue(unsigned int etaCode);
    static const double* toWer() {return tower;}
    static unsigned int ntowers() { return n_towers; }
  private:
    static const unsigned int n_towers = 18;
    static double tower[n_towers];
  };


}
#endif
