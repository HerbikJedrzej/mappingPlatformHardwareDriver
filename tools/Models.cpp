#include <Models.hh>
#include <tgmath.h>
#include <algorithm>
#include <cmath>

namespace models{

Model::Model(Model* _model, const double _K):\
    y(0),
    K(_K),
    model(_model){}

double Model::output(){
    return K * y;
}

void Model::input(const double value){
    if(model){
        model->input(value);
        this->_input(model->output());
    }
    else
        this->_input(value);
}

double Model::getTimeStep(){
    return dt;
}

Engine::Engine(const double _treshold, const double _A):
    Model(nullptr, 1),
    treshold(_treshold / 100.0),
    A(_A / 100.0),
    a((0.5 - (1.0 - A)/(1 - treshold)) / (1.0- A*A - (1.0 + treshold)*(1.0 - A))),
    b((1.0 - a*(1.0 - treshold*treshold)) / (1.0 - treshold)),
    c(1.0 - a -b){}

void Engine::_input(const double value){
    bool negative = (value < 0.0);
    const double x = negative? -value / 100.0 : value / 100.0;
    if(x > 1)
        y = negative? -100.0 : 100.0;
    else if(x > treshold){
        y = a * x * x + b * x + c;
        y *= negative? -100.0 : 100.0;
    }
    else
        y = 0.0;
}

Inercion::Inercion(const double _K, const double _T, const double p0):
    Model(nullptr, _K),
    T(1 / _T){
        y = p0 / _K;
    }

Inercion::Inercion(Model* _model, const double _K, const double _T, const double p0):
    Model(_model, _K),
    T(1 / _T){
        y = p0 / _K;
    }

void Inercion::_input(const double value){
    y += T * (value - y) * dt;
}

Integral::Integral(const double _K):
    Model(nullptr, _K){}

Integral::Integral(Model* _model, const double _K):
    Model(_model, _K){}

void Integral::_input(const double value){
    y += value * dt;
}

RealIntegral::RealIntegral(const double _K, const double _T, const double p0):
    Model(new Inercion(_K, _T, p0), 1){}

RealIntegral::RealIntegral(Model* _model, const double _K, const double _T, const double p0):
    Model(new Inercion(_model, _K, _T, p0), 1){}

void RealIntegral::_input(const double value){
    y += value * dt;
}

Proporcional::Proporcional(const double _K):
    Model(nullptr, _K){}

Proporcional::Proporcional(Model* _model, const double _K):
    Model(_model, _K){}

void Proporcional::_input(const double value){
    y = value;
}

LimitUp::LimitUp(const double max):
    Model(nullptr, 1),
    limit(max){}

LimitUp::LimitUp(Model* _model, const double max):
    Model(_model, 1),
    limit(max){}

void LimitUp::_input(const double value){
    y = (value > limit)? limit : value;
}

LimitDown::LimitDown(const double min):
    Model(nullptr, 1),
    limit(min){}

LimitDown::LimitDown(Model* _model, const double min):
    Model(_model, 1),
    limit(min){}

void LimitDown::_input(const double value){
    y = (value < limit)? limit : value;
}

Limit::Limit(const double max, const double min):
    Model(nullptr, 1),
    limitU(max),
    limitD(min){}

Limit::Limit(Model* _model, const double max, const double min):
    Model(_model, 1),
    limitU(max),
    limitD(min){}

void Limit::_input(const double value){
    if(value > limitU)
        y = limitU;
    else if(value < limitD)
        y = limitD;
    else
        y = value;
}

Derivative::Derivative(const double _K):
    Model(nullptr, _K),
    yPast(0){}

Derivative::Derivative(Model* _model, const double _K):
    Model(_model, _K),
    yPast(0){}

void Derivative::_input(const double value){
    y = (value - yPast) / dt;
    yPast = value;
}

RealDerivative::RealDerivative(const double _K, const double _T, const double p0):
    Model(new Inercion(_K, _T, p0), 1),
    yPast(0){}

RealDerivative::RealDerivative(Model* _model, const double _K, const double _T, const double p0):
    Model(new Inercion(_model, _K, _T, p0), 1),
    yPast(0){}

void RealDerivative::_input(const double value){
    y = (value - yPast) / dt;
    yPast = value;
}

Delay::Delay(const double sec):
    Model(nullptr, 1),
    delay((sec <= 0)? 1.0 : sec / dt),
    data(new double[delay]),
    counter(0){
        for(unsigned int i = 0; i < delay; i++)
            data[i] = 0;
    }

Delay::Delay(Model* _model, const double sec):
    Model(_model, 1),
    delay((sec <= 0)? 1.0 : sec / dt),
    data(new double[delay]),
    counter(0){
        for(unsigned int i = 0; i < delay; i++)
            data[i] = 0;
    }

Delay::~Delay(){
    delete [] data;
}

void Delay::_input(const double value){
    y = data[counter];
    data[counter] = value;
    counter++;
    if(counter == delay)
        counter = 0;
}

Oscilation::Oscilation(const double K, const double T, const double _E, const double p0):
    Oscilation(nullptr, K, T, _E, p0){
        y = p0 / K;
    }

Oscilation::Oscilation(Model* _model, const double K, const double T, const double _E, const double p0):
    Model(_model, K),
    y1(0, 0),
    E(std::min<double>(std::abs(_E) + dt/(2*T), 1)),
    re(E * std::abs(T)),
    im(std::abs(T) * sqrt(1.0 - E*E)),
    div(re*re+im*im),
    T1(re/div, im/div),
    T2(re/div, -im/div){
        y = p0 / K;
    }

void Oscilation::_input(const double value){
    y1 += T1 * (ComplexNumbers(value, 0) - y1)  * dt;
    y += (T2 * (y1 - ComplexNumbers(y, 0)) * dt).getR();
}

Oscilation::ComplexNumbers::ComplexNumbers(const double _r, const double _i):
    real(_r),
    img(_i){}

double Oscilation::ComplexNumbers::getR() const{
    return real;
}

double Oscilation::ComplexNumbers::getI() const{
    return img;
}

Oscilation::ComplexNumbers Oscilation::ComplexNumbers::operator+(const ComplexNumbers& val) const{
    return ComplexNumbers(real + val.getR(), img + val.getI());
}

void Oscilation::ComplexNumbers::operator+=(const ComplexNumbers& val){
    real += val.getR();
    img += val.getI();
}

void Oscilation::ComplexNumbers::operator=(const ComplexNumbers& val){
    real = val.getR();
    img = val.getI();
}

Oscilation::ComplexNumbers Oscilation::ComplexNumbers::operator-(const ComplexNumbers& val) const{
    return ComplexNumbers(real - val.getR(), img - val.getI());
}

Oscilation::ComplexNumbers Oscilation::ComplexNumbers::operator*(const ComplexNumbers& val) const{
    return ComplexNumbers(real * val.getR() - img * val.getI(), real * val.getI() + img * val.getR());
}

Oscilation::ComplexNumbers Oscilation::ComplexNumbers::operator*(const double& val) const{
    return ComplexNumbers(real * val, img * val);
}

Step::Step(const double& stepVal, const double& stepTime):
    Step(nullptr, stepVal, stepTime)
    {}

Step::Step(Model* _model, const double& stepVal, const double& stepTime):
    Model(_model, 1),
    val(stepVal),
    sTime(stepTime),
    time(0)
    {}

void Step::_input(const double value){
    y = (time < sTime)? value : value + val;
    time += dt;
}

Peak::Peak(const double& peakTime, const double& K):
    Peak(nullptr, peakTime, K)
    {}

Peak::Peak(Model* _model, const double& peakTime, const double& K):
    Model(_model, 1),
    val(K),
    pTime(peakTime),
    time(0)
    {}

void Peak::_input(const double value){
    y = (fabs(time - pTime) <= 0.00000000001)? value + 1/dt * val : value;
    time += dt;
}

Sinus::Sinus(const double& amplitude, const double& _T, const double& phi, const double& startTime):
    Sinus(nullptr, amplitude, _T, phi, startTime){}

Sinus::Sinus(Model* _model, const double& amplitude, const double& _T, const double& phi, const double& startTime):
    Model(_model, 1),
    time(0),
    A(amplitude),
    period(_T),
    shift(phi),
    sTime(startTime)
    {}

void Sinus::_input(const double value){
    y = (time < sTime)? value : value + A * sin(2 * M_PI * period * time + shift);
    time += dt;
}

Data::Data(const std::vector<double>& _data):
    Data(nullptr, _data){}

Data::Data(Model* _model, const std::vector<double>& _data):
    Model(_model, 1),
    data(_data),
    counter(0){}

void Data::_input(const double value){
    if(counter >= data.size())
        throw std::out_of_range("Data is used. Trying to read element out of data nr = " + std::to_string(counter));
    y = value + data[counter];
    counter++;
}

std::pair<std::vector<double>, std::vector<double>> simulate(models::Model* const model, const double startTime, const double endTime){
    if(!model)
        throw std::system_error();
    if(endTime < startTime)
        throw std::out_of_range(
            "Start time is bigger than end time. Start time = "
            + std::to_string(startTime)
            + "' end time = "
            + std::to_string(endTime)
        );
    std::pair<std::vector<double>, std::vector<double>> toReturn;
    std::vector<double>& t = toReturn.first;
    std::vector<double>& v = toReturn.second;
    unsigned int steps = (unsigned int)((endTime - startTime) / model->getTimeStep());
    for(unsigned int i = 0; i < steps; i++){
        t.push_back((double)i * model->getTimeStep() + startTime);
        model->input(0);
        v.push_back(model->output());
    }
    return toReturn;
}
std::pair<std::vector<double>, std::vector<double>> simulate(models::Model* const  model, const double time){
    return simulate(model, 0, time);
}

std::pair<std::vector<double>, std::vector<double>> simulate(const unsigned int length, models::Model* const  model){
    if(!model)
        throw std::system_error();
    std::pair<std::vector<double>, std::vector<double>> toReturn;
    std::vector<double>& t = toReturn.first;
    std::vector<double>& v = toReturn.second;
    for(unsigned int i = 0; i < length; i++){
        t.push_back((double)i * model->getTimeStep());
        model->input(0);
        v.push_back(model->output());
    }
    return toReturn;
}

}