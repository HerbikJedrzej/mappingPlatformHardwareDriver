#include "ConfigurationProvider.hh"

#define READ_PARAMETER(ADDR,VARIABLE) if(!getFromMemory(ADDR,VARIABLE,#VARIABLE))return false;

ConfigurationProvider::ConfigurationProvider(PlatformDrivers::Log& _log, const uint8_t deviceAddress, Drivers::I2C_Ifc* const _i2c):
    log(_log),
    deviceAddr(deviceAddress),
    i2c(_i2c){}


bool ConfigurationProvider::isKeyWrong(){
    uint8_t key[10];
    Drivers::BusStatus status = i2c->read(deviceAddr, 0x0000, true, key, 10);
    if(Drivers::BusStatus::OK != status){
        log.error("EEprom key reading failure.");
        return true;
    }
    if((key[0] == 0x15) && (key[1] == 0x26) && (key[2] == 0x37) && (key[3] == 0x48) && (key[4] == 0x59) &&
       (key[5] == 0x6a) && (key[6] == 0x7b) && (key[7] == 0x8c) && (key[8] == 0x9d) && (key[9] == 0xae))
            return false;
    log.error("EEprom key dosen't match.");
    log.error("Expected: 15263748596A7B8C9DAE");
    log.error("Recived:  %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",
        key[0], key[1], key[2], key[3], key[4], 
        key[5], key[6], key[7], key[8], key[9]);
    return true;
}

bool ConfigurationProvider::run(Configuration::Configuration& config){
    if(isKeyWrong())
        return false;
    if(!readingList(config))
        return false;
    log.info("All config are loaded from EEprom.");
    return true;
}

bool ConfigurationProvider::readingList(Configuration::Configuration& config){
    READ_PARAMETER(0x000a, config.spiKeyRx);
    READ_PARAMETER(0x000b, config.spiKeyTx);
    READ_PARAMETER(0x000c, config.mode);

    READ_PARAMETER(0x0100, config.pidForward.p);
    READ_PARAMETER(0x0108, config.pidForward.i);
    READ_PARAMETER(0x0110, config.pidForward.d);
    READ_PARAMETER(0x0118, config.pidForward.antiWindUp);
    READ_PARAMETER(0x0120, config.pidRotation.p);
    READ_PARAMETER(0x0128, config.pidRotation.i);
    READ_PARAMETER(0x0130, config.pidRotation.d);
    READ_PARAMETER(0x0138, config.pidRotation.antiWindUp);

    READ_PARAMETER(0x0203, config.numberOfBatteryCells);
    READ_PARAMETER(0x0204, config.maxMeasuredVoltage);
    READ_PARAMETER(0x020c, config.batteryAdcMaxWord);
    config.upperResistors_Ohm = new uint32_t[config.numberOfBatteryCells];
    config.lowerResistors_Ohm = new uint32_t[config.numberOfBatteryCells];
    for(uint8_t i = 0; i < config.numberOfBatteryCells; i++){
        READ_PARAMETER(0x0210 + i * 8, config.upperResistors_Ohm[i]);
        READ_PARAMETER(0x0214 + i * 8, config.lowerResistors_Ohm[i]);
    }
    if(config.numberOfBatteryCells > 3)
        log.warning("%s: Used more cells (%d) than 3.", __func__, config.numberOfBatteryCells);
    return true;
}
