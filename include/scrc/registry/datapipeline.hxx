#ifndef __DATAPIPELINE_HXX__
#define __DATAPIPELINE_HXX__

#include <string>
#include "api.hxx"
#include "json/json.h"

namespace SCRC
{
    class DataPipeline
    {
        public:
            DataPipeline() {}
            double read_estimate(std::string data_product, const std::string &component);
            Json::Value fetch_all_objects();
            Json::Value fetch_object_by_id(int identifier);
        private:
            API* api_ = new API;
    };
};

#endif