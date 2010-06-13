#ifndef L1ObjColl_H
#define L1ObjColl_H

#include "UserCode/L1RpcTriggerAnalysis/interface/L1Obj.h"
#include <vector>

class L1ObjColl : public TObject {

public:
  L1ObjColl() {}
  virtual ~L1ObjColl(){}

  void set(const std::vector<L1Obj> & obj) { theL1Obj = obj; }
  void set(const std::vector<bool> & comp) { theL1Matching = comp; }
  const std::vector<L1Obj> & getL1Objs() const { return theL1Obj; }
  const std::vector<bool> & getL1ObjsMatching() const { return theL1Matching; }

  std::vector<L1Obj> getL1ObjsMatched(double ptMin = 0) const;
  
private:
  std::vector<L1Obj> theL1Obj;
  std::vector<bool> theL1Matching;

public:
ClassDef(L1ObjColl,1)

};

#endif
