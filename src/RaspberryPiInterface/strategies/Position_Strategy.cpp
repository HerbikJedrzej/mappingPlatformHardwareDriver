#include "Position_Strategy.hh"

namespace StrategySpi{

PositionStrategy::PositionStrategy(
        double* _actualPositionX,
        double* _actualPositionY,
        RotationAngle* _actualRotation,
        double* _expectedPositionX,
        double* _expectedPositionY,
        RotationAngle* _expectedRotation,
        bool* _applyPosition,
        StrategySpiIfc* next
    ):
    StrategySpiIfc(next),
    actualPositionX(_actualPositionX),
    actualPositionY(_actualPositionY),
    actualRotation(_actualRotation),
    expectedPositionX(_expectedPositionX),
    expectedPositionY(_expectedPositionY),
    expectedRotation(_expectedRotation),
    applyPosition(_applyPosition){}

bool PositionStrategy::set(StrategySpiData& data, StrategySpiIfc** lastRun){
    *lastRun = this;

    int32_t raw = (*actualPositionX) * 1000;
    data.data[0] = (raw >> 24) & 0xff;
    data.data[1] = (raw >> 16) & 0xff;
    data.data[2] = (raw >> 8) & 0xff;
    data.data[3] = raw & 0xff;
    raw = (*actualPositionY) * 1000;
    data.data[4] = (raw >> 24) & 0xff;
    data.data[5] = (raw >> 16) & 0xff;
    data.data[6] = (raw >> 8) & 0xff;
    data.data[7] = raw & 0xff;
    raw = (((*actualRotation) < 360.0) && ((*actualRotation) >= 0.0))? 
        double(0x8000) * (*actualRotation) / 360.0 : 0;
    data.data[8] = (*applyPosition)? 0x80 : 0x00;
    data.data[8] |= (raw >> 8) & 0x7f;
    data.data[9] = raw & 0xff;

    data.dataSize -= 10;
    data.data += 10;
    return (next)? next->set(data, lastRun) : true;
}

bool PositionStrategy::get(StrategySpiData& data, StrategySpiIfc** lastRun){
    *lastRun = this;

    *applyPosition = ((data.data[8] & 0x80) != 0);
    *expectedPositionX = double(int32_t((data.data[0] << 24) | (data.data[1] << 16) | (data.data[2] << 8) | data.data[3])) / 1000;
    *expectedPositionY = double(int32_t((data.data[4] << 24) | (data.data[5] << 16) | (data.data[6] << 8) | data.data[7])) / 1000;
    *expectedRotation = double(uint16_t(((data.data[8] & 0x7f) << 8) | data.data[9])) * 360.0 / double(0x8000);

    data.dataSize -= 10;
    data.data += 10;
    return (next)? next->get(data, lastRun) : true;
}

unsigned int PositionStrategy::getSize(){
    return (next)? 10 + next->getSize() : 10;
}

const char* PositionStrategy::getName(){
    return "PositionStrategy";
}

}