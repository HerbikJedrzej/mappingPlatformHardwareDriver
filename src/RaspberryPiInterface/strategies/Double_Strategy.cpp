#include "Double_Strategy.hh"

namespace StrategySpi{

DoubleStrategy::DoubleStrategy(
        double* _setValue,
        double* _getValue,
        StrategySpiIfc* next
    ):
    StrategySpiIfc(next),
    setValue(_setValue),
    getValue(_getValue){}

bool DoubleStrategy::set(StrategySpiData& data, StrategySpiIfc** lastRun){
    *lastRun = this;

    converter.val = (setValue)? *setValue : 0.0;
    for(unsigned int i = 0; i < 8; i++)
        data.data[i] = converter.array[i];

    data.dataSize -= 8;
    data.data += 8;
    return (next)? next->set(data, lastRun) : true;
}

bool DoubleStrategy::get(StrategySpiData& data, StrategySpiIfc** lastRun){
    *lastRun = this;

    if(getValue){
        for(unsigned int i = 0; i < 8; i++)
            converter.array[i] = data.data[i];
        *getValue = converter.val;
    }

    data.dataSize -= 8;
    data.data += 8;
    return (next)? next->get(data, lastRun) : true;
}

unsigned int DoubleStrategy::getSize(){
    return (next)? 8 + next->getSize() : 8;
}

const char* DoubleStrategy::getName(){
    return "DoubleStrategy";
}

}