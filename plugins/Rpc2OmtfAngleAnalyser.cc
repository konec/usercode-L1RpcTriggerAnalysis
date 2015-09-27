#include "Rpc2OmtfAngleAnalyser.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <map>
#include <utility>
#include <algorithm>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"

#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "DataFormats/DetId/interface/DetIdCollection.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"

#include "CondFormats/RPCObjects/interface/RPCReadOutMapping.h"
#include "CondFormats/RPCObjects/interface/LinkBoardSpec.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "CondFormats/RPCObjects/interface/RPCEMap.h"
#include "CondFormats/DataRecord/interface/RPCEMapRcd.h"


#include "CondFormats/RPCObjects/interface/DccSpec.h"
#include "CondFormats/RPCObjects/interface/LinkBoardElectronicIndex.h"
#include "CondFormats/RPCObjects/interface/LinkBoardPackedStrip.h"
#include "CondFormats/RPCObjects/interface/TriggerBoardSpec.h"
#include "CondFormats/RPCObjects/interface/LinkBoardSpec.h"

typedef std::vector< std::pair< LinkBoardElectronicIndex, LinkBoardPackedStrip> > RawDataFrames;

Rpc2OmtfAngleAnalyser::Rpc2OmtfAngleAnalyser (const edm::ParameterSet & cfg) {}

void Rpc2OmtfAngleAnalyser::beginJob()
{
  std::cout << "begin Job" << std::endl;
}

std::string Rpc2OmtfAngleAnalyser::lbNameforDetId(const RPCReadOutMapping* cabling, uint32_t rawDetId)
{
  RPCReadOutMapping::StripInDetUnit duFrame(rawDetId, 1);
  RawDataFrames rawDataFrames = cabling->rawDataFrame(duFrame);

  std::string lbName = "unknown";
  std::vector<const DccSpec*> dccs = cabling->dccList();
  for (std::vector<const DccSpec*>::const_iterator it1= dccs.begin(); it1!= dccs.end(); ++it1) {
    const std::vector<TriggerBoardSpec> & rmbs = (*it1)->triggerBoards();
    for (std::vector<TriggerBoardSpec>::const_iterator it2 = rmbs.begin(); it2 != rmbs.end(); ++it2) {
      const  std::vector<LinkConnSpec> & links = it2->linkConns();
      for (std::vector<LinkConnSpec>::const_iterator it3 = links.begin(); it3 != links.end(); ++it3) {
        const  std::vector<LinkBoardSpec> & lbs = it3->linkBoards();
        for (std::vector<LinkBoardSpec>::const_iterator it4=lbs.begin(); it4 != lbs.end(); ++it4) {
          LinkBoardElectronicIndex ele = { (*it1)->id(), it2->dccInputChannelNum(), it3->triggerBoardInputNumber(), it4->linkBoardNumInLink()};
          for (auto fr : rawDataFrames) { 
            if (    fr.first.dccId == ele.dccId
                 && fr.first.dccInputChannelNum == ele.dccInputChannelNum
                 && fr.first.tbLinkInputNum == ele.tbLinkInputNum
                 && fr.first.lbNumInLink == ele.lbNumInLink
                    ) return it4->linkBoardName();
          }
        }
      }
    }
  }
  return std::string("unknown");
} 

void Rpc2OmtfAngleAnalyser::beginRun(const edm::Run& ru, const edm::EventSetup& es)
{
  std::cout <<"beingRun " << std::endl;

  edm::ESHandle<RPCGeometry> rpcGeometry;
  es.get<MuonGeometryRecord>().get(rpcGeometry);
  const std::vector<const RPCRoll*>& rolls = rpcGeometry->rolls();

  std::cout <<"size: " << rolls.size() << std::endl; 

  std::vector<const RPCRoll*> sector1Rolls;

  for (auto & roll : rolls) {
    RPCDetId aId = roll->id(); 
     if  (aId.region()>0 ||
        (aId.region()==0 && aId.ring()!=-2) ||
        (aId.region()==0 && aId.station()==4) ||
        (aId.region()==0 && aId.station()==2 && aId.layer()==2 && aId.roll()==1) ||
        (aId.region()==0 && aId.station()==3 && aId.roll()==1) ||
        (aId.region()==-1 && aId.ring()<3)
          )  continue;
/*
    if  (aId.region()<0 ||
        (aId.region()==0 && aId.ring()!=2) ||
        (aId.region()==0 && aId.station()==4) ||
        (aId.region()==0 && aId.station()==2 && aId.layer()==2 && aId.roll()==1) ||
        (aId.region()==0 && aId.station()==3 && aId.roll()==1) ||
        (aId.region()==1 && aId.ring()<3)
          )  continue;
*/

    if (aId.region()==0) {
       if (aId.sector() < 2 || aId.sector() > 4 ) continue;
    } 
    else {
      int aSector = (aId.sector()-1)*6+aId.subsector();
      if (aSector < 3 || aSector > 9) continue; 
    }
    sector1Rolls.push_back(roll); 
  }
  std::cout <<" proc1 rolls: " << sector1Rolls.size() << std::endl;

  
  //
  //  
  //
  edm::ESTransientHandle<RPCEMap> readoutMapping;
  es.get<RPCEMapRcd>().get(readoutMapping);
  const RPCReadOutMapping * cabling= readoutMapping->convert();
  std::cout <<" Has readout map, VERSION: " << cabling->version() << std::endl;

  std::map<std::string, std::vector<const RPCRoll* > > lbrolls;
  for (auto & roll : sector1Rolls) {
    RPCDetId rollDetId = roll->id();
    std::string lbName = lbNameforDetId(cabling, rollDetId);
    lbrolls[lbName].push_back(roll);
  }
  std::cout << " ROLL map size is: " << lbrolls.size() << std::endl;
  for (auto me : lbrolls) {
    std::string lbName = me.first;
    const RPCRoll * roll = me.second.front();    
    RPCReadOutMapping::StripInDetUnit duFrame(roll->id(), 1);  
    RawDataFrames rawDataFrames = cabling->rawDataFrame(duFrame);
    LinkBoardElectronicIndex eleIndex = rawDataFrames.front().first;
    const LinkBoardSpec *linkBoard = cabling->location(eleIndex);
    for (unsigned int lbChannel = 0; lbChannel <= 95; lbChannel++) {

      RPCReadOutMapping::StripInDetUnit duFrame = cabling->detUnitFrame(*linkBoard, LinkBoardPackedStrip(lbChannel) );
      uint32_t rawDetId = duFrame.first;
      int strip = duFrame.second;
      if (rawDetId && strip) {
        const RPCRoll* roll = rpcGeometry->roll(rawDetId);
        GlobalPoint stripPosition = roll->toGlobal(roll->centreOfStrip(strip));
        double angle = stripPosition.phi(); 
        //int iPhiHalfChannel = std::floor( (angle+angleN)/2. * 5760./2./M_PI) - 1024 - 230;   //as Artur does
        unsigned int halfChannel = 2*lbChannel;
        int iPhiHalfChannel = std::floor( angle * 5760./2./M_PI) - 1024 - 230;   //as Artur does
        std::cout << std::setfill(' ') 
                  << lbName 
                  <<" " <<std::setw(10)<< roll->id().rawId() 
                  <<" strip: "<< std::setw(3) << strip
                  << " lbChannel: "<< std::setw(3) <<  lbChannel 
                  <<" halfChannel: "<<std::setw(4) << halfChannel
                  <<" phi: "<<std::setw(5) << iPhiHalfChannel 
                  << std::endl;
        for (unsigned int lbChannelN = lbChannel+1; lbChannelN <= 95; lbChannelN++) {
          RPCReadOutMapping::StripInDetUnit duFrameN = cabling->detUnitFrame(*linkBoard, LinkBoardPackedStrip(lbChannelN) );
          uint32_t rawDetIdN = duFrameN.first;
          int stripN = duFrameN.second;
          if (rawDetIdN && stripN) {
            const RPCRoll* rollN = rpcGeometry->roll(rawDetIdN);
            GlobalPoint stripPositionN = rollN->toGlobal(rollN->centreOfStrip(stripN));
            double angleN = stripPositionN.phi(); 
            int iPhiHalfChannelN = std::floor( (angle+angleN)/2. * 5760./2./M_PI) - 1024 - 230;   //as Artur does
            for (unsigned int halfChannelN = 2*lbChannel+1; halfChannelN <2*lbChannelN; halfChannelN++) {
              std::cout << std::setfill(' ') 
                      << lbName 
                      <<" " <<std::setw(10)<< 0
                      <<" strip: "<< std::setw(3) <<     0 
                      << " lbChannel: "<< std::setw(3) <<        0
                      <<" halfChannel: "<<std::setw(4) << halfChannelN
                      <<" phi: "<<std::setw(5) << iPhiHalfChannelN 
                      << std::endl;
            }
            break;
          }
        }
      }
    }
  }


/*
  for (auto me : lbrolls) {
    std::string lbName = me.first;
    for (auto roll : me.second) {
      int nstrips = roll->nstrips();
      for (int strip=1; strip <= nstrips; strip++) {

        GlobalPoint stripPosition = roll->toGlobal(roll->centreOfStrip(strip));
        double angle = stripPosition.phi(); 
        int iPhi = std::floor( angle * 5760./2./M_PI) - 1024 - 230;   //as Artur does

        RPCReadOutMapping::StripInDetUnit duFrame(roll->id(), strip);
        RawDataFrames rawDataFrames = cabling->rawDataFrame(duFrame);
        unsigned int lbChannel = (rawDataFrames.front().second).packedStrip();   // may be several paths but strip is the same
        unsigned int halfChannel = lbChannel*2;
        std::cout << std::setfill(' ') 
                  << lbName 
                  <<" " << std::setw(10)<<roll->id().rawId()
                  <<" strip: "<< std::setw(3)<<strip 
                  <<" lbChannel: "<< std::setw(3) << lbChannel
                  <<" halfChannel: "<< std::setw(4) <<halfChannel
                  <<" phi: "<< std::setw(5) <<iPhi 
               // <<" rollName: " << roll->id() 
                  << std::endl;

// in case of endcap chambers
// there can be 2 fired consecutive LB channels connected to 
// (non consequtive) strips in different rolls
// what to do in this case? currently there is no such strip 
        LinkBoardElectronicIndex eleIndex = rawDataFrames.front().first;
        const LinkBoardSpec * linkBoard = cabling->location(eleIndex);
        RPCReadOutMapping::StripInDetUnit duFrameN = cabling->detUnitFrame(*linkBoard, LinkBoardPackedStrip(lbChannel+1) );
        uint32_t rawDetIdN = duFrameN.first;
        int stripN = duFrameN.second;
        if (rawDetIdN) {
          const RPCRoll* rollN = rpcGeometry->roll(rawDetIdN);
//        if (strip+1 <= nstrips) {
//         GlobalPoint stripPositionN = roll->toGlobal(roll->centreOfStrip(strip+1));
          GlobalPoint stripPositionN = rollN->toGlobal(rollN->centreOfStrip(stripN));
          double angleN = stripPositionN.phi(); 
          int iPhiHalfChannel = std::floor( (angle+angleN)/2. * 5760./2./M_PI) - 1024 - 230;   //as Artur does
          std::cout << std::setfill(' ') 
                    << lbName 
                    <<" " <<std::setw(10)<< 0
                    <<" strip: "<< std::setw(3) <<     0 
                    << " lbChannel: "<< std::setw(3) <<        0
                    <<" halfChannel: "<<std::setw(4) << halfChannel+1
                    <<" phi: "<<std::setw(5) << iPhiHalfChannel 
                  << std::endl;
        }


        //std::cout <<" strip: " << strip << " channel: " << lbChannel <<" iPhi: "<< iPhi << std::endl;
        //std::cout << lbName <<" "<<lbChannel<<" "<<iPhi << std::endl;
      }
    } 
  } 
*/




}


void Rpc2OmtfAngleAnalyser::analyze(const edm::Event&, const edm::EventSetup& es)
{
  std::cout <<" analyze event" << std::endl;
}
