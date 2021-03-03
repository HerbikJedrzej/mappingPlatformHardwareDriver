#pragma once

#include <EncoderParserIfc.hh>

namespace PlatformDrivers{

class ModelEncoderParser : public EncoderParserIfc{
public:
    ModelEncoderParser(const double initValue):
        value(initValue),
        previousValue(initValue){}
    double run() override{
        const double diff = value - previousValue;
        previousValue = value;
        return diff;
    }
    void setValue(const double& val){
        value = val;
    }
private:
    double value;
    double previousValue;
};

}
