#ifndef SynchroCountsObj_H
#define SynchroCountsObj_H

#include "TObject.h"

struct SynchroCountsObj : public TObject {
  int dccId, dccInputChannelNum, tbLinkInputNum, lbNumInLink;
  int  bx; 
  //ClassDef(SynchroCountsObj,1)
};

#endif
