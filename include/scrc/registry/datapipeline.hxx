#ifndef __SCRC_DATAPIPELINE_HXX__
#define __SCRC_DATAPIPELINE_HXX__

#include <string>
#include "json/json.h"
#include "spdlog/spdlog.h"
#include "scrc/registry/api.hxx"
#include "scrc/registry/file_system.hxx"
#include "scrc/registry/file.hxx"
#include "scrc/utilities/logging.hxx"

namespace SCRC
{
    class DataPipeline
    {
        public:
            DataPipeline(std::filesystem::path config_file_path, std::filesystem::path api_root=LOCAL_API_ROOT, spdlog::level::level_enum log_level=spdlog::level::info) : 
                file_system_(new LocalFileSystem(config_file_path)),
                api_(new API(api_root)) 
            {
                spdlog::set_default_logger(APILogger);
                APILogger->set_level(log_level);
            }
            double read_estimate(std::string data_product, const std::string &component);
            Json::Value fetch_all_objects();
            Json::Value fetch_object_by_id(int identifier);
            Json::Value fetch_all_data_products();
            Json::Value fetch_data_product_by_id(int identifier);
            Json::Value fetch_data_store_by_id(int identifier);
            int get_id_from_path(std::filesystem::path path);
        private:
            const LocalFileSystem* file_system_;
            API* api_ = nullptr;
    };

    class DataProductQuery : public Query
    {
        public:
            DataProductQuery() : Query(
            "data_product",
            {
                "updated_by",
                "last_updated",
                "object",
                "namespace",
                "name",
                "version"
            }) {}
            DataProductQuery(std::filesystem::path query_path) : Query(
                "data_product",
                query_path
            ) {}
    };

    class ObjectQuery : public Query
    {
        public:
            ObjectQuery() : Query(
            "object",
            {
                "components",
                "code_repo_of",
                "config_of",
                "submission_script_of",
                "external_object",
                "quality_control",
                "keywords",
                "authors",
                "licences",
                "data_product",
                "code_repo_release",
                "metadata",
                "updated_by",
                "last_updated",
                "storage_location",
                "description",
                "file_type",
                "issues"
            }) {}

            ObjectQuery(std::filesystem::path query_path) : Query(
                "object",
                query_path
            ) {}
    };

    class StorageLocationQuery : public Query
    {
        public:
            StorageLocationQuery() : Query(
            "storage_location",
            {
                "location_for_object",
                "original_store_of",
                "updated_by",
                "last_updated",
                "path",
                "hash",
                "storage_root"
            }) {}

            StorageLocationQuery(std::filesystem::path query_path) : Query(
                "storage_location",
                query_path
            ) {}
    };
};

#endif