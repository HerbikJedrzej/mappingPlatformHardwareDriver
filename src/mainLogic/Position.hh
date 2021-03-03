#pragma once

class RotationAngle{
    double fix(double a) const;
    double angle;
public:
    RotationAngle();
    RotationAngle(const double& _angle);

    RotationAngle(const RotationAngle&) = default;
    RotationAngle(RotationAngle&&) = default;
    RotationAngle& operator=(const RotationAngle&) = default;
    RotationAngle& operator=(RotationAngle&&) = default;
    virtual ~RotationAngle() = default;

    operator double() const;

    void operator=(const double& val);

    void operator+=(const double& val);
    void operator-=(const double& val);
    double operator!=(const double& val) const;
    double operator==(const double& val) const;

    double operator!=(const RotationAngle& val) const;
    double operator==(const RotationAngle& val) const;
};

class Position{
public:
    Position();
    Position(const double& X, const double& Y, const double& R);

    Position(const Position&) = default;
    Position(Position&&) = default;
    Position& operator=(const Position&) = default;
    Position& operator=(Position&&) = default;
    virtual ~Position() = default;

    static void setPointPrecision(const double& precision);

    double angleDiff(const Position& expectedPosition, const double& acceptableDistance = 0.0) const;
    double dislocation(const Position& expectedPosition) const;

    double x;
    double y;
    RotationAngle rotation;
private:
    double chooseAngle(const Position& expectedPosition, const double& acceptableDistance) const;
    static double pointPrecision;
};
