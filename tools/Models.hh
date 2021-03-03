#pragma once

#include <memory>
#include <vector>

namespace models{

class Model{
protected:
    virtual void _input(const double value) = 0;
    double y;
    const double K;
    std::shared_ptr<Model> model;
    const double dt{0.01};
public:
    Model(Model* _model, const double _K);
    Model(const Model&) = default;
    Model(Model&&) = default;
    Model& operator=(const Model&) = default;
    void input(const double value);
    virtual double output();
    double getTimeStep();
};

class Engine : public Model{
protected:
    void _input(const double value) override;
    const double treshold;
    const double A;
    const double a;
    const double b;
    const double c;
public:
    Engine(const double _treshold = 0.0, const double _A = 0.0);
    ~Engine() = default;
};

class Inercion : public Model{
protected:
    void _input(const double value) override;
    const double T;
public:
    Inercion(const double _K, const double _T, const double p0 = 0);
    Inercion(Model* _model, const double _K, const double _T, const double p0 = 0);
    ~Inercion() = default;
};

class Integral : public Model{
protected:
    void _input(const double value) override;
public:
    Integral(const double _K);
    Integral(Model* _model, const double _K);
    ~Integral() = default;
};
 
class RealIntegral : public Model{
protected:
    void _input(const double value) override;
public:
    RealIntegral(const double _K, const double _T, const double p0 = 0);
    RealIntegral(Model* _model, const double _K, const double _T, const double p0 = 0);
    ~RealIntegral() = default;
};

class Proporcional : public Model{
protected:
    void _input(const double value) override;
public:
    Proporcional(const double _K);
    Proporcional(Model* _model, const double _K);
    ~Proporcional() = default;
};

class LimitUp : public Model{
protected:
    void _input(const double value) override;
    const double limit;
public:
    LimitUp(const double max);
    LimitUp(Model* _model, const double max);
    ~LimitUp() = default;
};

class LimitDown : public Model{
protected:
    void _input(const double value) override;
    const double limit;
public:
    LimitDown(const double min);
    LimitDown(Model* _model, const double min);
    ~LimitDown() = default;
};

class Limit : public Model{
protected:
    void _input(const double value) override;
    const double limitU;
    const double limitD;
public:
    Limit(const double max, const double min);
    Limit(Model* _model, const double max, const double min);
    ~Limit() = default;
};

class Derivative : public Model{
protected:
    void _input(const double value) override;
    double yPast;
public:
    Derivative(const double _K);
    Derivative(Model* _model, const double _K);
    ~Derivative() = default;
};

class RealDerivative : public Model{ // Response for peak is diffren than in matlab
protected:
    void _input(const double value) override;
    double yPast;
public:
    RealDerivative(const double _K, const double _T, const double p0 = 0);
    RealDerivative(Model* _model, const double _K, const double _T, const double p0 = 0);
    ~RealDerivative() = default;
};

class Delay : public Model{
protected:
    void _input(const double value) override;
    unsigned int delay;
    double* data;
    unsigned int counter;
public:
    Delay(const double time);
    Delay(Model* _model, const double time);
    virtual ~Delay();
};

class Oscilation : public Model{
protected:
    void _input(const double value) override;
    class ComplexNumbers{
        double real;
        double img;
        public:
        ComplexNumbers(const double _r, const double _i);
        double getR() const;
        double getI() const;
        void operator=(const ComplexNumbers&);
        ComplexNumbers operator+(const ComplexNumbers&) const;
        void operator+=(const ComplexNumbers&);
        ComplexNumbers operator-(const ComplexNumbers&) const;
        ComplexNumbers operator*(const ComplexNumbers&) const;
        ComplexNumbers operator*(const double&) const;
    };
    ComplexNumbers y1;
    const double E;
    const double re;
    const double im;
    double div;
    const ComplexNumbers T1;
    const ComplexNumbers T2;
public:
    Oscilation(const double K, const double T, const double _E, const double p0 = 0);
    Oscilation(Model* _model, const double K, const double T, const double _E, const double p0 = 0);
    ~Oscilation() = default;
};

class Step : public Model{
    const double val;
    const double sTime;    
    double time;
    void _input(const double value) override;
public:
    Step(const double& stepVal, const double& stepTime);
    Step(Model* _model, const double& stepVal, const double& stepTime);
    ~Step() = default;
};

class Peak : public Model{
    const double val;
    const double pTime;    
    double time;
    void _input(const double value) override;
public:
    Peak(const double& peakTime, const double& K = 1);
    Peak(Model* _model, const double& peakTime, const double& K = 1);
    ~Peak() = default;
};

class Sinus : public Model{
    double time;
    const double A;
    const double period;
    const double shift;
    const double sTime;    
    void _input(const double value) override;
public:
    Sinus(const double& amplitude, const double& _T, const double& phi = 0, const double& startTime = 0);
    Sinus(Model* _model, const double& amplitude, const double& _T, const double& phi = 0, const double& startTime = 0);
    ~Sinus() = default;
};

class Data : public Model{
    const std::vector<double>& data;
    unsigned int counter;
    void _input(const double value) override;
public:
    Data(const std::vector<double>& _data);
    Data(Model* _model, const std::vector<double>& _data);
    ~Data() = default;
};

std::pair<std::vector<double>, std::vector<double>> simulate(models::Model* const model, const double startTime, const double endTime);
std::pair<std::vector<double>, std::vector<double>> simulate(models::Model* const  model, const double time);
std::pair<std::vector<double>, std::vector<double>> simulate(const unsigned int length, models::Model* const  model);

}