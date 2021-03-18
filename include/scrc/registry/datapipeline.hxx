#ifndef __SCRC_DATAPIPELINE_HXX__
#define __SCRC_DATAPIPELINE_HXX__

#include <string>
#include "api.hxx"
#include "json/json.h"
#include "scrc/registry/file_system.hxx"
#include "scrc/registry/options.hxx"

namespace SCRC
{
    class DataPipeline
    {
        public:
            DataPipeline(std::filesystem::path config_file_path) : 
                file_system_(new LocalFileSystem(config_file_path)) {}
            double read_estimate(std::string data_product, const std::string &component);
            Json::Value fetch_all_objects();
            Json::Value fetch_object_by_id(int identifier);
            Json::Value fetch_all_data_products();
            Json::Value fetch_data_product_by_id(int identifier);
            Json::Value search_data_products(std::string search_str, DataProduct::Filters filter=DataProduct::Filters::NAME);
            Json::Value search_objects(std::string search_str, Object::Filters filter=Object::Filters::KEYWORDS);
        private:
            const LocalFileSystem* file_system_;
            std::filesystem::path get_data_download_path_(int identifier);
            std::filesystem::path get_data_download_path_(std::filesystem::path local_data_path);
            API* api_ = new API;
    };
};

#endif