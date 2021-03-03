#pragma once

#include "PWM_Ifc.hh"
#include <Models.hh>

namespace Drivers{

class ModelEngine : public PWM_Ifc{
private:
    double setValue;
    std::unique_ptr<models::Model> model;
public:
    double run();
    ModelEngine(models::Model* _model);
    ~ModelEngine();
    uint8_t size() override;
    void set(const double percent, const uint8_t& engine = 0) override;
};

}
