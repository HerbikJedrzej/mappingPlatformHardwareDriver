#include "SPIparser.hh"
#include <OwnExceptions.hh>
#include "strategies/TransmissionKey_Strategy.hh"
#include "strategies/AllDataUsed_Strategy.hh"
#include "strategies/Position_Strategy.hh"
#include "strategies/Battery_Strategy.hh"
#include "strategies/DistanceMeasure_Strategy.hh"
#include "strategies/Double_Strategy.hh"

#define SET_FLAG(X) executedMethods[flags::X] = true

SPIparser::SPIparser(SPIprotocol_Ifc* const _spiProtocol, InterfaceSpiData& interfaceData):
    log(interfaceData.log),
    spiProtocol(_spiProtocol),
    strategies(getStrategiesList(interfaceData)),
    readyToWrite(false){
        uint16_t size = strategies->getSize();
        if(!spiProtocol)
            THROW_invalid_argument("SPI protocol driver is nullptr.");
        log.info("SPI protocol size changed form %d to %d", spiProtocol->size(), size);
        spiProtocol->setSize(size--);
        strategies->addNextStrategy(
            new StrategySpi::AllDataUsedStrategy(spiProtocol->getTxData() + size, spiProtocol->getRxData() + size)
        );
        log.info("Used strategies:");
        strategies->printStrategies(log);
    }


void SPIparser::run(){
    if(spiProtocol->isReadyToUse()){
        spiProtocol->markUsed();
        parseIncomingData();
        if(readyToWrite)
            log.error("Data were not send by SPI.");
        readyToWrite = true;
    }
    else if(readyToWrite){
        readyToWrite = false;
        parseOutgoingData();
    }
}

void SPIparser::parseIncomingData(){
    StrategySpi::StrategySpiData data{spiProtocol->getRxData(), spiProtocol->size()};
    StrategySpi::StrategySpiIfc* lastRun = nullptr;
    if(!strategies->get(data, &lastRun))
        log.error("Failes get method in: %s", lastRun->getName());
}

void SPIparser::parseOutgoingData(){
    StrategySpi::StrategySpiData data{spiProtocol->getTxData(), spiProtocol->size()};
    StrategySpi::StrategySpiIfc* lastRun = nullptr;
    if(!strategies->set(data, &lastRun))
        log.error("Failes set method in: %s", lastRun->getName());
}

StrategySpi::StrategySpiIfc* SPIparser::getStrategiesList(InterfaceSpiData& data){
    StrategySpi::StrategySpiIfc* strategy =
        new StrategySpi::TransmissionKeyStrategy(data.keyRecived, data.keySent, &data.log);
    switch(data.mode){
        case 0: {
            Position& act = data.actualPosition;
            Position& exp = data.expectedPosition;
            data.batteryCells = 2;
            data.tuneVoltage = new double[2];
            data.tuneVoltage[0] = 0.0;
            data.tuneVoltage[1] = 0.0;
            data.batteryVoltage = new double[2];
            data.batteryVoltage[0] = 0.0;
            data.batteryVoltage[1] = 0.0;
            data.distanceMeasureSize = 3;
            data.distanceMeasure = new double[3];
            data.distanceMeasure[0] = 0.0;
            data.distanceMeasure[1] = 0.0;
            data.distanceMeasure[2] = 0.0;
            strategy->addNextStrategy(new StrategySpi::PositionStrategy(
                &act.x, &act.y, &act.rotation,
                &exp.x, &exp.y, &exp.rotation,
                &data.applyPosition));
            strategy->addNextStrategy(new StrategySpi::BatteryStrategy(
                data.batteryVoltage, data.tuneVoltage, 2));
            strategy->addNextStrategy(new StrategySpi::DistanceMeasureStrategy(
                &data.distanceMeasure[0]));
            strategy->addNextStrategy(new StrategySpi::DistanceMeasureStrategy(
                &data.distanceMeasure[1]));
            strategy->addNextStrategy(new StrategySpi::DistanceMeasureStrategy(
                &data.distanceMeasure[2]));
            return strategy;
        }
        case 1: {
            data.batteryCells = 2;
            data.tuneVoltage = new double[2];
            data.tuneVoltage[0] = 0.0;
            data.tuneVoltage[1] = 0.0;
            data.batteryVoltage = new double[2];
            data.batteryVoltage[0] = 0.0;
            data.batteryVoltage[1] = 0.0;
            strategy->addNextStrategy(new StrategySpi::DoubleStrategy(
                &data.setValuePID, &data.getValuePID));
            strategy->addNextStrategy(new StrategySpi::BatteryStrategy(
                data.batteryVoltage, data.tuneVoltage, 2));
            return strategy;
       }
        case 2: {
            data.batteryCells = 2;
            data.tuneVoltage = new double[2];
            data.tuneVoltage[0] = 0.0;
            data.tuneVoltage[1] = 0.0;
            data.batteryVoltage = new double[2];
            data.batteryVoltage[0] = 0.0;
            data.batteryVoltage[1] = 0.0;
            strategy->addNextStrategy(new StrategySpi::DoubleStrategy(
                &data.setValuePID, &data.getValuePID));
            strategy->addNextStrategy(new StrategySpi::BatteryStrategy(
                data.batteryVoltage, data.tuneVoltage, 2));
            return strategy;
        }
        default:
            THROW_invalid_argument("Unsupported SPI interface mode.");
    }
}