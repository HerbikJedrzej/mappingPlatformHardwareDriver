#include "AHRS.hh"
#include <OwnExceptions.hh>

#define M_PI 3.14159265358979323846 // pi

AHRS::AHRS(double* _akceletometer, double* _gyroskope, double* _magnetometer, const double rotationZ):
    akceletometer(_akceletometer),
    gyroskope(_gyroskope),
    magnetometer(_magnetometer),
    stepingFilter(),
    complementaryFilter(),
    kalmanFilter(),
    sinAngleZ(sin(rotationZ * M_PI / 180.0)),
    cosAngleZ(cos(rotationZ * M_PI / 180.0))
    {
        complementaryFilter.changeA(0.01);
    }

double AHRS::operator[](unsigned int i){
    if( i > 2 )
        THROW_out_of_range("There is only roll, pitch and yawl angles.");
    if( i == 2 )
        return 180 * complementaryFilter[2] / M_PI;
    if( i == 0 )
        return ( 180 * ( cosAngleZ * complementaryFilter[0] - sinAngleZ * complementaryFilter[1] ) / M_PI );
    return ( 180 * ( sinAngleZ * complementaryFilter[0] + cosAngleZ * complementaryFilter[1] ) / M_PI );
}

void AHRS::run(const double& rollDegOffset, const double& pitchDegOffset){
    stepingFilter.run(akceletometer);
    const double* const variance = stepingFilter.getVariance();
    kalmanFilter.changeR(variance);
    complementaryFilter.run(
        kalmanFilter.run(
            stepingFilter.getFiltredValues(),
            gyroskope,
            complementaryFilter[0],
            complementaryFilter[1]
        ),
        gyroskope,
        magnetometer,
        M_PI / 180.0 * rollDegOffset,
        M_PI / 180.0 * pitchDegOffset
    );
    const double sr = sin(complementaryFilter[0]);
    const double cr = cos(complementaryFilter[0]);
    const double sp = sin(complementaryFilter[1]);
    const double cp = cos(complementaryFilter[1]);
    horizontalAkceleration = (-sp * stepingFilter[0] + sr * cp * stepingFilter[1] + cr * cp * stepingFilter[2]);
    horizontalAkcelerationVariation =
        sp * sp *           variance[0] +
        sr * cp * sr * cp * variance[1] +
        cr * cp * cr * cp * variance[2];
}

double AHRS::getHorizontalAkceleration(){
    return horizontalAkceleration;
}

double AHRS::getHorizontalAkcelerationVariation(){
    return horizontalAkcelerationVariation;
}
