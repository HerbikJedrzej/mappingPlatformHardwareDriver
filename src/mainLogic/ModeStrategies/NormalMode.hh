#pragma once

#include <Log.hh>
#include "ModeStrategy_ifc.hh"

namespace PlatformDrivers{

class NormalMode : public ModeStrategy{
    Log& log;
public:
    NormalMode(Log& _log);
    void runSpiInterfaceLoop(DriversPlatform& driversPlatform, InterfaceSpiData& spiInterfaceData) override;
    void runPlatformLoop(DriversPlatform& driversPlatform, InterfaceSpiData& spiInterfaceData) override;
    const char* getName() override;
    bool validate(DriversPlatform& driversPlatform) override;
};

}