#pragma once
#include <GPIO.hh>
#include <BothChannelEngineDriver.hh>
#include <US-015.hh>
#include <EncoderMenager.hh>
#include <AHRS.hh>
#include <PID.hh>
#include <SPIparser.hh>
#include <BatteryObserver.hh>

namespace PlatformDrivers{

struct DriversPlatform{
    Drivers::PWM_Ifc* engineRight = {nullptr};
    Drivers::PWM_Ifc* engineLeft = {nullptr};
    PlatformDrivers::EncoderMenager* encoderMenager = {nullptr};
    Drivers::US_015*  distanceFront = {nullptr};
    Drivers::US_015* distanceRight = {nullptr};
    Drivers::US_015* distanceLeft = {nullptr};
    Drivers::GPIO* gpio = {nullptr};
    AHRS* ahrs = {nullptr};
    PID* pidEngineMove = {nullptr};
    PID* pidEngineRotate = {nullptr};
    BatteryObserver* batteryObserver = {nullptr};
};

#define VALIDATE_DRIVER(X) \
if(!driversPlatform.X){ \
    log.error("%s: Missing driver '%s'", __func__, #X); \
    return false; \
}

class ModeStrategy{
public:
    virtual ~ModeStrategy(){}
    virtual void runSpiInterfaceLoop(DriversPlatform& driversPlatform, InterfaceSpiData& spiInterfaceData) = 0;
    virtual void runPlatformLoop(DriversPlatform& driversPlatform, InterfaceSpiData& spiInterfaceData) = 0;
    virtual const char* getName() = 0;
    virtual bool validate(DriversPlatform& driversPlatform) = 0;
};

}