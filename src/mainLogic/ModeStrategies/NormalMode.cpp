#include "NormalMode.hh"
#include <Position.hh>

namespace PlatformDrivers{

NormalMode::NormalMode(Log& _log):
    log(_log){}

const char* NormalMode::getName(){
    return "NormalMode";
}

bool NormalMode::validate(DriversPlatform& driversPlatform){
    VALIDATE_DRIVER(engineRight);
    VALIDATE_DRIVER(engineLeft);
    VALIDATE_DRIVER(encoderMenager);
    VALIDATE_DRIVER(distanceFront);
    VALIDATE_DRIVER(distanceRight);
    VALIDATE_DRIVER(distanceLeft);
    VALIDATE_DRIVER(gpio);
    VALIDATE_DRIVER(ahrs);
    VALIDATE_DRIVER(pidEngineMove);
    VALIDATE_DRIVER(pidEngineRotate);
    VALIDATE_DRIVER(batteryObserver);
    return true;
}

void NormalMode::runSpiInterfaceLoop(DriversPlatform& drivers, InterfaceSpiData& data){
    data.distanceMeasure[0] = drivers.distanceFront->getDistanceInMilimeters();
    data.distanceMeasure[1] = drivers.distanceRight->getDistanceInMilimeters();
    data.distanceMeasure[2] = drivers.distanceLeft->getDistanceInMilimeters();
    drivers.gpio->pin(OutputList::LED_RED, true);
}

void NormalMode::runPlatformLoop(DriversPlatform& drivers, InterfaceSpiData& data){
    drivers.encoderMenager->run();
    data.actualPosition = drivers.encoderMenager->get();
    // drivers.pidEngineMove->setY(drivers.encoderMenager->get().x);
    drivers.pidEngineRotate->setR(data.actualPosition.angleDiff(data.expectedPosition, 20.0));
    drivers.pidEngineRotate->setY(0.0);
    drivers.pidEngineMove->setR(data.actualPosition.dislocation(data.expectedPosition));
    drivers.pidEngineMove->setY(0.0);
    const double pidOutMove = drivers.pidEngineMove->calculate();
    const double pidOutRotate = drivers.pidEngineRotate->calculate();
    drivers.engineRight->set(pidOutMove + pidOutRotate);
    drivers.engineLeft->set(pidOutMove - pidOutRotate);
    data.batteryVoltage[0] = drivers.batteryObserver->batteryVoltageVal(0);
    data.batteryVoltage[1] = drivers.batteryObserver->batteryVoltageVal(1);
    drivers.gpio->pin(OutputList::LED_BLUE, true);
    drivers.gpio->pin(OutputList::LED_GREEN, true);
}

}