#ifndef __SCRC_DATAPIPELINE_HXX__
#define __SCRC_DATAPIPELINE_HXX__

#include <string>
#include <algorithm>
#include <iostream>
#include "json/json.h"
#include "spdlog/spdlog.h"
#include "scrc/registry/api.hxx"
#include "scrc/registry/file_system.hxx"
#include "scrc/registry/data_object.hxx"
#include "scrc/registry/file.hxx"
#include "scrc/utilities/logging.hxx"

namespace SCRC
{
    class DataPipelineImpl_;

    class DataPipeline
    {
        public:
            explicit DataPipeline();
            ~DataPipeline();

            DataPipeline(DataPipeline&& rhs) noexcept;
            DataPipeline& operator=(DataPipeline&& rhs) noexcept;
            
            DataPipeline(const DataPipeline& rhs);
            DataPipeline& operator=(const DataPipeline& rhs);

            double read_estimate(std::string data_product, const std::string &component);
            void add_to_register(std::string label);

        private:
            std::unique_ptr<DataPipelineImpl_> pimpl_;
    };

    class DataPipelineImpl_
    {
        public:
            const LocalFileSystem* file_system;
            API* api = nullptr;

            DataPipelineImpl_(std::filesystem::path config_file_path, std::filesystem::path apiroot=LOCAL_API_ROOT, spdlog::level::level_enum log_level=spdlog::level::info) : 
                file_system(new LocalFileSystem(config_file_path)),
                api(new API(apiroot)) 
            {
                spdlog::set_default_logger(APILogger);
                APILogger->set_level(log_level);
            }

            Json::Value fetch_all_objects();
            Json::Value fetch_all_namespaces();
            Json::Value fetch_object_by_id(int identifier);
            Json::Value fetch_all_data_products();
            Json::Value fetch_all_external_objects();
            Json::Value fetch_data_product_by_id(int identifier);
            Json::Value fetch_store_root_by_id(int identifier);
            Json::Value fetch_data_store_by_id(int identifier);
            Json::Value fetch_external_object_by_id(int identifier);
            std::filesystem::path download_data_product(ReadObject::DataProduct* data_product);
            int get_id_from_namespace(std::string name_space);
            int get_id_from_path(std::filesystem::path path);

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

    class StorageRootQuery : public Query
    {
        public:
            StorageRootQuery() : Query(
                "storage_root",
                {
                    "name",
                    "root",
                    "path",
                    "accessibility"
                }
            ) {}

            StorageRootQuery(std::filesystem::path query_path) : Query(
                "storage_root",
                query_path
            ) {}
    };

    class ExternalObjectQuery : public Query
    {
        public:
            ExternalObjectQuery() : Query(
                "external_object",
                {
                    "doi_or_unique_name",
                    "primary_not_supplement",
                    "release_date",
                    "title",
                    "description",
                    "version",
                    "object",
                    "source",
                    "original_store",
                }
            ) {}

            ExternalObjectQuery(std::filesystem::path query_path) : Query(
                "external_object",
                query_path
            ) {}
    };

    class NamespaceQuery : public Query
    {
        public:
            NamespaceQuery() : Query(
            "namespace",
            std::vector<std::string>{
                "name"
            }) {}

            NamespaceQuery(std::filesystem::path query_path) : Query(
                "namespace",
                query_path
            ) {}
    };
};

#endif