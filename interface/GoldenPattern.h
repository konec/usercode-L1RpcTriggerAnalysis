#ifndef UserCode_L1RpcTriggerAnalysis_GoldenPattern_H
#define UserCode_L1RpcTriggerAnalysis_GoldenPattern_H

#include "DataFormats/MuonDetId/interface/RPCDetId.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/MtfCoordinateConverter.h"
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
 Key(int etaCode=99, float pt=0, int charge= 0, float phi=0, 
     unsigned int refStrip=-99) : 
   theEtaCode(etaCode), theRefStrip(refStrip), theCharge(charge) { 
   
   thePtCode = L1RpcTriggerAnalysisEfficiencyUtilities::PtScale::ptCode(pt);
   if(thePtCode>31) thePtCode = 31;
   while  (phi < 0) { phi+=2*M_PI; }
   while  (phi > 2*M_PI) { phi-=2*M_PI; }
   thePhiCode = floor(phi * 1.0/(2*M_PI)); 
   theRotation = 0;
   theDet = 0;
 }
   inline bool operator< (const Key & o) const {
     if (thePtCode > o.thePtCode) return true;
     else if (thePtCode==o.thePtCode && theCharge < o.theCharge) return true;
     //if (theCharge*thePtCode < o.theCharge*o.thePtCode) return true;
     else if (theCharge*thePtCode==o.theCharge*o.thePtCode && thePhiCode < o.thePhiCode) return true;
     else if (theCharge*thePtCode==o.theCharge*o.thePtCode && thePhiCode==o.thePhiCode && theEtaCode<o.theEtaCode) return true;
     else if (theCharge*thePtCode==o.theCharge*o.thePtCode && thePhiCode==o.thePhiCode && theEtaCode==o.theEtaCode &&  theDet<o.theDet) return true;
     else return false;
   }
   bool operator==(const Key& o) const {
     return !(theEtaCode!=o.theEtaCode || thePtCode!=o.thePtCode || thePhiCode!=o.thePhiCode || theCharge!=o.theCharge || theDet!=o.theDet);
   }
   float ptValue() const { return  L1RpcTriggerAnalysisEfficiencyUtilities::PtScale::ptValue(thePtCode); }
   float phiValue() const { return (float)(thePhiCode)/(nPhi(theDet) /(2*M_PI)); }
   float etaValue() const {return L1RpcTriggerAnalysisEfficiencyUtilities::EtaScale::etaValue(theEtaCode);}
   int chargeValue() const { return theCharge;}
   uint32_t     theDet; 
   int theEtaCode;
   unsigned int thePtCode; 
   unsigned int thePhiCode;
   unsigned int theRefStrip;
   int          theCharge;
   int          theRotation;
   //static const int nPhi = 2*1152;
   static const int nPhi(uint32_t aDet){
     return 2*1152;
     if(aDet/(uint32_t)100==1) return 5*1152;
     if(aDet/(uint32_t)100==2) return 5*1152;
     if(aDet/(uint32_t)100==3) return 2*1152;
     return 2*1152;
   }

   friend std::ostream & operator << (std::ostream &out, const Key & o) {
     out << "Key_eta:"<<o.theEtaCode<<"("<<o.theDet<<")_pt:"<<o.thePtCode<<"_charge"<<o.theCharge
	 <<"_phi:"<<o.thePhiCode<<"("<<o.theRefStrip<<")"
	 <<"_rotation:"<<o.theRotation; 
     return out;
   }
 };
 
  //
  // Result
  //
  class Result {
  public: 
    Result() : checkMe(true), theValue(0.),
      hasStation1(false), hasStation2(false), hasStation3(false), hasRefStation(false) {
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

    bool withStation1() const { return hasStation1;}
    bool withStation2() const { return hasStation2;}
    bool withStation3() const { return hasStation3;}

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
   
    bool hasStation1, hasStation2, hasStation3, hasRefStation;
    mutable std::map<GoldenPattern::PosBenCase, std::vector< std::pair<uint32_t, float > > > myResults;
    friend std::ostream & operator << (std::ostream &out, const Result& o); 
    friend class GoldenPattern; 
  };


  //
  // GoldenPatterns methods
  //
 GoldenPattern(): hasIntegratedCache(false) {}
 GoldenPattern(const GoldenPattern::Key & key) : theKey(key), hasIntegratedCache(false){}

  Key key() const {return theKey;}

  void add( const Pattern & p,  MtfCoordinateConverter *myPhiConverter);
  void add( GoldenPattern::PosBenCase aCase, uint32_t rawId, int pos, unsigned int freq);
  void makeIntegratedCache();
  Result compare( const Pattern & p,  MtfCoordinateConverter *myPhiConverter);

  void plot();

  int size();
  

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

  std::map<int64_t,float> patternDataIntegrated;

  bool hasIntegratedCache;

private:

  bool purge();
  float whereInDistribution(int obj, const MFreq & m) const;
  float whereInDistribution(PosBenCase mType, uint32_t rawId, int pos) const;
  friend std::ostream & operator << (std::ostream &out, const GoldenPattern & o);
  friend class PatternManager;
};
#endif 
