#pragma once

#include <Log.hh>
#include "ModeStrategy_ifc.hh"

namespace PlatformDrivers{

class TuneMovePidMode : public ModeStrategy{
    Log& log;
    double percentOfPower;
    double previousValue;
    unsigned int counter;
public:
    TuneMovePidMode(Log& _log);
    void runSpiInterfaceLoop(DriversPlatform& driversPlatform, InterfaceSpiData& spiInterfaceData) override;
    void runPlatformLoop(DriversPlatform& driversPlatform, InterfaceSpiData& spiInterfaceData) override;
    const char* getName() override;
    bool validate(DriversPlatform& driversPlatform) override;
};

}