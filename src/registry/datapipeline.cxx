#include "scrc/registry/datapipeline.hxx"

namespace SCRC
{
    Json::Value DataPipeline::fetch_all_objects()
    {
        APILogger->debug("DataPipeline: Fetching all objects from registry");
        return api_->query(ObjectQuery(""));
    }

    Json::Value DataPipeline::fetch_all_data_products()
    {
        return api_->query(DataProductQuery(""));
    }

    Json::Value DataPipeline::fetch_object_by_id(int identifier)
    {
        return api_->query(ObjectQuery(std::to_string(identifier)));
    }

    Json::Value DataPipeline::fetch_data_product_by_id(int identifier)
    {
        return api_->query(DataProductQuery(std::to_string(identifier)));
    }

    int get_id_from_path(std::filesystem::path path)
    {
        return std::stoi(path.stem());
    }

    Json::Value DataPipeline::fetch_data_store_by_id(int identifier)
    {
        return api_->query(StorageLocationQuery(std::to_string(identifier)));
    }
};