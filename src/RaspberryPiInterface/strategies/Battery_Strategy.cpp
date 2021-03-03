#include "Battery_Strategy.hh"

namespace StrategySpi{

BatteryStrategy::BatteryStrategy(
        double* _actualVoltage,
        double* _tuneVoltageValue,
        const unsigned int _numberOfCells,
        StrategySpiIfc* next
    ):
    StrategySpiIfc(next),
    actualVoltage(_actualVoltage),
    tuneVoltageValue(_tuneVoltageValue),
    currentUsedCell(0),
    numberOfCells(_numberOfCells){
        if(numberOfCells > 8)
            THROW_out_of_range("Max number of battery cells could be 8.");
    }

bool BatteryStrategy::set(StrategySpiData& data, StrategySpiIfc** lastRun){
    *lastRun = this;

    uint16_t rawValue = 0;
    if((actualVoltage[currentUsedCell]) >= 40.96)
        rawValue = 0x1fff;
    else if((actualVoltage[currentUsedCell]) > 0.0)
        rawValue = uint16_t(actualVoltage[currentUsedCell] * 200.0) & 0x1fff;
    rawValue |= (currentUsedCell << 13) & 0x0e000;

    data.data[0] = uint8_t((rawValue >> 8) & 0xff);
    data.data[1] = uint8_t(rawValue & 0xff);

    currentUsedCell++;
    if(currentUsedCell >= numberOfCells)
        currentUsedCell = 0;

    data.dataSize -= 2;
    data.data += 2;
    return (next)? next->set(data, lastRun) : true;
}

bool BatteryStrategy::get(StrategySpiData& data, StrategySpiIfc** lastRun){
    *lastRun = this;

    uint16_t rawData = (data.data[0] << 8) | data.data[1];
    uint8_t cell = (data.data[0] >> 5) & 0b111;
    if(cell >= numberOfCells)
        THROW_out_of_range("Get information about cell out of range. Max allowed is 8.");
    tuneVoltageValue[cell] = double(rawData & 0x1fff) * 0.005;

    data.dataSize -= 2;
    data.data += 2;
    return (next)? next->get(data, lastRun) : true;
}

unsigned int BatteryStrategy::getSize(){
    return (next)? 2 + next->getSize() : 2;
}

const char* BatteryStrategy::getName(){
    return "BatteryStrategy";
}

}