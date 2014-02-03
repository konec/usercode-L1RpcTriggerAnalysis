/*
 * MtfCoordinateConverter.h
 *
 *  Created on: Oct 15, 2013
 *      Author: Karol Bunkowski
 */

#ifndef MTFCOORDINATECONVERTER_H_
#define MTFCOORDINATECONVERTER_H_

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"

class MtfCoordinateConverter {
public:
    static const int DT_PHI_BINS = 4096;
    static const int DT_PHIB_BINS = 512;
private:
    const edm::EventSetup& es;

    edm::ESHandle<CSCGeometry> geoCsc;
    edm::ESHandle<DTGeometry>  geoDt;
    edm::ESHandle<RPCGeometry>  geoRpc;

    float phiref;

public:
    MtfCoordinateConverter(const edm::EventSetup& es);
    virtual ~MtfCoordinateConverter();

    uint32_t getLayerNumber(uint32_t rawId);

    void setReferencePhi(float aPhiRef);

    float getReferencePhi() const {return phiref; };

    int convert(std::pair<uint32_t,  unsigned int > aData);
    float convertRpc(uint32_t detUnitId, int16_t strip);
    float convertCsc(uint32_t detUnitId, uint16_t halfstrip, uint16_t pattern, uint16_t keyWG);
    float convertDt(uint32_t detUnitId, int16_t phi);
};

#endif /* MTFCOORDINATECONVERTER_H_ */
