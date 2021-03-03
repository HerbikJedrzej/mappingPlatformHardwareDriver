#include "ModelEngine.hh"

namespace Drivers{

ModelEngine::ModelEngine(models::Model* _model):
    setValue(0),
    model(_model){}

ModelEngine::~ModelEngine(){
}

uint8_t ModelEngine::size(){
    return 1;
}

void ModelEngine::set(const double percent, const uint8_t&){
    setValue = percent;
}

double ModelEngine::run(){
    model->input(setValue);
    return model->output();
}

}
