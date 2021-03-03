#pragma once
#include "StrategySPI_ifc.hh"
#include <Log.hh>

namespace StrategySpi{

class TransmissionKeyStrategy : public StrategySpiIfc{
    PlatformDrivers::Log* log;
    const uint8_t keyR;
    const uint8_t keyT;
public:
    TransmissionKeyStrategy(const uint8_t& keyRecived, const uint8_t& keyTransmitted, PlatformDrivers::Log* _log = nullptr, StrategySpiIfc* next = nullptr);
    bool set(StrategySpiData& data, StrategySpiIfc** lastRun) override;
    bool get(StrategySpiData& data, StrategySpiIfc** lastRun) override;
    unsigned int getSize() override;
    const char* getName() override;
};

}