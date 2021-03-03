#include "EncoderMenager.hh"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace PlatformDrivers{

EncoderMenager::EncoderMenager(EncoderParserIfc* const _encoderRight, EncoderParserIfc* const _encoderLeft, const Dimension& _dimension):
    encoderRight(_encoderRight),
    encoderLeft(_encoderLeft),
    dimension(_dimension),
    wheelPerimeter(M_PI * dimension.wheelDiameter){}

void EncoderMenager::run(){
    const double rightWheelDistance = wheelPerimeter * encoderRight->run() / 360.0;
    const double leftWheelDistance = wheelPerimeter * encoderLeft->run() / 360.0;
    const double beta = position.rotation * M_PI / 180.0;
    double psi = 0;
    double positionDifference = rightWheelDistance;
    if(rightWheelDistance != leftWheelDistance){
        psi = (rightWheelDistance - leftWheelDistance) / dimension.wheelsDistance;
        positionDifference = 2 * ((rightWheelDistance + leftWheelDistance) / (2 * psi)) * sin(psi / 2);
        position.rotation += psi * 180.0 / M_PI;
    }else if(rightWheelDistance == 0.0)
        return;
    position.x += positionDifference * cos(beta + psi / 2);
    position.y += positionDifference * sin(beta + psi / 2);
}

const Position& EncoderMenager::get(){
    return position;
}

void EncoderMenager::setPointData(const Position& _position){
    position = _position;
}

}
