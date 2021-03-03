#pragma once

#include "Kalman.hh"
#include "ComplementaryFilter.hh"
#include "StepingMeanFilter.hh"

class AHRS{
public:
    AHRS(double* _akceletometer, double* _gyroskope, double* _magnetometer, const double rotationZ = 0);
    double operator[](unsigned int i);
    void run(const double& rollDegOffset = 0.0, const double& pitchDegOffset = 0.0);
    double getHorizontalAkceleration();
    double getHorizontalAkcelerationVariation();
protected:
    double* akceletometer;
    double* gyroskope;
    double* magnetometer;
    StepingMeanFilter stepingFilter;
    ComplementaryFilter complementaryFilter;
    KalmanAHRS kalmanFilter;
    const double sinAngleZ;
    const double cosAngleZ;
    double horizontalAkceleration;
    double horizontalAkcelerationVariation;
};
