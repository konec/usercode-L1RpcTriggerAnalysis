#ifndef UserCode_L1RpcTriggerAnalysis_PatternCreator_H
#define UserCode_L1RpcTriggerAnalysis_PatternCreator_H

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include <vector>
#include <utility>
#include <ostream>


class  Pattern {
public:
  typedef std::vector< std::pair <uint32_t,  unsigned int > >  DataType;

  Pattern() {}

  //is not empty
  operator bool() const { return theData.size() > 0; }

  bool add(std::pair<uint32_t,  unsigned int > aData) { return addOrCopy(aData); }

  static void add( std::vector<Pattern> & vpat, std::pair<uint32_t,  unsigned int > aData);

  unsigned int size() const { return theData.size(); }
  operator const DataType & () const {  return theData; }

private:

  // try to add data from raw id to this pattern. if the data from detUnit 
  // is already assigned to this patterns return a copy of this pattern with
  // modified (from aData) data attached to detUnit. Otherwise add data from detUnit
  // to this pattern and return an ampty pattern;  
  Pattern  addOrCopy ( std::pair<uint32_t,  unsigned int > aData); 
  
private:
  DataType  theData;
  friend std::ostream & operator << (std::ostream &out, const Pattern &o);
};
#endif


