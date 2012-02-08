#include "UserCode/L1RpcTriggerAnalysis/interface/LayerCoincManager.h"
#include <cmath>

bool LayerCoincManager:: matched( float eta,  std::bitset<6> & items) const
{
  bool result = false;


  const std::vector< std::bitset<6> > *layers = 0;
  if (fabs(eta) < 0.83) layers = &theLayersB;
  else if (fabs(eta) < 0.93) layers = &theLayersT6;
  else if (fabs(eta) < 1.04) layers = &theLayersT7;
  else if (fabs(eta) < 1.14) layers = &theLayersT8;
  else layers = &theLayersE;
 
  typedef std::vector< std::bitset<6> >::const_iterator IT;
  for (IT it= layers->begin(); it != layers->end(); ++it) if (it->to_ulong() == items.to_ulong()) result = true;
//  for (IT it= theLayersB.begin(); it != theLayersB.end(); ++it) if (it->to_ulong() == items.to_ulong()) result = true;
  return result; 
}

LayerCoincManager::LayerCoincManager()
{
  theLayersB.push_back(std::bitset<6>(std::string("011110")));
  theLayersB.push_back(std::bitset<6>(std::string("101110")));
  theLayersB.push_back(std::bitset<6>(std::string("110110")));
  theLayersB.push_back(std::bitset<6>(std::string("111010")));
  theLayersB.push_back(std::bitset<6>(std::string("111100")));
  theLayersB.push_back(std::bitset<6>(std::string("001111")));
  theLayersB.push_back(std::bitset<6>(std::string("010111")));
  theLayersB.push_back(std::bitset<6>(std::string("100111")));
  theLayersB.push_back(std::bitset<6>(std::string("011011")));
  theLayersB.push_back(std::bitset<6>(std::string("101011")));
  theLayersB.push_back(std::bitset<6>(std::string("110011")));
  theLayersB.push_back(std::bitset<6>(std::string("011101")));
  theLayersB.push_back(std::bitset<6>(std::string("101101")));
  theLayersB.push_back(std::bitset<6>(std::string("110101")));
  theLayersB.push_back(std::bitset<6>(std::string("111001")));
  theLayersB.push_back(std::bitset<6>(std::string("011111")));
  theLayersB.push_back(std::bitset<6>(std::string("101111")));
  theLayersB.push_back(std::bitset<6>(std::string("110111")));
  theLayersB.push_back(std::bitset<6>(std::string("111011")));
  theLayersB.push_back(std::bitset<6>(std::string("111101")));
  theLayersB.push_back(std::bitset<6>(std::string("111110")));
  theLayersB.push_back(std::bitset<6>(std::string("111111")));
  theLayersB.push_back(std::bitset<6>(std::string("111000")));
  theLayersB.push_back(std::bitset<6>(std::string("110100")));
  theLayersB.push_back(std::bitset<6>(std::string("110010")));
  theLayersB.push_back(std::bitset<6>(std::string("110001")));
  theLayersB.push_back(std::bitset<6>(std::string("100101")));
  theLayersB.push_back(std::bitset<6>(std::string("101001")));
  theLayersB.push_back(std::bitset<6>(std::string("101010")));
  theLayersB.push_back(std::bitset<6>(std::string("101100")));
  theLayersB.push_back(std::bitset<6>(std::string("010101")));
  theLayersB.push_back(std::bitset<6>(std::string("011001")));
  theLayersB.push_back(std::bitset<6>(std::string("011010")));
  theLayersB.push_back(std::bitset<6>(std::string("011100")));
  theLayersB.push_back(std::bitset<6>(std::string("000111")));
  theLayersB.push_back(std::bitset<6>(std::string("001011")));
  theLayersB.push_back(std::bitset<6>(std::string("001101")));
  theLayersB.push_back(std::bitset<6>(std::string("001110")));

  theLayersT6.push_back(std::bitset<6>(std::string("001111")));
  theLayersT6.push_back(std::bitset<6>(std::string("010111")));
  theLayersT6.push_back(std::bitset<6>(std::string("011011")));
  theLayersT6.push_back(std::bitset<6>(std::string("011101")));
  theLayersT6.push_back(std::bitset<6>(std::string("011110")));
  theLayersT6.push_back(std::bitset<6>(std::string("011111")));
  theLayersT6.push_back(std::bitset<6>(std::string("010101")));
  theLayersT6.push_back(std::bitset<6>(std::string("011001")));
  theLayersT6.push_back(std::bitset<6>(std::string("011010")));
  theLayersT6.push_back(std::bitset<6>(std::string("011100")));
  theLayersT6.push_back(std::bitset<6>(std::string("000111")));
  theLayersT6.push_back(std::bitset<6>(std::string("001011")));
  theLayersT6.push_back(std::bitset<6>(std::string("001101")));
  theLayersT6.push_back(std::bitset<6>(std::string("001110")));

  theLayersT7.push_back(std::bitset<6>(std::string("001111")));
  theLayersT7.push_back(std::bitset<6>(std::string("010111")));
  theLayersT7.push_back(std::bitset<6>(std::string("011011")));
  theLayersT7.push_back(std::bitset<6>(std::string("011101")));
  theLayersT7.push_back(std::bitset<6>(std::string("011110")));
  theLayersT7.push_back(std::bitset<6>(std::string("011111")));
  theLayersT7.push_back(std::bitset<6>(std::string("010101")));
  theLayersT7.push_back(std::bitset<6>(std::string("011001")));
  theLayersT7.push_back(std::bitset<6>(std::string("011010")));
  theLayersT7.push_back(std::bitset<6>(std::string("011100")));
  theLayersT7.push_back(std::bitset<6>(std::string("000111")));
  theLayersT7.push_back(std::bitset<6>(std::string("001011")));
  theLayersT7.push_back(std::bitset<6>(std::string("001101")));
  theLayersT7.push_back(std::bitset<6>(std::string("001110")));

  theLayersT8.push_back(std::bitset<6>(std::string("000111")));
  theLayersT8.push_back(std::bitset<6>(std::string("001011")));
  theLayersT8.push_back(std::bitset<6>(std::string("001101")));
  theLayersT8.push_back(std::bitset<6>(std::string("001110")));
  theLayersT8.push_back(std::bitset<6>(std::string("001111")));

  theLayersE.push_back(std::bitset<6>(std::string("000111")));
}
