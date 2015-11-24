#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <map>
#include <utility>
#include <algorithm>

#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/Common/interface/Handle.h"

#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "DataFormats/DetId/interface/DetIdCollection.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"


#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/DTGeometry/interface/DTChamber.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"

#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"

#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCChamber.h"
#include "Geometry/CSCGeometry/interface/CSCLayer.h"
#include "Geometry/CSCGeometry/interface/CSCLayerGeometry.h"
#include "DataFormats/MuonDetId/interface/CSCDetId.h"

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

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

typedef std::vector< std::pair< LinkBoardElectronicIndex, LinkBoardPackedStrip> > RawDataFrames;

class OmtfAngleAnalyser : public edm::EDAnalyzer {
public:
  OmtfAngleAnalyser (const edm::ParameterSet & cfg);
  virtual ~OmtfAngleAnalyser(){}
  virtual void beginJob() { std::cout << "begin Job" << std::endl; }
  virtual void beginRun(const edm::Run&,  const edm::EventSetup& es) {
    std::cout <<"beingRun " << std::endl;
//    dt2omtf(es);
//    csc2omtf(es);
    rpc2omtf(es);
  }

  virtual void analyze(const edm::Event&, const edm::EventSetup& es) {
    std::cout <<"analyze event" << std::endl;
  }

private:
  std::string lbNameforDetId(const RPCReadOutMapping* cabling, uint32_t rawDetId);
  void csc2omtf( const edm::EventSetup& es);
  void rpc2omtf( const edm::EventSetup& es);
  void dt2omtf( const edm::EventSetup& es);
};


OmtfAngleAnalyser::OmtfAngleAnalyser (const edm::ParameterSet & cfg) {}

void OmtfAngleAnalyser::dt2omtf(const edm::EventSetup& es)
{
  double hsPhiPitch = 2*M_PI/5400.;
  edm::ESHandle<DTGeometry> dtGeometry;
  es.get<MuonGeometryRecord>().get(dtGeometry);
  const std::vector<const DTChamber*> & chambers = dtGeometry->chambers();
  std::cout <<"Size of chambers: " << chambers.size() << std::endl;

  for (int posneg=-1; posneg <=1; posneg+=2) {
    for (int board=1; board<=6; board++) {
      std::cout <<"BOARD: " << board <<" overlap: "<<posneg<< std::endl;
      for (auto chamber:chambers) {
        DTChamberId dtid = chamber->id();
        if (posneg*2 != dtid.wheel()) continue;
        if (dtid.station() >3 ) continue;
        int sector = dtid.sector();

        if (board !=6 && !(sector >= board*2-1 && sector <= board*2+1) ) continue;
        if (board ==6 && !(sector >= 11 || sector==1) ) continue;
        int ichamber = sector-1-2*(board-1);
        if (ichamber < 0) ichamber += 12;
 
        double offsetLoc = ( (ichamber-1)*M_PI/6+M_PI/12.)/hsPhiPitch;

        //std::cout << dtid << "scale: "<< 1./4096/hsPhiPitch << " offset: " << std::setw(5)<< offsetLoc <<std::endl;
        std::cout << dtid << "scale: "<< 1./4096*5400/2/M_PI << " offset: " << std::setw(5)<< offsetLoc <<std::endl;
      }
    }
  }

  
}

void OmtfAngleAnalyser::csc2omtf(const edm::EventSetup& es)
{
  edm::ESHandle<CSCGeometry> cscGeometry;
  es.get<MuonGeometryRecord>().get(cscGeometry);
  const std::vector<const CSCChamber*> & chambers = cscGeometry->chambers();
  std::cout <<"Size of chambers: " << chambers.size() << std::endl;
/*
  for (auto chamber:chambers) {
    const CSCChamberSpecs* cspec = chamber->specs(); 
    CSCDetId cid = chamber->id();
    if (cid.zendcap() != 1) continue;
//    if ( !(cid.ring()==2 && (cid.station()==2)) ) continue;
    if ( !(cid.ring()==2 && (cid.station()==2 || cid.station()==3)) ) continue;
    if (cid.chamber() > 8 || cid.chamber() < 2) continue;
//    if (cid.chamber() != 8) continue;
    unsigned int nStrips = cspec->nStrips();
    float stripPhiPitch = cspec->stripPhiPitch();
    std::cout <<"------------"<<2*M_PI/stripPhiPitch << std::endl;
    std::cout << cspec->chamberTypeName()<<"/"<<cid.chamber()<< chamber->id()
              <<" strips: "<<nStrips
              <<" phiPitch: "<<cspec->stripPhiPitch() 
              << std::endl;
//    std::cout <<" id: " << chamber->id() << std::endl;
//    std::cout <<"Layer offset: "; for (unsigned int i=1; i <=6; i++) std::cout <<" "<<chamber->layer(i)->geometry()->stripOffset();
//    std::cout <<std::endl;
    const CSCLayer* layer = chamber->layer(3);
    int order = ( layer->centerOfStrip(2).phi() > layer->centerOfStrip(1).phi() ) ? 1 : -1;
//    const CSCLayerGeometry * lgeo = layer->geometry(); 
//    std::cout << *lgeo << std::endl;
//    std::cout <<" strip Pitch: "<<stripPhiPitch << " stripOffset: "<<lgeo->stripOffset() << std::endl;

    for (unsigned int is=1; is <= nStrips; is++) {
      GlobalPoint pos = layer->centerOfStrip(is);
      float center_phi = pos.phi();
      std::cout <<"halfStrip: "<< 2*(is-1)   << " phi: "<< center_phi-order*stripPhiPitch/4.<< std::endl;
      std::cout <<"halfStrip: "<< 2*(is-1)+1 << " phi: "<< center_phi+order*stripPhiPitch/4.<< std::endl;
    }
  }

*/

  double hsPhiPitch = 2*M_PI/5400.;
  for (int posneg=-1; posneg <=1; posneg+=2) {
    int endcap = (posneg==-1)? 2 : 1;
    for (int board=1; board<=6; board++) {
//      if (endcap!=2) continue;
//      if (board!=2) continue;
      std::cout <<"BOARD: " << board <<" overlap: "<<posneg<< std::endl;
      for (auto chamber:chambers) {
        const CSCChamberSpecs* cspec = chamber->specs();
        CSCDetId cid = chamber->id();

        //
        // filter out chambers not connected to OMTF board
        //
        if (cid.endcap() != endcap) continue;
        if ( cid.ring() != 3 && !(cid.ring()==2 && (cid.station()==2 || cid.station()==3 || cid.station()==1)) ) continue;
        if (board !=6) {
          if (cid.chamber() < (board-1)*6 + 2) continue;
          if (cid.chamber() > (board-1)*6 + 8) continue;
        } else {
          if (cid.chamber() > 2 && cid.chamber() < 32) continue; 
        }

        //tmp
//        if ( cid.ring() != 3) continue;
//        if (cid.station() !=2) continue;
//        if ( cid.station() !=2 && cid.station() !=3) continue;
//          if (cid.chamber() != 2 && cid.chamber() != 3) continue;

        //
        // assign number 0..6, consecutive board for a processor
        //
        int ichamber = (cid.chamber()-2-6*(board-1));
        if (ichamber < 0) ichamber += 36;

        //
        // old offset number (temporary!)
        //
//        int offset = -1024 + 80 + ichamber*160;
//        std::cout << " zendcap: "<<cid.zendcap()<<" "<<cspec->chamberTypeName()<<"/"<<cid.chamber()<< chamber->id() 
//         << " ichamber: "<<ichamber<<" offset: "<<offset<< std::endl;

        //
        // get offset for each chamber
        //
        const CSCLayer* layer = chamber->layer(3);
        double dPhi = layer->centerOfStrip(2).phi() - layer->centerOfStrip(1).phi();
        int order = (dPhi > 0) ? 1 : -1;
        double stripPhiPitch = cspec->stripPhiPitch();
        double scale = fabs(stripPhiPitch/hsPhiPitch/2.); 
        if ( fabs(scale-1.) < 0.0002) scale=1.;
        double phi15deg = M_PI/3.*(board-1)+M_PI/12.;
        double phiHalfStrip0 = layer->centerOfStrip(10).phi() - order*9*stripPhiPitch - order*stripPhiPitch/4.;
        //double phiHalfStrip0 = layer->centerOfStrip(50).phi() - order*49*stripPhiPitch - order*stripPhiPitch/4.;
        if ( board==6 || phiHalfStrip0<0) phiHalfStrip0 += 2*M_PI; 
        double foffset = (phiHalfStrip0-phi15deg)/hsPhiPitch;
        int offset = lround(foffset);
        std::cout << " zendcap: "<<cid.zendcap()<<" "<<cspec->chamberTypeName()<<"/"<<cid.chamber()<< chamber->id() 
                  << " ichamber: "<<ichamber<<" order: "<<order<<" scale: "<<scale<<" offset: "<<offset
//                  <<" hs0: "<<phiHalfStrip0<<" float_offset: "<<foffset
//                   <<" phi_range: ("<<layer->centerOfStrip(1).phi()<<", "<<layer->centerOfStrip(80).phi()<<")"
                  <<std::endl;
 
/*
        //
        // test chamber conversion from global phi and parameters.
        //
        for (int is=1; is <= cspec->nStrips(); is++) {
          float center_phi = layer->centerOfStrip(is).phi();
          for (int ihs = 0; ihs <=1; ihs++) {
            //std::cout <<"halfStrip: "<< 2*(is-1)   << " phi: "<< center_phi-order*stripPhiPitch/4.<< std::endl;
            //std::cout <<"halfStrip: "<< 2*(is-1)+1 << " phi: "<< center_phi+order*stripPhiPitch/4.<< std::endl;
            double halfStripPhi = center_phi+(2*ihs-1)*order*stripPhiPitch/4;
            int iphi1 = lround(halfStripPhi/hsPhiPitch) - (board-1)*5400/6 - 5400/24;
            if (iphi1 < -5400/24) iphi1 += 5400;
            int halfStrip = 2*(is-1)+ihs;
//            double fphi2 = foffset + order*scale*halfStrip;
//            int iphi2 = static_cast<int>(fphi2);
            int iphi2 = offset + order*scale*halfStrip;
            int delta_iphi12 = iphi2-iphi1;
            if (abs(delta_iphi12) > 0) std::cout <<"chamber: "<<ichamber<<" iphi: "<<iphi1<<" iphi2: "<<iphi2<<" delta: "<<delta_iphi12<<std::endl; 
//            else std::cout <<" OK "<< std::endl;
          }
        }
*/
      }
    }
  }
}

void OmtfAngleAnalyser::rpc2omtf(const edm::EventSetup& es)
{

  edm::ESHandle<RPCGeometry> rpcGeometry;
  es.get<MuonGeometryRecord>().get(rpcGeometry);
  const std::vector<const RPCRoll*>& rolls = rpcGeometry->rolls();

  const double hsPhiPitch = 2*M_PI/5400.;
  
  std::cout <<"size: " << rolls.size() << std::endl; 


  unsigned int processor = 6;
  int overlap = 1;  
  double phi15deg =  M_PI/3.*(processor-1)+M_PI/12.; 
  if (processor==6) phi15deg -= 2*M_PI;
  std::vector<const RPCRoll*> sector1Rolls;
  for (auto & roll : rolls) {
    RPCDetId aId = roll->id(); 

     if (overlap == -1) {
     if  (aId.region()>0 ||
        (aId.region()==0 && aId.ring()!=-2) ||
        (aId.region()==0 && aId.station()==4) ||
        (aId.region()==0 && aId.station()==2 && aId.layer()==2 && aId.roll()==1) ||
        (aId.region()==0 && aId.station()==3 && aId.roll()==1) ||
        (aId.region()==-1 && aId.ring()<3)
          )  continue;
     }

    if (overlap == 1) {
    if  (aId.region()<0 ||
        (aId.region()==0 && aId.ring()!=2) ||
        (aId.region()==0 && aId.station()==4) ||
        (aId.region()==0 && aId.station()==2 && aId.layer()==2 && aId.roll()==1) ||
        (aId.region()==0 && aId.station()==3 && aId.roll()==1) ||
        (aId.region()==1 && aId.ring()<3)
          )  continue;
    }

    if (aId.region()==0) {
       if ( (processor==1) && (aId.sector() < 1 || aId.sector() > 3 ) ) continue;
       if ( (processor==2) && (aId.sector() < 3 || aId.sector() > 5 ) ) continue;
       if ( (processor==3) && (aId.sector() < 5 || aId.sector() > 7 ) ) continue;
       if ( (processor==4) && (aId.sector() < 7 || aId.sector() > 9 ) ) continue;
       if ( (processor==5) && (aId.sector() < 9 || aId.sector() > 11 ) ) continue;
       if ( (processor==6) && (aId.sector() != 11 && aId.sector() != 12 && aId.sector() != 1) ) continue;
    } 
    else {
      int aSector = (aId.sector()-1)*6+aId.subsector();
      if ( (processor==1) &&  (aSector < 2  || aSector > 8) ) continue; 
      if ( (processor==2) &&  (aSector < 8  || aSector > 14) ) continue; 
      if ( (processor==3) &&  (aSector < 14 || aSector > 20) ) continue; 
      if ( (processor==4) &&  (aSector < 20 || aSector > 26) ) continue; 
      if ( (processor==5) &&  (aSector < 26 || aSector > 32) ) continue; 
      if ( (processor==6) &&  !(aSector >= 32 || aSector <= 2) ) continue;
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
        switch (processor) { case 1:  case 6: break; default : {if (angle < 0) angle += 2*M_PI; break; } }
        unsigned int halfChannel = 2*lbChannel;
        int iPhiHalfChannel  = std::lround( (angle-phi15deg)/hsPhiPitch );
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
            switch (processor) { case 1:  case 6: break; default : {if (angleN < 0) angleN += 2*M_PI; break; } }
            int iPhiHalfChannelN = std::lround( ((angle+angleN)/2.-phi15deg)/hsPhiPitch);
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
}

std::string OmtfAngleAnalyser::lbNameforDetId(const RPCReadOutMapping* cabling, uint32_t rawDetId)
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




DEFINE_FWK_MODULE(OmtfAngleAnalyser);
