#ifndef __DISTRIBUTIONS_HXX__
#define __DISTRIBUTIONS_HXX__

#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <vector>

namespace SCRC
{
    typedef std::map<std::string, double> params;
    typedef std::map<std::string, std::vector<double>> arr_params;

    class Distribution
    {
        private:
            params parameters_;
            arr_params array_parameters_;
        public:
            std::string name;
            Distribution() {}
            Distribution(std::string name, params parameters, arr_params arr_parameters=arr_params()) : parameters_(parameters), name(name){}
            friend std::ostream& operator<<(std::ostream& os, const Distribution d)
            {
                os << "Distribution('" << d.name << "', ";
                for(auto p : d.parameters_)
                {
                    os << p.first << "=";
                    os << p.second;
                    os << ", ";
                }
                
                for(auto p : d.array_parameters_)
                {
                    os << p.first << "= [";
                    for(auto i : p.second)
                    {
                        os << i;
                        os << ", ";
                    }
                    os << "]";
                }

                os << ")";

                return os;
            }
            double get_parameter(std::string param_name);
            std::vector<double> get_array_parameter(std::string param_name);
    };
};

#endif