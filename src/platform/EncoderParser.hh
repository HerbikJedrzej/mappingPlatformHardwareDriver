#pragma once
#include <cstdint>
#include <stdlib.h>
#include <EncoderParserIfc.hh>

namespace PlatformDrivers{

template<class CounterType>
class EncoderParser : public EncoderParserIfc{
public:
    EncoderParser(const CounterType& _encoderRegister, const CounterType _maxCounterRegister, const double _fullRotation);
    double run() override;
private:
    int32_t countDifference(CounterType counterRegister);
    int32_t countOverflowedDifference(CounterType counterRegister);
    const CounterType& encoderRegister;
    CounterType previousCounterRegister;
    const CounterType maxCounterRegister;
    const double fullRotation;
};

template<class CounterType>
EncoderParser<CounterType>::EncoderParser(const CounterType& _encoderRegister, const CounterType _maxCounterRegister, const double _fullRotation):
    encoderRegister(_encoderRegister),
    previousCounterRegister(_encoderRegister),
    maxCounterRegister(_maxCounterRegister),
    fullRotation(_fullRotation){}

template<class CounterType>
int32_t EncoderParser<CounterType>::countDifference(CounterType counterRegister){
    const int diff1 = counterRegister - previousCounterRegister;
    const int diff2 = countOverflowedDifference(counterRegister);
    return (abs(diff1) > abs(diff2))? diff2 : diff1;
}

template<class CounterType>
int32_t EncoderParser<CounterType>::countOverflowedDifference(CounterType counterRegister){
    if(counterRegister < previousCounterRegister)
        return ((counterRegister + maxCounterRegister + 1) - previousCounterRegister);
    return (counterRegister - (previousCounterRegister + maxCounterRegister + 1));
}

template<class CounterType>
double EncoderParser<CounterType>::run(){
    const int registerDifference = countDifference(encoderRegister);
    previousCounterRegister = encoderRegister;
    return double(registerDifference) / fullRotation * 360;
}

}
