#include "TuneMovePID.hh"

namespace PlatformDrivers{

TuneMovePidMode::TuneMovePidMode(Log& _log):
    log(_log),
    percentOfPower(0.0),
    previousValue(percentOfPower),
    counter(1){}

const char* TuneMovePidMode::getName(){
    return "TuneMovePidMode";
}

bool TuneMovePidMode::validate(DriversPlatform& driversPlatform){
    VALIDATE_DRIVER(batteryObserver);
    VALIDATE_DRIVER(engineLeft);
    VALIDATE_DRIVER(engineRight);
    VALIDATE_DRIVER(encoderMenager);
    VALIDATE_DRIVER(gpio);
    VALIDATE_DRIVER(pidEngineRotate);
    return true;
}

void TuneMovePidMode::runSpiInterfaceLoop(DriversPlatform& drivers, InterfaceSpiData& data){
    data.setValuePID = data.actualPosition.x;
    percentOfPower = data.getValuePID;
    if(percentOfPower > 100.0)
        percentOfPower = 100.0;
    else if(percentOfPower < -100.0)
        percentOfPower = -100.0;
    if(previousValue != percentOfPower){
        log.info("PID set value: %d", int(percentOfPower));
        previousValue = percentOfPower;
        counter = 400;
    }
    data.batteryVoltage[0] = drivers.batteryObserver->batteryVoltageVal(0);
    data.batteryVoltage[1] = drivers.batteryObserver->batteryVoltageVal(1);
    drivers.gpio->pin(OutputList::LED_RED, true);
}

void TuneMovePidMode::runPlatformLoop(DriversPlatform& drivers, InterfaceSpiData& data){
    drivers.encoderMenager->run();
    data.actualPosition = drivers.encoderMenager->get();
    drivers.pidEngineRotate->setR(
        data.actualPosition.angleDiff(Position(data.actualPosition.x,
            data.actualPosition.y, data.expectedPosition.rotation), 20.0));
    drivers.pidEngineRotate->setY(0.0);
    double pidOut = drivers.pidEngineRotate->calculate();
    drivers.engineRight->set(percentOfPower + pidOut);
    drivers.engineLeft->set( percentOfPower - pidOut);
    if(counter != 0){
        log.info("PID in->out: %d -> %d", int(percentOfPower), int(data.actualPosition.x));
        counter--;
    }
    drivers.gpio->pin(OutputList::LED_BLUE, true);
    drivers.gpio->pin(OutputList::LED_GREEN, true);
}

}