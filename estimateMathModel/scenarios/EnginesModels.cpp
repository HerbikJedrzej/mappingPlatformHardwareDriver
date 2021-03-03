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
    const double correction = output[0];
    for(auto& elem : output){
        elem -= correction;
    }
    file.close();
    return Measurements{time, input, output};
}

const std::vector<std::string> measurementsPerents{
    "100",
    "-100",
    "75",
    "-75",
    "50",
    "-50",
};

const std::vector<std::string>engines = {"R", "L"};

TEST(EngineModels, measurement100){
    for(const auto& engine : engines){
        plot::figure_size(1200, 780);
        for(const auto& percent : measurementsPerents){
            auto measurements = loadDataFromFile("./dataFiles/enginesMeasurement/engine" + engine + "_" + percent + ".txt", 1);
            plot::named_plot("Angle(deg)[" + percent + "%]", measurements.time, measurements.output);
            plot::named_plot("power(" + percent + "%)", measurements.time, measurements.input);
        }
        plot::title("Math models for " + engine + " engine");
        plot::legend();
        plot::save("./mathTestsOutputData/EngineModels" + engine + ".png");
        plot::figure_close();
    }
}

// #define MODEL new models::RealIntegral(1.8, 0.2218)

#define MODEL(K,A) \
new models::RealIntegral( \
    new models::Inercion( \
        new models::Engine(17.0, A) \
    ,-1.0, 0.1124) \
, K, 0.1127)

#define MODEL_R MODEL(3.12, 57.5)
#define MODEL_L MODEL(2.44, 63.5)

TEST(EnginesModels, model){
    for(const auto& engine : engines){
        plot::figure_size(1200, 780);
        for(const auto& percent : measurementsPerents){
            auto measurements = loadDataFromFile("./dataFiles/enginesMeasurement/engine" + engine + "_" + percent + ".txt", 1);
            std::unique_ptr<models::Model> model;
            std::vector<double> modelResponse;
            model.reset((engine == "R")? MODEL_R : MODEL_L);
            for(const auto& elem : measurements.input){
                model->input(elem);
                modelResponse.push_back(model->output());
            }
            plot::named_plot("Angle(deg)[" + percent + "%]", measurements.time, measurements.output);
            plot::named_plot("Angle model(deg)[" + percent + "%]", measurements.time, modelResponse);
        }
        plot::title("Models response for " + engine + " engine");
        plot::legend();
        plot::save("./mathTestsOutputData/EngineModels" + engine + "_modelResponse.png");
        plot::figure_close();
    }
}

}
