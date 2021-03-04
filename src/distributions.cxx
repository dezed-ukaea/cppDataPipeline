#include "distributions.hxx"

namespace SCRC
{
    double Distribution::get_parameter(std::string param_name)
    {
        if(parameters_.find(param_name) == parameters_.end())
        {
            throw std::runtime_error("Parameter '"+param_name+"' not found.");
        }
        
        return parameters_[param_name];
    }

    std::vector<double> Distribution::get_array_parameter(std::string param_name)
    {
        if(array_parameters_.find(param_name) == array_parameters_.end())
        {
            throw std::runtime_error("Parameter '"+param_name+"' not found.");
        }
        return array_parameters_[param_name];
    }
};