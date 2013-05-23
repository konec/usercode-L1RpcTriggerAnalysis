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
  bool add ( std::pair<uint32_t,  unsigned int > aData); 
  unsigned int size() const { return theData.size(); }
  operator const DataType & () const {  return theData; }

  static bool add( std::vector<Pattern> & vpat, std::pair<uint32_t,  unsigned int > aData);
  
private:
  DataType  theData;
  friend std::ostream & operator << (std::ostream &out, const Pattern &o);
};
#endif


