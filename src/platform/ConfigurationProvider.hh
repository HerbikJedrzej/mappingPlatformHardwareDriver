#pragma once
#include <Log.hh>
#include <I2C_Ifc.hh>
#include <OwnExceptions.hh>

namespace Configuration{

struct PIDvalues{
    double p;
    double i;
    double d;
    double antiWindUp;
};

struct Configuration{
    uint8_t spiKeyTx;
    uint8_t spiKeyRx;
    uint8_t mode;
    PIDvalues pidRotation;
    PIDvalues pidForward;
    uint8_t numberOfBatteryCells;
    double maxMeasuredVoltage;
    uint32_t batteryAdcMaxWord;
    uint32_t* upperResistors_Ohm = {nullptr};
    uint32_t* lowerResistors_Ohm = {nullptr};
    ~Configuration(){
        if(upperResistors_Ohm)
            delete [] upperResistors_Ohm;
        if(lowerResistors_Ohm)
            delete [] lowerResistors_Ohm;
    }
};

}

class ConfigurationProvider{
public:
    ConfigurationProvider(PlatformDrivers::Log& _log, const uint8_t deviceAddress, Drivers::I2C_Ifc* const _i2c);
    bool run(Configuration::Configuration& config);
protected:
    virtual bool readingList(Configuration::Configuration& config);
    template <typename Type>
    union Converter{
        Type value;
        uint8_t array[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    };
    template <typename Type>
    bool getFromMemory(const uint16_t& regAddr, Type& variable, const char* failComment = "none"){
        const unsigned int size = sizeof(Type);
        if(size > 8)
            THROW_invalid_argument("Unsupported convertion type.");
        Converter<Type> converter;
        if(Drivers::BusStatus::OK != i2c->read(deviceAddr, regAddr, true, converter.array, size)){
            log.error("Reading '%s' faled.", failComment);
            return false;
        }
        variable = converter.value;
        return true;
    }
private:
    bool isKeyWrong();
    PlatformDrivers::Log& log;
    const uint8_t deviceAddr;
    Drivers::I2C_Ifc* const i2c;
};
