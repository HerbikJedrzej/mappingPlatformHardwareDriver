#pragma once

#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <cstdlib>
#include <math.h>

class ComplementaryFilter
{
public:
    void changeA(const double newA);
    void run(const double mesuredAkcelerometer[3], const double mesuredGyroskope[3], const double mesuredMegnetometer[3], const double& rollOffset, const double& pitchOffset);
    double operator[](unsigned int i);
protected:
    double complementaryPastAngles[3] = {0, 0, 0};
    const double dt = 0.01;
    double A = 0.01;
    double B = 1.0 - A;
    double filtredAngle[3] = {0, 0, 0};
    double expandRangeOfAngle(double complementary, double mesured);
    double complementaryOneOfThem(const unsigned int witchAngle, double angle, double angleVelocity);
};
