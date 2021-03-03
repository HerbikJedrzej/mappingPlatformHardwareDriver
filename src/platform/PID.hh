#pragma once

class PID{
public:
    PID();
    double calculate();
    void setR(const double r1);
    void setY(const double y1);
    void setLimit(const double min, const double max); // default disable
    void reset(const double val = 0);
    double getSum();
    double P;
    double I;
    double D;
    double AntiWindup;
protected:
    double minOut = {0};
    double maxOut = {0};
private:
    void min(double &s1, const double &s2);
    void max(double &s1, const double &s2);
    double r;
    double u;
    double e;
    double eSum;
    double ePast;
    double y;
    double toReturn = {0};
    double antiWindupCorrection;
};
