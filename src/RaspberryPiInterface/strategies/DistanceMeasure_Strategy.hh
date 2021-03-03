#pragma once
#include "StrategySPI_ifc.hh"

namespace StrategySpi{

class DistanceMeasureStrategy : public StrategySpiIfc{
    double* distance;
public:
    DistanceMeasureStrategy(double* _distance, StrategySpiIfc* next = nullptr);
    bool set(StrategySpiData& data, StrategySpiIfc** lastRun) override;
    bool get(StrategySpiData& data, StrategySpiIfc** lastRun) override;
    unsigned int getSize() override;
    const char* getName() override;
};

}