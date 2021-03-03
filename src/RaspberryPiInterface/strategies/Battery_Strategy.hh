#pragma once
#include "StrategySPI_ifc.hh"

namespace StrategySpi{

class BatteryStrategy : public StrategySpiIfc{
    double* actualVoltage;
    double* tuneVoltageValue;
    unsigned int currentUsedCell;
    const unsigned int numberOfCells; 
public:
    BatteryStrategy(
        double* _actualVoltage,
        double* _tuneVoltageValue,
        const unsigned int _numberOfCells,
        StrategySpiIfc* next = nullptr
    );
    bool set(StrategySpiData& data, StrategySpiIfc** lastRun) override;
    bool get(StrategySpiData& data, StrategySpiIfc** lastRun) override;
    unsigned int getSize() override;
    const char* getName() override;
};

}