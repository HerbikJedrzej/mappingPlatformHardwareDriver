#include "StrategyManager.hh"
#include "ModeStrategies/NormalMode.hh"
#include "ModeStrategies/TuneMovePID.hh"
#include "ModeStrategies/TuneRotatePID.hh"

namespace PlatformDrivers{

ModeManager::ModeManager(DriversPlatform& _driversPlatform, InterfaceSpiData& _spiInterfaceData):
    driversPlatform(_driversPlatform),
    spiInterfaceData(_spiInterfaceData)
{
    Log& log = spiInterfaceData.log;
    switch(spiInterfaceData.mode){
        case 0:
            strategy = new NormalMode(log);
            break;
        case 1:
            strategy = new TuneRotatePidMode(log);
            break;
        case 2:
            strategy = new TuneMovePidMode(log);
            break;
        default:
            strategy = nullptr;
            break;
    }
    if(strategy){
        log.info("%s: Mode: %s.", __func__, strategy->getName());
        if(!strategy->validate(driversPlatform)){
            delete strategy;
            strategy = nullptr;
            log.error("%s: Strategy do not pass validation.", __func__);
        }
    }
    else
        log.error("%s: Unknown mode value %d", __func__, spiInterfaceData.mode);
}

void ModeManager::runSpiInterfaceLoop(){
    if(strategy)
        strategy->runSpiInterfaceLoop(driversPlatform, spiInterfaceData);
}

void ModeManager::runPlatformLoop(){
    if(strategy)
        strategy->runPlatformLoop(driversPlatform, spiInterfaceData);
}

}