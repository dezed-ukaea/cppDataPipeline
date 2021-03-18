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

    std::filesystem::path DataPipeline::get_data_download_path_(int identifier)
    {
        Json::Value product_metadata_ = fetch_data_product_by_id(identifier);
        std::filesystem::path download_path_ = API_ROOT;
        download_path_ /= "data_product";
        download_path_ /= product_metadata_["namespace"].asString();
        download_path_ += ":";
        download_path_ += product_metadata_["name"].asString();
        download_path_ += "@";
        download_path_ += product_metadata_["version"].asString();

        return download_path_;
    }

    void DataPipeline::download_data(int identifier)
    {
        std::filesystem::path download_path_ = get_data_download_path_(identifier);
        api_->download(download_path_, "");
    }
};