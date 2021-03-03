#pragma once
#include "StrategySPI_ifc.hh"
#include <Position.hh>

namespace StrategySpi{

class PositionStrategy : public StrategySpiIfc{
    double* actualPositionX;
    double* actualPositionY;
    RotationAngle* actualRotation;
    double* expectedPositionX;
    double* expectedPositionY;
    RotationAngle* expectedRotation;
    bool* applyPosition;
public:
    PositionStrategy(
        double* _actualPositionX,
        double* _actualPositionY,
        RotationAngle* _actualRotation,
        double* _expectedPositionX,
        double* _expectedPositionY,
        RotationAngle* _expectedRotation,
        bool* _applyPosition,
        StrategySpiIfc* next = nullptr
    );
    bool set(StrategySpiData& data, StrategySpiIfc** lastRun) override;
    bool get(StrategySpiData& data, StrategySpiIfc** lastRun) override;
    unsigned int getSize() override;
    const char* getName() override;
};

}