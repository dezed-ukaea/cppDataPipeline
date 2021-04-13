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
        const std::string dimension_1_title;
        const std::string dimension_2_title;
        const std::vector<std::string> dimension_1_names;
        const std::vector<std::string> dimension_2_names;
        const std::vector<std::vector<T>> array;
    };
};

#endif