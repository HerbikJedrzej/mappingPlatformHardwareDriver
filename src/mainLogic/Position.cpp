#include "Position.hh"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

RotationAngle::RotationAngle():
    RotationAngle(0.0){}

RotationAngle::RotationAngle(const double& _angle):
    angle(fix(_angle)){}

double RotationAngle::fix(double a) const{
    while(a < 0){
        a += 360.0;
    }
    while(a >= 360.0){
        a -= 360.0;
    }
    return a;
}

RotationAngle::operator double() const{
    return angle;
}

void RotationAngle::operator=(const double& val){
    angle = fix(val);
}

void RotationAngle::operator+=(const double& val){
    angle = fix(angle + val);
}

void RotationAngle::operator-=(const double& val){
    angle = fix(angle - val);
}

double RotationAngle::operator!=(const double& val) const{
    return angle != val;
}

double RotationAngle::operator==(const double& val) const{
    return angle == val;
}

double RotationAngle::operator!=(const RotationAngle& val) const{
    return angle != val.angle;
}

double RotationAngle::operator==(const RotationAngle& val) const{
    return angle == val.angle;
}

double Position::pointPrecision = 1.0;

Position::Position():
    Position(0.0, 0.0, 0.0){}

Position::Position(const double& X, const double& Y, const double& R):
    x(X),
    y(Y),
    rotation(R){}

void Position::setPointPrecision(const double& precision){
    pointPrecision = precision;
}

double Position::angleDiff(const Position& expectedPosition, const double& acceptableDistance) const{
    const double angle = chooseAngle(expectedPosition, acceptableDistance);
    const double diff = (rotation > angle)?
        rotation - angle :
        angle - rotation;
    if(diff > 180.0){
        if(rotation < angle)
            return angle - rotation - 360.0;
        return angle - rotation + 360.0;
    }
    return angle - rotation;
}

double Position::dislocation(const Position& expectedPosition) const{
    const double diffX = expectedPosition.x - x;
    const double diffY = expectedPosition.y - y;
    const double angle = rotation * M_PI / 180.0;
    return diffX * cos(angle) + diffY * sin(angle);
}

double Position::chooseAngle(const Position& expectedPosition, const double& acceptableDistance) const{
    const double diffX = expectedPosition.x - x;
    const double diffY = expectedPosition.y - y;
    if((diffX * diffX + diffY * diffY) < (acceptableDistance * acceptableDistance))
        return expectedPosition.rotation;
    return 180.0 / M_PI * atan(diffY / diffX);
    // return 180.0 / M_PI * atan2(diffY, diffX);
}