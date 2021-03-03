#pragma once

#define STRENGTH_OF_STEPING_MEAN 20
#include <cstddef>
#include <cstdint>
#include <stdint.h>
#include <cstdlib>
#include "math.h"

class StepingMeanFilter
{
public:
    StepingMeanFilter();
    void run(const double newData[3]);
    double* getVariance();
    double operator[](unsigned int i);
    double* getFiltredValues();
protected:
    unsigned counter = {0};
    double sum[3] = {0, 0, 0};
    double toReturn[3] = {0, 0, 0};
    double variance[3] = {0, 0, 0};
    double memory[3][STRENGTH_OF_STEPING_MEAN] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
};
