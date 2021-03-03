#pragma once

#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <cstdlib>
#include "math.h"

class KalmanAHRS
{
public:
    void changeQ(const double newQ[6]);
    void changeR(const double newR[6]);
    double operator[](unsigned int i);
    KalmanAHRS();
    double *run(const double akcelerometer[3], const double gyroskope[3], double angleRollInRadians, double anglePitchInRadians);
protected:
    const double dt = {0.01};
    const double g = {9.81};
    double gyroskopeVariation = {0.005};
    double acceleretionVectorLength = {0};
    double acceleretionVectorLengthSquared = {0};
    double A[6][6] = {
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 1}};
    double X[6] = {
        0,
        0,
        -9.81,
        0,
        0,
        0};
    double P[6][6] = {
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1}};
    double Q[6] = {
        2.5,
        2.5,
        2.5,
        0.001,
        0.001,
        0.001};
    double R[3] = {
        0.0005,
        0.0005,
        0.0005};
    double z[6] = {
        0,
        0,
        0,
        0,
        0,
        0};
    double w[6] = {
        0,
        0,
        0,
        0,
        0,
        0};
    void countErrorForNewPreditedState(double result[6][6]);
    void countNewInputData(const double akc[3], const double gyro[3], double result[6]);
    void countInverseCovariance(double Pkp[6][6]);
    void countKalmanGain(double Pkp[6][6], double result[6][6]);
    void countNewState(double K[6][6], double Yk[6]);
    void countNewErrorForState(double K[6][6], double Pkp[6][6]);
    void countMatrixA(const double acceleromiterMesurements[3], double roolAngle, double pitchAngle);
    void countPreditedState();
};
