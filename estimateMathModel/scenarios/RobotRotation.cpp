#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <plot.hh>
#include <Models.hh>
#include <utility>
#include <vector>
#include <string>
#include <PID.hh>
#include <Models.hh>

namespace{

struct Measurements{
    std::vector<double> time;
    std::vector<double> input;
    std::vector<double> output;
};

Measurements loadDataFromFile(const std::string& fileName, unsigned int ignoreFirstLines = 0){
    std::vector<double> input;
    std::vector<double> output;
    std::vector<double> time;
    std::ifstream file(fileName);
    double timeStamp = 0.0;
    if(!file.good())
        throw std::invalid_argument("File is not open correct.");
    std::string ignoredLine;
    for(unsigned int i = 0; i < ignoreFirstLines; i++)
        std::getline(file, ignoredLine);
    while(!file.eof()){
        double tmp;
        file >> tmp;
        if(file.eof())
            break;
        input.push_back(tmp);
        file >> tmp;
        output.push_back(tmp);
        time.push_back(timeStamp);
        timeStamp += 0.01;
    }
    double previousData = output[0];
    const double correction = output[0];
    double offset = 0.0;
    for(auto& elem : output){
        const double delta = previousData - elem;
        previousData = elem;
        if(delta < -300)
            offset -= 360;
        else if(delta > 300)
            offset += 360;
        elem += offset - correction;
    }
    file.close();
    return Measurements{time, input, output};
}

TEST(RobotRotation, measurement){
    auto measurements = loadDataFromFile("./dataFiles/measureWithWheels1.txt", 1);
    plot::figure_size(1200, 780);
    plot::named_plot("Angle(deg)", measurements.time, measurements.output);
    plot::named_plot("power(%)", measurements.time, measurements.input);
    plot::title("PID axis H.");
    plot::legend();
    plot::save("./mathTestsOutputData/RobotRotation_measurement.png");
    plot::figure_close();
}

// #define MODEL new models::RealIntegral(1.8, 0.2218)
#define MODEL new models::RealIntegral(new models::Inercion(1.0, 0.1124), 1.9, 0.1127)

TEST(RobotRotation, model){
    auto measurements = loadDataFromFile("./dataFiles/measureWithWheels1.txt", 1);
    auto nmeasurements = loadDataFromFile("./dataFiles/measureWithWheels2.txt", 1);
    std::unique_ptr<models::Model> model;
    std::vector<double> modelResponse;
    std::vector<double> nmodelResponse;
    model.reset(MODEL);
    for(const auto& elem : nmeasurements.input){
        model->input(elem);
        nmodelResponse.push_back(model->output());
    }
    model.reset(MODEL);
    for(const auto& elem : measurements.input){
        model->input(elem);
        modelResponse.push_back(model->output());
    }
    plot::figure_size(1200, 780);
    plot::named_plot("Angle measured(deg)", measurements.time, measurements.output);
    plot::named_plot("power(%)", measurements.time, measurements.input);
    plot::named_plot("Angle model(%)", measurements.time, modelResponse);
    plot::named_plot("Negative Angle measured(deg)", nmeasurements.time, nmeasurements.output);
    plot::named_plot("Negative power(%)", nmeasurements.time, nmeasurements.input);
    plot::named_plot("Negative Angle model(%)", nmeasurements.time, nmodelResponse);
    plot::title("PID axis H.");
    plot::legend();
    plot::save("./mathTestsOutputData/RobotRotation_model.png");
    plot::figure_close();
}

TEST(RobotRotation, pid){
    auto measurementsTime = loadDataFromFile("./dataFiles/measureWithWheels2.txt", 1).time;
    std::unique_ptr<models::Model> model;
    std::vector<double> modelResponse;
    std::vector<double> setValueVec;
    std::vector<double> pidOutput;
    double setValue = 90.0;
    model.reset(MODEL);
    PID pid;
    constexpr double dt = 0.01;
    pid.P = 1.3686;
    pid.I = 0.93263 * dt;
    pid.D = 0.5209 / dt;
    pid.AntiWindup = 1.4;
    pid.setLimit(-100.0, 100.0);
    pid.setR(setValue);
    for(unsigned int i = 0; i < measurementsTime.size(); i++){
        pid.setY(model->output());
        pidOutput.push_back(pid.calculate());
        model->input(pidOutput.back());
        modelResponse.push_back(model->output());
        setValueVec.push_back(setValue);
    }
    plot::figure_size(1200, 780);
    plot::named_plot("set value", measurementsTime, setValueVec);
    plot::named_plot("response", measurementsTime, modelResponse);
    plot::named_plot("PID Out", measurementsTime, pidOutput);
    plot::title("PID axis H.");
    plot::legend();
    plot::save("./mathTestsOutputData/RobotRotation_pid.png");
    plot::figure_close();
}

}
