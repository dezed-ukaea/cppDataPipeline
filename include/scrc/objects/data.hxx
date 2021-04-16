#ifndef __SCRC_DATATABLE_HXX__
#define __SCRC_DATATABLE_HXX__

#include <string>
#include <vector>
#include <stdexcept>

namespace SCRC
{
    template<typename T>
    struct DataTableColumn
    {   
        const std::string name;
        const std::string unit;
        const std::vector<std::string>* row_names;
        const std::vector<T>* values;

        T operator[] (const char* key)
        {
            const auto iterator_ = std::find(row_names->begin(), row_names->end(), std::string(key));
            if(iterator_ == row_names->end())
            {
                throw std::invalid_argument("No row name '"+std::string(key)+"' in column '"+name+"'");
            }

            const int distance_ = std::distance(row_names->begin(), iterator_);

            return values->operator[](distance_);

        }
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

        int rank() const { return dimensions.size(); }
    };
};

#endif