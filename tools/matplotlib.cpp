#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <iostream>
#include "matplotlibcpp.h"
#include "plot.hh"

/*
    If occur error of figure_size or variabe desktop, then add "backend: Agg" to file ~/.config/matplotlib/matplotlibrc
*/

void plot::figure_size(unsigned int x, unsigned int y){
    matplotlibcpp::figure_size(x, y);
}
void plot::figure_close(){
    matplotlibcpp::close();
}
void plot::named_plot(const std::string& name, std::vector<double> time, std::vector<double> data){
    matplotlibcpp::named_plot(name, time, data);
}
void plot::title(const std::string& tit){
    matplotlibcpp::title(tit);
}
void plot::legend(){
    matplotlibcpp::legend();
}
void plot::save(const std::string& name){
    matplotlibcpp::save(name);
}
