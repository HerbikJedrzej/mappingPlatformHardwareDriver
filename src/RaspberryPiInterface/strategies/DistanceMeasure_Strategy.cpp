#include "DistanceMeasure_Strategy.hh"

namespace StrategySpi{

DistanceMeasureStrategy::DistanceMeasureStrategy(double* _distance, StrategySpiIfc* next):
    StrategySpiIfc(next),
    distance(_distance){}

bool DistanceMeasureStrategy::set(StrategySpiData& data, StrategySpiIfc** lastRun){
    *lastRun = this;
    uint16_t tmp = *distance * double(0xffff) / 2500.0;
    data.data[0] = (tmp >> 8) & 0xff;
    data.data[1] = tmp & 0xff;
    data.dataSize -= 2;
    data.data += 2;
    return (next)? next->set(data, lastRun) : true;
}

bool DistanceMeasureStrategy::get(StrategySpiData& data, StrategySpiIfc** lastRun){
    *lastRun = this;
    data.dataSize -= 2;
    data.data += 2;
    return (next)? next->get(data, lastRun) : true;
}

unsigned int DistanceMeasureStrategy::getSize(){
    return (next)? 2 + next->getSize() : 2;
}

const char* DistanceMeasureStrategy::getName(){
    return "DistanceMeasureStrategy";
}

}