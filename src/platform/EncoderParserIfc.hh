#pragma once
#include <cstdint>
#include <stdlib.h>

namespace PlatformDrivers{

class EncoderParserIfc{
public:
    virtual double run() = 0;
};

}
