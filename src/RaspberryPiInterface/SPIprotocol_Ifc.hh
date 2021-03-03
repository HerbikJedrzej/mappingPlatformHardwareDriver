#pragma once
#include <cstdint>

class SPIprotocol_Ifc{
public:
    virtual void setSize(const uint16_t& _size) = 0;
    virtual uint8_t* getTxData() = 0;
    virtual uint8_t* getRxData() = 0;
    virtual uint16_t size() = 0;
    virtual bool isReadyToUse() = 0;
    virtual void markUsed() = 0;
};