#include "scrc/registry/datapipeline.hxx"

namespace SCRC
{
    Json::Value DataPipelineImpl_::fetch_all_objects()
    {
        APILogger->debug("DataPipelineImpl_: Fetching all objects from registry");
        return api->query(ObjectQuery(""));
    }

    Json::Value DataPipelineImpl_::fetch_all_data_products()
    {
        return api->query(DataProductQuery(""));
    }

    Json::Value DataPipelineImpl_::fetch_object_by_id(int identifier)
    {
        return api->query(ObjectQuery(std::to_string(identifier)));
    }

    Json::Value DataPipelineImpl_::fetch_data_product_by_id(int identifier)
    {
        return api->query(DataProductQuery(std::to_string(identifier)));
    }

    Json::Value DataPipelineImpl_::fetch_external_object_by_id(int identifier)
    {
        return api->query(ExternalObjectQuery(std::to_string(identifier)));
    }

    Json::Value DataPipelineImpl_::fetch_all_external_objects()
    {
        return api->query(ExternalObjectQuery(""));
    }

    Json::Value DataPipelineImpl_::fetch_all_namespaces()
    {
        return api->query(NamespaceQuery(""));
    }

    int DataPipelineImpl_::get_id_from_path(std::filesystem::path path)
    {
        return std::stoi(path.stem());
    }

    int DataPipelineImpl_::get_id_from_namespace(std::string name_space)
    {
        if(name_space.empty())
        {
            throw std::invalid_argument(
                "Empty string given as name for namespace id retrieval"
            );
        }

        NamespaceQuery qnamespace_ = NamespaceQuery();
        qnamespace_.append("name", name_space);
        Json::Value namespace_json_ = api->query(qnamespace_)[0];

        std::string url_str_ = namespace_json_["url"].asString();

        // Need to remove the trailing '/'
        const std::filesystem::path url_ = url_str_.substr(0, url_str_.size()-1);

        if(url_.filename().empty())
        {
            std::cout << url_.filename() << std::endl;
            throw json_parse_error("Failed to retrieve ID for namespace '"+name_space+"'");
        }

        APILogger->debug("Retrieved ID '{0}' for namespace '{1}'", url_.filename().string(), name_space);

        return std::stoi(url_.filename());
    }

    Json::Value DataPipelineImpl_::fetch_data_store_by_id(int identifier)
    {
        return api->query(StorageLocationQuery(std::to_string(identifier)));
    }

    Json::Value DataPipelineImpl_::fetch_store_root_by_id(int identifier)
    {
        return api->query(StorageRootQuery(std::to_string(identifier)));
    }

    void DataPipeline::add_to_register(std::string label)
    {
        
    }

    std::filesystem::path DataPipelineImpl_::download_data_product(ReadObject::DataProduct* data_product)
    {
        std::string namespace_local_ = data_product->get_namespace();

        // Check if there is a namespace for the entry, else use the global default
        const std::string namespace_ = (namespace_local_.empty()) ? file_system->get_default_input_namespace() : namespace_local_;
        const int ns_id_ = get_id_from_namespace(namespace_);
        const std::string reg_path_ = data_product->get_path();

        DataProductQuery dpquery_ = DataProductQuery();
        dpquery_.append("namespace", std::to_string(ns_id_));
        dpquery_.append("name", reg_path_);

        const Json::Value res_ = api->query(dpquery_);

        if(res_.size() == 0)
        {
            throw rest_apiquery_error(
                "No data product found for '"+namespace_+":"+reg_path_+"'"
            );
        }

        const std::string object_id_path_ = res_[0]["object"].asString();
        const std::string object_id_ = std::filesystem::path(object_id_path_.substr(0, object_id_path_.size()-1)).filename();

        APILogger->debug("DataPipeline: Retrieved ID '"+object_id_+"' for object '"+namespace_+":"+reg_path_+"'");

        Json::Value objres_ = fetch_object_by_id(std::stoi(object_id_));

        if(!objres_["storage_location"])
        {
            if(objres_["external_object"])
            {
                APILogger->debug("DataPipeline: Object is external object.");
            }
            else
            {
                throw rest_apiquery_error("Object is not a recognised type");
            }

            std::string ext_obj_id_ = objres_["external_object"].asString();
            ext_obj_id_ = ext_obj_id_.substr(0, ext_obj_id_.size()-1);
            Json::Value extobj_res_ = fetch_external_object_by_id(std::stoi(ext_obj_id_));

        }

        else 
        {
            const std::string store_loc_ = objres_["storage_location"].asString();
            const std::string store_loc_id_ = std::filesystem::path(store_loc_.substr(0, store_loc_.size()-1)).filename();
            const Json::Value store_res_ = fetch_data_store_by_id(std::stoi(store_loc_id_));
            const std::filesystem::path rel_file_loc_ = store_res_["path"].asString();

            const std::string root_loc_str_ = store_res_["storage_root"].asString();
            const std::string root_loc_id_ = std::filesystem::path(root_loc_str_.substr(0, root_loc_str_.size()-1)).filename();
            const Json::Value root_res_ = fetch_store_root_by_id(std::stoi(root_loc_id_));
            const std::filesystem::path root_loc_ = root_res_["root"].asString();

            APILogger->debug("DataPipeline: File download location '{0}'", (root_loc_ / rel_file_loc_).string());

            const std::filesystem::path download_loc_ = file_system->get_data_store() / namespace_ / std::filesystem::path(reg_path_);

            // Create directories if they do not exist
            if(!std::filesystem::exists(download_loc_))
            {
                std::filesystem::create_directories(download_loc_);
            }

            api->download_file(root_loc_ / rel_file_loc_, download_loc_ / rel_file_loc_.filename());

            if(!std::filesystem::exists(download_loc_ / rel_file_loc_.filename()))
            {
                throw sync_error("Failed to download data file for '"+namespace_+":"+rel_file_loc_.string()+"'");
            }

            return download_loc_ / rel_file_loc_.filename();
        }

        throw std::runtime_error("Currently unsupported download operation");

    }

};