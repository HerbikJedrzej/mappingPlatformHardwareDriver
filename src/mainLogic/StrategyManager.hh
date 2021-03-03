#pragma once
#include <stdint.h>
#include <Log.hh>
#include <ModeStrategies/ModeStrategy_ifc.hh>
#include <SPIparser.hh>

namespace PlatformDrivers{

class ModeManager{
    ModeStrategy* strategy;
    DriversPlatform& driversPlatform;
    InterfaceSpiData& spiInterfaceData;
public:
    ModeManager(DriversPlatform& _driversPlatform, InterfaceSpiData& _spiInterfaceData);
    void runSpiInterfaceLoop();
    void runPlatformLoop();
};

}