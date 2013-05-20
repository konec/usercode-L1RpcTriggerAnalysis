#ifndef AnaOtfEff_H
#define AnaOtfEff_H

class TrackObj;
class L1Obj;
class TObjArray;
class EventObj;
namespace edm {class ParameterSet;}

class AnaOtfEff {
public:
  AnaOtfEff( const edm::ParameterSet& cfg);
  void init(TObjArray& histos);
  void run( const EventObj* ev, const TrackObj* muon, const L1Obj& l1Otf);

  bool debug;
private:
  double ptCut;
};

#endif

