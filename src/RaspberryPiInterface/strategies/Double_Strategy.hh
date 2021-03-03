#pragma once
#include "StrategySPI_ifc.hh"

namespace StrategySpi{

class DoubleStrategy : public StrategySpiIfc{
    double* setValue;
    double* getValue;
    union Converter{
        uint8_t array[8];
        double val;
    } converter;
public:
    DoubleStrategy(
        double* _setValue,
        double* _getValue,
        StrategySpiIfc* next = nullptr
    );
    bool set(StrategySpiData& data, StrategySpiIfc** lastRun) override;
    bool get(StrategySpiData& data, StrategySpiIfc** lastRun) override;
    unsigned int getSize() override;
    const char* getName() override;
};

}