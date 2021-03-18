#ifndef __SCRC_DATAPIPELINE_HXX__
#define __SCRC_DATAPIPELINE_HXX__

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
            Json::Value fetch_all_data_products();
            Json::Value fetch_data_product_by_id(int identifier);
            void download_data(int identifier);
        private:
            std::filesystem::path get_data_download_path_(int identifier);
            API* api_ = new API;
    };
};

#endif