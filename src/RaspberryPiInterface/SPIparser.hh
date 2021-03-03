#pragma once
#include "SPIprotocol_Ifc.hh"
#include <Log.hh>
#include "strategies/StrategySPI_ifc.hh"
#include <Position.hh>

struct InterfaceSpiData{
    PlatformDrivers::Log& log;
    const uint8_t keySent;
    const uint8_t keyRecived;
    const uint8_t mode;
    Position actualPosition;
    Position expectedPosition;
    bool applyPosition = {false};
    unsigned int distanceMeasureSize = {0};
    double* distanceMeasure = {nullptr};
    unsigned int batteryCells = {0};
    double* tuneVoltage = {nullptr};
    double* batteryVoltage = {nullptr};
    double setValuePID, getValuePID = {0};
    ~InterfaceSpiData(){
        if(distanceMeasure)
            delete [] distanceMeasure;
        if(tuneVoltage)
            delete [] tuneVoltage;
        if(batteryVoltage)
            delete [] batteryVoltage;
    }
};

class SPIparser{
protected:
    PlatformDrivers::Log& log;
    SPIprotocol_Ifc* const spiProtocol;
    StrategySpi::StrategySpiIfc* strategies;
    bool readyToWrite;
public:
    SPIparser(SPIprotocol_Ifc* const _spiProtocol, InterfaceSpiData& interfaceData);
    void run();
protected:
    StrategySpi::StrategySpiIfc* getStrategiesList(InterfaceSpiData& interfaceData);
    void parseIncomingData();
    void parseOutgoingData();
};
