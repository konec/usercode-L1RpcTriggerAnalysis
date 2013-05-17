#ifndef UserCode_L1RpcTriggerAnalysis_GoldenPattern_H
#define UserCode_L1RpcTriggerAnalysis_GoldenPattern_H

#include "UserCode/L1RpcTriggerAnalysis/interface/L1RpcTriggerAnalysisEfficiencyUtilities.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/Pattern.h"
#include <map>
#include <vector>
#include <cmath>
#include <ostream>
#include <iostream>

class GoldenPattern {
public:

  //
  // where
  //
  enum PosBenCase { POSRPC=1, POSCSC=2, BENCSC=3, POSDT=4, BENDT=5 };

  //
  // Key
  //
  struct Key {
    Key(uint32_t det=0, double pt=0, int charge= 0, double phi=0) : theDet(det), theCharge(charge) { 
      thePtCode = L1RpcTriggerAnalysisEfficiencyUtilities::PtScale().ptCode(pt);
      while  (phi < 0) { phi+=2*M_PI; }
      thePhiCode = int( phi * 3000.); 
    } 
    bool operator< (const Key & o) const {
      if (theCharge*thePtCode < o.theCharge*o.thePtCode) return true;
      else if (theCharge*thePtCode==o.theCharge*o.thePtCode && thePhiCode < o.thePhiCode) return true;
      else if (theCharge*thePtCode==o.theCharge*o.thePtCode && thePhiCode==o.thePhiCode && theDet<o.theDet) return true;
      else return false;
    }
    bool operator==(const Key& o) const {
      return theDet==o.theDet && thePtCode==o.thePtCode && thePhiCode==o.thePhiCode && theCharge==o.theCharge;
    }
    double ptValue() const { return  L1RpcTriggerAnalysisEfficiencyUtilities::PtScale().ptValue( thePtCode); }
    double phiValue() const { return (double)thePhiCode/3000.; }
    uint32_t     theDet; 
    unsigned int thePtCode; 
    unsigned int thePhiCode;
    int          theCharge;
    friend std::ostream & operator << (std::ostream &out, const Key & o) {
      out << "Key_det:"<<o.theDet<<"_pt:"<<o.thePtCode<<"_charge"<<o.theCharge<<"_phi:"<<o.thePhiCode; 
      return out;
    }
  };

  //
  // Result
  //
  class Result {
  public: 
    Result() : checkMe(true), theValue(0.), 
               nMatchedPosRpc(0), nMatchedPosCsc(0), nMatchedPosDt(0), nMatchedBenCsc(0), nMatchedBenDt(0)  {}
    bool operator<( const Result & o) const;
    operator bool() const;
    double value() const;
    unsigned int nMatchedTot() const;
  private:
    void run() const { if (checkMe) {checkMe=false; runNoCheck(); } }
    void runNoCheck() const;
    double norm(PosBenCase where, double whereInDist) const;
  private:
    mutable bool checkMe; 
    mutable double theValue;
    mutable unsigned int nMatchedPosRpc, nMatchedPosCsc, nMatchedPosDt, nMatchedBenCsc, nMatchedBenDt;
    std::vector< std::pair<uint32_t, double > > posRpcResult;
    std::vector< std::pair<uint32_t, double > > posCscResult;
    std::vector< std::pair<uint32_t, double > > benCscResult;
    std::vector< std::pair<uint32_t, double > > posDtResult;
    std::vector< std::pair<uint32_t, double > > benDtResult;
    friend std::ostream & operator << (std::ostream &out, const Result& o); 
    friend class GoldenPattern; 
  };


  //
  // GoldenPatterns methods
  //
  GoldenPattern() {}
  GoldenPattern(const GoldenPattern::Key & key) : theKey(key) {}
  void add( const Pattern & p);
  void add( GoldenPattern::PosBenCase aCase, uint32_t rawId, int pos, unsigned int freq);
  Result compare( const Pattern & p) const;

private:

  Key theKey;
  typedef  std::map<int, unsigned int> MFreq;
  typedef  std::map<uint32_t, MFreq > DetFreq; 

  DetFreq posRpc;

  DetFreq posCsc;
  DetFreq bendingCsc;

  DetFreq posDt;
  DetFreq bendingDt;

private:

  void purge();
  double whereInDistribution(int obj, const MFreq & m) const;
  friend std::ostream & operator << (std::ostream &out, const GoldenPattern & o);
  friend class PatternManager;
};
#endif 
