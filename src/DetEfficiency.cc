#include "UserCode/L1RpcTriggerAnalysis/interface/DetEfficiency.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include <sstream>

bool DetEfficiency::isDummy() const
{
  if (theRawId==637632537|| theRawId==637567001) return true;
  return ( RPCDetId(theRawId).roll() == 0);
}

double DetEfficiency::efficiency() const 
{
  return (theMuons == 0) ? 0. : static_cast<double>(theHits)/static_cast<double>(theMuons);
}

double DetEfficiency::efficiencyError() const
{
  double eff = efficiency();
  double nH = (theHits == 0) ? 1 : static_cast<double>(theHits);
//  return eff/sqrt(nH); 

//  double corr = (eff < 1.) ? sqrt(eff*(1.-eff)) : 0.;
//  eff *= corr;
  return  sqrt(eff*(1.-eff))/sqrt(nH);
}

std::string DetEfficiency::print() const
{
  RPCDetId rpc(theRawId);
  std::stringstream name;
  std::string localPartVal[7]={"Forward","Central","Backward","A","B","C","D"};
  if (rpc.roll()==0) name.str("DUMMY");
  else if (rpc.region() ==0) {
    std::string layerVal[6]={"RB1in","RB1out","RB2in","RB2out","RB3","RB4"};
    std::string subsVal[5]={"--","-","","+","++"};
    name << "W";
    if (rpc.ring()>0) name <<"+";
    name <<rpc.ring();
    int lv = rpc.station() <=2 ? 2*( rpc.station()-1)+ rpc.layer() : rpc.station()+2 ;
    name <<"/"<<layerVal[lv-1]<<"/"<<rpc.sector();
    name <<"_sub:"<<rpc.subsector(); 
    name <<"_roll:"<<rpc.roll(); 
    //name<<"_roll="<<localPartVal[rpc.roll()+1];
  } else {
    if (rpc.region() == -1) name <<"RE-";
    if (rpc.region() ==  1) name <<"RE+";
    name <<rpc.station()<<"/"<<rpc.ring()<<"/"<<rpc.sector()<<" roll:"<<rpc.roll();
  }
  std::stringstream str;
  str <<" DET: "<<theRawId<<" "<<name.str()<<" H: "<<theHits<<" SH: "<<theSimHits<<" M: "<<theMuons<< " eff: "<<efficiency()<<" effErr: "<<efficiencyError();
  return str.str();
}
