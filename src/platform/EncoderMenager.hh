#pragma once
#include "EncoderParserIfc.hh"
#include <Position.hh>

namespace PlatformDrivers{

class EncoderMenager{
public:
    struct Dimension{
        double wheelDiameter = 1.0;
        double wheelsDistance = 1.0;
    };
    EncoderMenager(EncoderParserIfc* const _encoderRight, EncoderParserIfc* const _encoderLeft, const Dimension& _dimension);
    void run();
    void setPointData(const Position& _position);
    const Position& get();
private:
    EncoderParserIfc* const encoderRight;
    EncoderParserIfc* const encoderLeft;
    const Dimension dimension;
    const double wheelPerimeter;
    Position position;
};

}
