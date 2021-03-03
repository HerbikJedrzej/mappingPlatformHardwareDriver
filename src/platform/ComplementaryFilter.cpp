#include "ComplementaryFilter.hh"

#define M_PI       3.14159265358979323846   // pi

double ComplementaryFilter::expandRangeOfAngle(double complementary, double mesured)
{
    int i = ((int)complementary) / (2 * M_PI);
    if ((2 * M_PI) * i + mesured - complementary > (1.7 * M_PI))
        i--;
    if ((2 * M_PI) * i + mesured - complementary < (-1.7 * M_PI))
        i++;
    return (2 * M_PI) * i + mesured;
}
double ComplementaryFilter::complementaryOneOfThem(const unsigned int witchAngle, double angle, double angleVelocity)
{
    if (witchAngle == 2)
        angle = expandRangeOfAngle(complementaryPastAngles[2], angle);
    angle = A * angle + B * (complementaryPastAngles[witchAngle] + angleVelocity * dt);
    complementaryPastAngles[witchAngle] = angle;
    return angle;
}
void ComplementaryFilter::run(const double mesuredAkcelerometer[3], const double mesuredGyroskope[3], const double mesuredMegnetometer[3], const double& rollOffset, const double& pitchOffset)
{
    filtredAngle[0] = atan2(-mesuredAkcelerometer[1], -mesuredAkcelerometer[2]);
    filtredAngle[0] = complementaryOneOfThem(0, filtredAngle[0], mesuredGyroskope[0]) + rollOffset;

    filtredAngle[1] = atan2(cos(filtredAngle[0]) * mesuredAkcelerometer[0], -mesuredAkcelerometer[2]);
    filtredAngle[1] = complementaryOneOfThem(1, filtredAngle[1], mesuredGyroskope[1]) + pitchOffset;

    if(mesuredMegnetometer == nullptr){
        const double tmpA = A;
        A = 0;
        B = 1;
        filtredAngle[2] = complementaryOneOfThem(2, filtredAngle[2], mesuredGyroskope[2]);
        A = tmpA;
        B = 1 - tmpA;
        return;
    }
    filtredAngle[2] = atan2(
            sin(filtredAngle[0]) * mesuredMegnetometer[2] - cos(filtredAngle[0]) * mesuredMegnetometer[1]
        ,
            cos(filtredAngle[1]) * mesuredMegnetometer[0] +
            sin(filtredAngle[0]) * sin(filtredAngle[1]) * mesuredMegnetometer[1] +
            cos(filtredAngle[0]) * sin(filtredAngle[1]) * mesuredMegnetometer[2]
    );
    filtredAngle[2] = complementaryOneOfThem(2, filtredAngle[2], mesuredGyroskope[2]);
}
double ComplementaryFilter::operator[](unsigned int i)
{
    return filtredAngle[i];
}

void ComplementaryFilter::changeA(const double newA)
{
    A = newA;
    B = 1.0 - A;
}
