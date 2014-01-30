#ifndef UserCode_L1RpcTriggerAnalysis_PatternCreator_H
#define UserCode_L1RpcTriggerAnalysis_PatternCreator_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <vector>
#include <utility>
#include <ostream>
#include <map>


class  Pattern {
public:

  typedef std::multimap<uint32_t,  unsigned int >  DataType;

 Pattern():multipleHits(false) {}

  static uint32_t rotateDetId(uint32_t rawId, int step);
  Pattern getRotated(int step) const;

  //is not empty
  operator bool() const { return theData.size() > 0; }

  bool add(std::pair<uint32_t,  unsigned int > aData);

  ///Function making a list of unique detIds
  void makeHitDetsList();

  ///Returns true if a Patter has multiple hits in ady detunit
  bool hasMultipleHits() const {return multipleHits;}

  const std::vector<DataType::value_type> & uniqueDigis() const {return detsHit;}

  unsigned int size() const { return theData.size(); }
  operator const DataType & () const {  return theData; }

  bool operator==(const Pattern& o) const;

private:
  
private:
  bool multipleHits;
  DataType  theData;
  std::vector<DataType::value_type> detsHit;
  friend std::ostream & operator << (std::ostream &out, const Pattern &o);
};
#endif


