#ifndef __SCRC_DATATABLE_HXX__
#define __SCRC_DATATABLE_HXX__

#include <string>
#include <vector>

namespace SCRC
{
    struct DataTable
    {

    };

    template<typename T>
    struct ArrayObject
    {
        const std::vector<std::string> dimension_titles;
        const std::vector<std::vector<std::string>> dimension_names;
        const std::vector<int> dimensions;
        const std::vector<T> array;

        T get(std::vector<int> co_ords) const
        {
            int flat_index_ = 0;

            if(dimensions.size() != co_ords.size())
            {
                throw std::invalid_argument(
                    "Number of coordinates must match number of dimensions"
                );
            }

            for(int i{0}; i < dimensions.size(); ++i) 
            {
                if(co_ords[i] > dimensions[i]-1)
                {
                    throw std::invalid_argument(
                        "Coordinate["+std::to_string(i)+"] exceeds dimension index limit: "+std::to_string(co_ords[i])+" > "+std::to_string(dimensions[i]-1)
                    );
                }

                flat_index_ += dimensions[i]*co_ords[i];
            }

            return array[flat_index_];
        }
    };
};

#endif