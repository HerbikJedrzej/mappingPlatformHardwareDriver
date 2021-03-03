#include<string>
#include<vector>

namespace plot{
    void figure_size(unsigned int x, unsigned int y);
    void named_plot(const std::string& name, std::vector<double> time, std::vector<double> data);
    void title(const std::string& tit);
    void legend();
    void save(const std::string& name);
    void figure_close();
}