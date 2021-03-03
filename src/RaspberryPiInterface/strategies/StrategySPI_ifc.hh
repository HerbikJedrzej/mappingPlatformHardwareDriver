#pragma once
#include <cstdint>
#include <string>
#include <OwnExceptions.hh>
#include <Log.hh>

namespace StrategySpi{

struct StrategySpiData{
    uint8_t* data = {nullptr};
    unsigned int dataSize = {0};
};

class StrategySpiIfc{
protected:
    StrategySpiIfc* next;
public:
    StrategySpiIfc(StrategySpiIfc* _next):
        next(_next){}
    virtual bool set(StrategySpiData& data, StrategySpiIfc** lastRun) = 0;
    virtual bool get(StrategySpiData& data, StrategySpiIfc** lastRun) = 0;
    virtual unsigned int getSize() = 0;
    virtual const char* getName() = 0;
    virtual void printStrategies(PlatformDrivers::Log& log, const unsigned int& nr = 1){
        log.info("Strategy nr: %d - %s", nr, this->getName());
        if(next)
            next->printStrategies(log, nr + 1);
    }
    void addNextStrategy(StrategySpiIfc* _next){
        if(_next == this)
            THROW_invalid_argument("Cannot add strategy. It's alredy in pool");
        if(next)
            return next->addNextStrategy(_next);
        next = _next;
    }
    virtual ~StrategySpiIfc(){
        if(next)
            delete next;
    }
};

}