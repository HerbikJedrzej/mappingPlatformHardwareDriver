#include "StepingMeanFilter.hh"

StepingMeanFilter::StepingMeanFilter(){}

void StepingMeanFilter::run(const double newData[3]) {
    for(unsigned int i = 0; i < 3; i++){
        sum[i] -= memory[i][counter];
        memory[i][counter] = newData[i];
        sum[i] += memory[i][counter];
        toReturn[i] = sum[i] / STRENGTH_OF_STEPING_MEAN;
    }
    counter++;
    if (counter == STRENGTH_OF_STEPING_MEAN)
        counter = 0;
}

double StepingMeanFilter::operator[](unsigned int i){
    return toReturn[i];
}

double* StepingMeanFilter::getVariance(){
    for(unsigned int i = 0; i < 3; i++){
        double mean = sum[i] / STRENGTH_OF_STEPING_MEAN;
        double result = 0;
        for(unsigned int j = 0; j < STRENGTH_OF_STEPING_MEAN; j++){
            double tmp = mean - memory[i][j];
            result += tmp * tmp;
        }
        variance[i] = result / (STRENGTH_OF_STEPING_MEAN - 1);
    }
    return variance;
}

double* StepingMeanFilter::getFiltredValues(){
    return toReturn;
}
