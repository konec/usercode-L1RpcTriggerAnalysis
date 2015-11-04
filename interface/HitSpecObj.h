#ifndef HitSpecObj_H
#define HitSpecObj_H
#include "TObject.h"

#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"

class HitSpecObj : public TObject {

public:
  HitSpecObj( unsigned int id=0, 
              const GlobalPoint  &pos = GlobalPoint(), 
              const GlobalVector &mom = GlobalVector()) 
    :  theRawId(id), 
       pos_x(pos.x()), pos_y(pos.y()),pos_z(pos.z()),
       mom_x(mom.x()), mom_y(mom.y()),mom_z(mom.z()) {}

  unsigned int rawId() const { return theRawId; }
  GlobalPoint position() const { return GlobalPoint(pos_x,pos_y,pos_z); }
  GlobalVector momentum() const { return GlobalVector(mom_x, mom_y, mom_z); }

private:
  unsigned int theRawId;
  double pos_x,  pos_y, pos_z;
  double mom_x, mom_y, mom_z;

  friend std::ostream & operator << (std::ostream &out, const HitSpecObj&o) {
    out <<" HitSpec  DET: "<<o.theRawId<<", GlbPos: "<<o.position()<<", GlbMom: "<<o.momentum();
    return out;
  }


  //ClassDef(HitSpecObj,1)
};
#endif 
