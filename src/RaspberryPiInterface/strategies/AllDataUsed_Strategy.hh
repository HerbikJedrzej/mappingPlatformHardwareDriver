#pragma once
#include "StrategySPI_ifc.hh"

namespace StrategySpi{

class AllDataUsedStrategy : public StrategySpiIfc{
    const uint8_t* lastTxElem;
    const uint8_t* lastRxElem;
public:
    AllDataUsedStrategy(const uint8_t* lastTransmittedElement = nullptr, const uint8_t* lastRecivedElement = nullptr);
    bool set(StrategySpiData& data, StrategySpiIfc** lastRun) override;
    bool get(StrategySpiData& data, StrategySpiIfc** lastRun) override;
    unsigned int getSize() override;
    const char* getName() override;
};

}