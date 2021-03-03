#include "TransmissionKey_Strategy.hh"

namespace StrategySpi{

TransmissionKeyStrategy::TransmissionKeyStrategy(const uint8_t& keyRecived, const uint8_t& keyTransmitted, PlatformDrivers::Log* _log, StrategySpiIfc* next):
    StrategySpiIfc(next),
    log(_log),
    keyR(keyRecived),
    keyT(keyTransmitted){}

bool TransmissionKeyStrategy::set(StrategySpiData& data, StrategySpiIfc** lastRun){
    *lastRun = this;
    data.data[0] = keyT;
    data.dataSize--;
    data.data++;
    return (next)? next->set(data, lastRun) : true;
}

bool TransmissionKeyStrategy::get(StrategySpiData& data, StrategySpiIfc** lastRun){
    *lastRun = this;
    if(data.data[0] != keyR){
        if(log)
            log->warning("Wrong key. Expected: 0x%02X recived: 0x%02X", keyR, data.data[0]);
        return false;
    }
    data.dataSize--;
    data.data++;
    return (next)? next->get(data, lastRun) : true;
}

unsigned int TransmissionKeyStrategy::getSize(){
    return (next)? 1 + next->getSize() : 1;
}

const char* TransmissionKeyStrategy::getName(){
    return "TransmissionKeyStrategy";
}

}