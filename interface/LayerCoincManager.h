#ifndef UserCode_L1RpcTriggerAnalysis_LayerCoincManager_H
#define UserCode_L1RpcTriggerAnalysis_LayerCoincManager_H

#include <vector>
#include <map>
#include <bitset>

class LayerCoincManager {
public:
  LayerCoincManager();
  bool matched( float eta,  std::bitset<6> & items) const;
private:
    std::vector< std::bitset<6> > theLayersB, theLayersT6, theLayersT7, theLayersT8, theLayersE;  

};
#endif
