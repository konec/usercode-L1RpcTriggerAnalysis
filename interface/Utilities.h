#ifndef L1RpcTriggerAnalysis_Utilities_H
#define L1RpcTriggerAnalysis_Utilities_H

// variable-size PT bins corresponding to L1 trigger scale. 
// Lower bound of bin ipt corresponds to L1Pt(ipt),
// with exception of ipt=0 and 1 (which should be NAN and 0. instead of 0. and 0.1 respectively)
class L1PtScale {
public:
  static unsigned int ptCode(float ptValue);
  static float ptValue(unsigned int ptCode);
  static const unsigned int nPtBins = 32;
  static double ptBins[nPtBins + 1];
};

// variable-size ETA bins corresponding to PAC trigger granularity
class L1RpcEtaScale {
public:
  static int etaCode(float etaValue); // tower for given eta, +-17 for over/undeflows
  static float etaValue(int etaCode); // central eta value of a tower, +-2.4 for over/underrflows 
  static  const unsigned int nEtaBins=33;
  static double etaBins[nEtaBins+1];
};


#endif
