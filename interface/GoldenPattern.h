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
  enum PosBenCase { POSRPC=0, POSCSC=1, BENCSC=2, POSDT=3, BENDT=4 };

  //
  // Key
  //
  struct Key {
    Key(uint32_t det=0, float pt=0, int charge= 0, float phi=0) : theDet(det), theCharge(charge) { 
      thePtCode = L1RpcTriggerAnalysisEfficiencyUtilities::PtScale().ptCode(pt);
      while  (phi < 0) { phi+=2*M_PI; }
      thePhiCode = round( phi * 2*1152./(2*M_PI)); 
      //thePhiCode = phi;
      theRotation = 0;
      theRefStrip = 999;
    }
    inline bool operator< (const Key & o) const {
      if (theCharge*thePtCode < o.theCharge*o.thePtCode) return true;
      else if (theCharge*thePtCode==o.theCharge*o.thePtCode && thePhiCode < o.thePhiCode) return true;
      else if (theCharge*thePtCode==o.theCharge*o.thePtCode && thePhiCode==o.thePhiCode && theDet<o.theDet) return true;
      else return false;
    }
    bool operator==(const Key& o) const {
      return !(theDet!=o.theDet || thePtCode!=o.thePtCode || thePhiCode!=o.thePhiCode || theCharge!=o.theCharge);
    }
    float ptValue() const { return  L1RpcTriggerAnalysisEfficiencyUtilities::PtScale().ptValue( thePtCode); }
    float phiValue() const { return (float)thePhiCode/(2*1152./(2*M_PI)); }
    float etaValue() const { return 6*(theDet==637602109) + 
	                                    7*(theDet==637634877) +
	                                    8*(theDet==637599914) +
	                                    9*(theDet==637632682); }
    int chargeValue() const { return theCharge;}
    uint32_t     theDet; 
    unsigned int thePtCode; 
    unsigned int thePhiCode;
    unsigned int theRefStrip;
    int          theCharge;
    int          theRotation;
    friend std::ostream & operator << (std::ostream &out, const Key & o) {
      out << "Key_det:"<<o.theDet<<"_pt:"<<o.thePtCode<<"_charge"<<o.theCharge
	  <<"_phi:"<<o.thePhiCode<<"("<<o.theRefStrip<<")"
	  <<"_rotation: "<<o.theRotation; 
      return out;
    }
  };

  //
  // Result
  //
  class Result {
  public: 
    Result() : checkMe(true), theValue(0.),
               hasStation1(false), hasStation2(false) {
      nMatchedPoints[GoldenPattern::POSRPC] = 0;
      nMatchedPoints[GoldenPattern::POSDT] = 0;
      nMatchedPoints[GoldenPattern::POSCSC] = 0;
      nMatchedPoints[GoldenPattern::BENDT] = 0;
      nMatchedPoints[GoldenPattern::BENCSC] = 0;  

      myResults[GoldenPattern::POSRPC] =  std::vector< std::pair<uint32_t, float > >();   
      myResults[GoldenPattern::POSDT] =  std::vector< std::pair<uint32_t, float > >();   
      myResults[GoldenPattern::POSCSC] =  std::vector< std::pair<uint32_t, float > >();   
      myResults[GoldenPattern::BENDT] =  std::vector< std::pair<uint32_t, float > >();   
      myResults[GoldenPattern::BENCSC] =  std::vector< std::pair<uint32_t, float > >();   
    }
    bool operator<( const Result & o) const;
    operator bool() const;
    float value() const;
    unsigned int nMatchedTot() const;
    bool hasRpcDet(uint32_t rawId) {
      for (auto it=myResults[GoldenPattern::POSRPC].cbegin(); 
	   it != myResults[GoldenPattern::POSRPC].cend(); ++it) {
        if (it->first == rawId && it->second > 0. && it->second < 1.) return true; 
      }
      return false;
    }
  private:
    void run() const { if (checkMe) {checkMe=false; runNoCheck(); } }
    void runNoCheck() const;
    float norm(PosBenCase where, float whereInDist) const;
  private:
    mutable bool checkMe; 
    mutable float theValue;
    mutable std::map<GoldenPattern::PosBenCase, unsigned int> nMatchedPoints;
   
    bool hasStation1, hasStation2;
    mutable std::map<GoldenPattern::PosBenCase, std::vector< std::pair<uint32_t, float > > > myResults;
    friend std::ostream & operator << (std::ostream &out, const Result& o); 
    friend class GoldenPattern; 
  };


  //
  // GoldenPatterns methods
  //
  GoldenPattern() {}
  GoldenPattern(const GoldenPattern::Key & key) : theKey(key) {}

  Key key() const {return theKey;}

  void add( const Pattern & p);
  void add( GoldenPattern::PosBenCase aCase, uint32_t rawId, int pos, unsigned int freq);
  void makeIntegratedCache();
  Result compare( const Pattern & p);



private:

  ///Pattern kinematical identification (eta,phi,pt)
  Key theKey;

  ///Distribution in given DetUnit.
  typedef  std::map<int, float> MFreq;

  ///Ditribution in DetUnits. For given measuremnt type, one can have
  ///measurementd in many layers(=DetUnits)
  typedef  std::map<uint32_t, MFreq > DetFreq; 
  
  ///Set of distributions for all measurement types (DT position, DT bending, RPC, etc)
  typedef  std::map<GoldenPattern::PosBenCase, DetFreq > SystFreq;

  SystFreq PattCore;
  SystFreq PattCoreIntegrated;

private:

  bool purge();
  float whereInDistribution(int obj, const MFreq & m) const;
  float whereInDistribution(PosBenCase mType, uint32_t rawId, int pos) const;
  friend std::ostream & operator << (std::ostream &out, const GoldenPattern & o);
  friend class PatternManager;
};
#endif 
