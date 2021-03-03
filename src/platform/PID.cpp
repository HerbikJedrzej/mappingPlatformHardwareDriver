#include "PID.hh"

PID::PID():
    P(0),
    I(0),
    D(0),
    AntiWindup(0),
    r(0),
    u(0),
    e(0),
    eSum(0),
    ePast(0),
    y(0){}

void PID::setR(const double r1){
    r = r1;
}

void PID::setY(const double y1){
    y = y1;
}

void PID::setLimit(const double min, const double max){
    minOut = min;
    maxOut = max;
}

double PID::getSum(){
    return eSum;
}

void PID::reset(const double val){
    eSum = val;
}

double PID::calculate()
{
    ePast = e;
    e = r - y;
    eSum += e + AntiWindup * antiWindupCorrection;
    const double eDelta = (e - ePast);
    toReturn = P * e + I * eSum + D * eDelta;
    if(maxOut > minOut){
        const double beforeLimiting = toReturn;
        max(toReturn, maxOut);
        min(toReturn, minOut);
        antiWindupCorrection = toReturn - beforeLimiting;
    }
    return toReturn;
}

void PID::min(double& s1, const double& s2){
    if(s1 < s2)
        s1 = s2;
}
void PID::max(double &s1, const double &s2){
    if(s1 > s2)
        s1 = s2;
}
