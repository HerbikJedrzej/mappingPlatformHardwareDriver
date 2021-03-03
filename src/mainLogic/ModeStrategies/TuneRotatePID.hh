#pragma once

#include <Log.hh>
#include "ModeStrategy_ifc.hh"

namespace PlatformDrivers{

class TuneRotatePidMode : public ModeStrategy{
    Log& log;
    double percentOfPower;
    double previousValue;
    unsigned int counter;
    double angle;
public:
    TuneRotatePidMode(Log& _log);
    void runSpiInterfaceLoop(DriversPlatform& driversPlatform, InterfaceSpiData& spiInterfaceData) override;
    void runPlatformLoop(DriversPlatform& driversPlatform, InterfaceSpiData& spiInterfaceData) override;
    const char* getName() override;
    bool validate(DriversPlatform& driversPlatform) override;
};

}