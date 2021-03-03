#include "TuneRotatePID.hh"

namespace PlatformDrivers{

TuneRotatePidMode::TuneRotatePidMode(Log& _log):
    log(_log),
    percentOfPower(0.0),
    previousValue(percentOfPower),
    counter(1),
    angle(0.0){}

const char* TuneRotatePidMode::getName(){
    return "TuneRotatePidMode";
}

bool TuneRotatePidMode::validate(DriversPlatform& driversPlatform){
    VALIDATE_DRIVER(batteryObserver);
    VALIDATE_DRIVER(engineLeft);
    VALIDATE_DRIVER(engineRight);
    VALIDATE_DRIVER(encoderMenager);
    VALIDATE_DRIVER(gpio);
    return true;
}

void TuneRotatePidMode::runSpiInterfaceLoop(DriversPlatform& drivers, InterfaceSpiData& data){
    data.setValuePID = angle;
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

void TuneRotatePidMode::runPlatformLoop(DriversPlatform& drivers, InterfaceSpiData&){
    drivers.encoderMenager->run();
    angle = drivers.encoderMenager->get().rotation;
    drivers.engineRight->set(percentOfPower);
    drivers.engineLeft->set(-percentOfPower);
    if(counter != 0){
        log.info("PID in->out: %d -> %d", int(percentOfPower), int(angle));
        counter--;
    }
    drivers.gpio->pin(OutputList::LED_BLUE, true);
    drivers.gpio->pin(OutputList::LED_GREEN, true);
}

}