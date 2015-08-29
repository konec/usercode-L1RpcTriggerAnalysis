/*
 * MtfCoordinateConverter.cpp
 *
 *  Created on: Oct 15, 2013
 *      Author: Karol Bunkowski
 */

#include "UserCode/L1RpcTriggerAnalysis/interface/MtfCoordinateConverter.h"

#include "L1Trigger/CSCCommonTrigger/interface/CSCConstants.h"
#include "L1Trigger/CSCCommonTrigger/interface/CSCPatternLUT.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "L1Trigger/DTUtilities/interface/DTTrigGeom.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

// FIXME_MK #include "L1TriggerDPGUpgrade/DataFormats/interface/L1TMuonTriggerPrimitive.h"

#include "UserCode/L1RpcTriggerAnalysis/interface/DTphDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/CSCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDigiSpec.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/GoldenPattern.h"
#include "UserCode/L1RpcTriggerAnalysis/interface/RPCDetIdUtil.h"

MtfCoordinateConverter::MtfCoordinateConverter(const edm::EventSetup& es): es(es) {

    const MuonGeometryRecord& geom = es.get<MuonGeometryRecord>();

    geom.get(geoCsc);
    geom.get(geoDt);
    geom.get(geoRpc);

    phiref = 0.0;

}

MtfCoordinateConverter::~MtfCoordinateConverter() {
    // TODO Auto-generated destructor stub
}
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
void MtfCoordinateConverter::setReferencePhi(float aPhiRef){

  phiref = aPhiRef;

  while  (phiref < 0) { phiref+=2*M_PI; }
  while  (phiref > 2*M_PI) { phiref-=2*M_PI; }

}
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
int  MtfCoordinateConverter::convert(std::pair<uint32_t,  unsigned int > aData){
  
  float phi = 999.0;
  
  DetId detId(aData.first);
  if (detId.det() != DetId::Muon) std::cout << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;
  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
    RPCDigiSpec digi(aData.first, aData.second);
    
    float phi1 = convertRpc(aData.first,digi.fromStrip());
    float phi2 = convertRpc(aData.first,digi.toStrip());
    phi = (phi1+phi2)/2.0;
  }
    break;
  case MuonSubdetId::DT: {
    DTphDigiSpec digi(aData.first, aData.second);
    phi = convertDt(aData.first,digi.phi());
    break;
  }
  case MuonSubdetId::CSC: {
    CSCDigiSpec digi(aData.first, aData.second);
    phi = convertCsc(aData.first, digi.strip(),digi.pattern(),digi.keywire());
    break;
  }
  }

  phi-=phiref;
  
  while  (phi < 0) { phi+=2*M_PI; }
  while  (phi > 2*M_PI) { phi-=2*M_PI; }
  
  int nDivisions = GoldenPattern::Key::nPhi;
  int iPhi =  floor(phi * nDivisions/(2*M_PI));

  if(iPhi>nDivisions/2.0) iPhi-=nDivisions;

  return iPhi;
}
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
bool isCSCCounterClockwise(const std::unique_ptr<const CSCLayer>& layer) {
  const int nStrips = layer->geometry()->numberOfStrips();
  const double phi1 = layer->centerOfStrip(1).phi();
  const double phiN = layer->centerOfStrip(nStrips).phi();
  return ( (std::abs(phi1 - phiN) < M_PI  && phi1 >= phiN) ||
       (std::abs(phi1 - phiN) >= M_PI && phi1 < phiN)     );
}

float MtfCoordinateConverter::convertRpc(uint32_t detUnitId, int16_t strip) {

const RPCDetId id(detUnitId);
const RPCRoll* roll = geoRpc->roll(id);
/*
std::cout<<__FUNCTION__<<":"<<__LINE__
<<" detUnitId: "<<detUnitId
<<id
<<" roll:  "<<" "<<roll<<std::endl;
*/
const LocalPoint lp = roll->centreOfStrip(strip);
const GlobalPoint gp = roll->toGlobal(lp);

float globalPhi = gp.phi().value();
return globalPhi;

}

//copied from L1TriggerDPGUpgrade/L1TMuon/interface/GeometryTranslator.cc
float MtfCoordinateConverter::convertCsc(uint32_t detUnitId, uint16_t halfstrip, uint16_t pattern, uint16_t keyWG) {
    const CSCDetId id(detUnitId);
    // we should change this to weak_ptrs at some point
    // requires introducing std::shared_ptrs to geometry
    std::unique_ptr<const CSCChamber> chamb(geoCsc->chamber(id));
    std::unique_ptr<const CSCLayerGeometry> layer_geom(
            chamb->layer(CSCConstants::KEY_ALCT_LAYER)->geometry()
    );
    std::unique_ptr<const CSCLayer> layer(
            chamb->layer(CSCConstants::KEY_ALCT_LAYER)
    );

    //const unsigned maxStrips = layer_geom->numberOfStrips();

    // so we can extend this later
    // assume TMB2007 half-strips only as baseline
    double offset = 0.0;
    switch(1) {
    case 1:
        offset = CSCPatternLUT::get2007Position(pattern);
    }
    const unsigned halfstrip_offs = unsigned(0.5 + halfstrip + offset);
    const unsigned strip = halfstrip_offs/2 + 1; // geom starts from 1

    // the rough location of the hit at the ALCT key layer
    // we will refine this using the half strip information
    const LocalPoint coarse_lp =
            layer_geom->stripWireGroupIntersection(strip,keyWG);
    const GlobalPoint coarse_gp = layer->surface().toGlobal(coarse_lp);

    // the strip width/4.0 gives the offset of the half-strip
    // center with respect to the strip center
    const double hs_offset = layer_geom->stripPhiPitch()/4.0;

    // determine handedness of the chamber
    const bool ccw = isCSCCounterClockwise(layer);
    // we need to subtract the offset of even half strips and add the odd ones
    const double phi_offset = ( ( halfstrip_offs%2 ? 1 : -1)*
            ( ccw ? -hs_offset : hs_offset ) );

    // the global eta calculation uses the middle of the strip
    // so no need to increment it
    const GlobalPoint final_gp( GlobalPoint::Polar( coarse_gp.theta(),
            (coarse_gp.phi().value() +
                    phi_offset),
                    coarse_gp.mag() ) );

    // We need to add in some notion of the 'error' on trigger primitives
    // like the width of the wire group by the width of the strip
    // or something similar

    // release ownership of the pointers
    chamb.release();
    layer_geom.release();
    layer.release();

    float globalPhi = final_gp.phi().value();
    return globalPhi;
}

float MtfCoordinateConverter::convertDt(uint32_t detUnitId, int16_t phi) {
    DTChamberId baseid(detUnitId);

    int16_t sectorOffset = M_PI/6. * (baseid.sector()) * DT_PHI_BINS;
    //std::cout<<__FUNCTION__<<":"<<__LINE__<<" DT  "<<" "<<std::dec<<baseid<<" sectorOffset "<<sectorOffset<<std::endl;

    float val = ((float)(phi + sectorOffset))/(DT_PHI_BINS);

    return  val;

}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
uint32_t  MtfCoordinateConverter::getLayerNumber(uint32_t rawId){

  uint32_t aLayer = 0;
  
  DetId detId(rawId);
  if (detId.det() != DetId::Muon) std::cout << "PROBLEM: hit in unknown Det, detID: "<<detId.det()<<std::endl;


  switch (detId.subdetId()) {
  case MuonSubdetId::RPC: {
    RPCDetIdUtil aIdUtil(rawId);
    aLayer = aIdUtil.layer() + 10*(!aIdUtil.isBarrel());
  }
    break;
  case MuonSubdetId::DT: {
    DTChamberId dt(rawId);
    aLayer = dt.station();
    break;
  }
  case MuonSubdetId::CSC: {
    CSCDetId csc(rawId);
    aLayer = csc.station();
    break;
  }
  }  

 return aLayer;
}
//////////////////////////////////////////////////
//////////////////////////////////////////////////
