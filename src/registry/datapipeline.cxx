#include "scrc/registry/datapipeline.hxx"

namespace SCRC
{
    Json::Value DataPipeline::fetch_all_objects()
    {
        return api_->request("object/");
    }

    Json::Value DataPipeline::fetch_all_data_products()
    {
        return api_->request("data_product/");
    }

    Json::Value DataPipeline::fetch_object_by_id(int identifier)
    {
        return api_->request("object/"+std::to_string(identifier)+"/");
    }

    Json::Value DataPipeline::fetch_data_product_by_id(int identifier)
    {
        return api_->request("data_product/"+std::to_string(identifier)+"/");
    }
};