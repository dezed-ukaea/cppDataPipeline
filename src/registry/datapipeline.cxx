#include "scrc/registry/datapipeline.hxx"

namespace SCRC
{
    DataPipeline::~DataPipeline()
    {
        
    }

    void DataPipeline::add_to_register(std::string& item)
    {
    }

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
            const std::string hash_ = store_res_["hash"].asString();

            const std::string root_loc_str_ = store_res_["storage_root"].asString();
            const std::string root_loc_id_ = std::filesystem::path(root_loc_str_.substr(0, root_loc_str_.size()-1)).filename();
            const Json::Value root_res_ = fetch_store_root_by_id(std::stoi(root_loc_id_));
            const std::filesystem::path root_loc_ = root_res_["root"].asString();

            APILogger->debug("DataPipeline: File download location '{0}'", (root_loc_ / rel_file_loc_).string());

            const std::filesystem::path download_loc_ = file_system->get_data_store() / namespace_ / std::filesystem::path(reg_path_);

            if(std::filesystem::exists(download_loc_ / rel_file_loc_.filename()))
            {
                const std::string existing_hash_ = calculate_hash_from_file(download_loc_ / rel_file_loc_.filename());

                APILogger->debug("DataPipeline:HASH:\n\tCalculated: {0}\n\tRemote: {1}", existing_hash_, hash_);

                if(hash_ == existing_hash_)
                {
                    APILogger->debug("DataPipeline: File '{0}' is already latest version and will not be updated", (download_loc_ / rel_file_loc_.filename()).string());
                    return download_loc_ / rel_file_loc_.filename();
                }
            }

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

    void DataPipelineImpl_::push_new_coderun()
    {
        const std::string time_stamp_ = current_time_stamp();
    }

    std::string DataPipelineImpl_::new_source(const YAML::Node& data)
    {
        Json::Value post_data_;
        post_data_["name"] = data["name"].as<std::string>();
        post_data_["abbreviation"] = data["abbreviation"].as<std::string>();
        post_data_["website"] = data["website"].as<std::string>();

        return api->post("source", post_data_);
    }

    std::string DataPipelineImpl_::new_storage_root(const YAML::Node& data)
    {
        Json::Value post_data_;
        post_data_["name"] = data["name"].as<std::string>();
        post_data_["root"] = data["root"].as<std::string>();
        post_data_["accessibility"] = std::to_string(int(data["accessibility"].as<std::string>() == "closed"));

        return api->post("storage_root", post_data_);
    }

    std::string DataPipelineImpl_::new_storage_location(const YAML::Node& data)
    {
        Json::Value post_data_;
        post_data_["path"] = data["path"].as<std::string>();
        post_data_["hash"] = data["hash"].as<std::string>();
        post_data_["storage_root_id"] = data["storage_root_id"].as<std::string>();

        return api->post("storage_location", post_data_);
    }

    std::string DataPipelineImpl_::new_external_object(const YAML::Node& data)
    {
        Json::Value post_data_;
        post_data_["doi_or_unique_name"] = data["doi_or_unique_name"].as<std::string>();
        post_data_["release_date"] = data["release_date"].as<std::string>();
        post_data_["title"] = data["title"].as<std::string>();
        post_data_["description"] = data["description"].as<std::string>();
        post_data_["version"] = data["version"].as<std::string>();
        post_data_["object"] = data["object"].as<std::string>();
        post_data_["source"] = data["source"].as<std::string>();
        post_data_["original_store"] = data["original_store"].as<std::string>();

        return api->post("external_object", post_data_);
    }

    void DataPipelineImpl_::register_external_object(const YAML::Node& register_entry)
    {
        const std::string file_type_ = register_entry["file_type"].as<std::string>();
        const std::filesystem::path root_ = register_entry["root"].as<std::string>();
        const std::string product_name_ = register_entry["product_name"].as<std::string>();
        const std::filesystem::path local_dir_ = file_system->get_data_store() / file_system->get_default_output_namespace() / product_name_;
        const std::string time_stamp_ = current_time_stamp();
        const std::filesystem::path temp_file_ = calculate_hash_from_string(time_stamp_) + "." + file_type_;
        const std::string file_hash_ = calculate_hash_from_file(local_dir_ / temp_file_);
        const std::filesystem::path new_filename_ = file_hash_ + "." + file_type_;
        const std::string path_ = register_entry["path"].as<std::string>();

        if(path_.find("SELECT") != std::string::npos && path_.find("WHERE") != std::string::npos)
        {
            if(!std::filesystem::exists(local_dir_))
            {
                std::filesystem::create_directories(local_dir_);
            }

            const std::string encoded_path_ = root_ / std::filesystem::path(url_encode(path_));
            api->download_file(encoded_path_, local_dir_ / temp_file_);
        }

        const std::string source_id_ = new_source(register_entry);

        const std::string accessibility_ = register_entry["accessibility"].as<std::string>();
        bool private_access_;

        if(accessibility_ == "open")
        {
            private_access_ = false;
        }
        else if(accessibility_ == "closed")
        {
            private_access_ = true;
        }
        else
        {
            throw std::invalid_argument("Value '" + accessibility_ + "' for key 'accessibility' not recognised");
        }

        const std::string source_root_id_ = new_storage_root(register_entry);

        const std::string storage_location_id_ = new_storage_location(register_entry);

        const std::string external_object_id = new_external_object(register_entry);

        APILogger->info("Added '{0}' as '{1}' to registry", register_entry["external_object"].as<std::string>(), external_object_id);

    }

    void DataPipelineImpl_::add_to_register(const std::string& handle, const std::string& alias)
    {
        const YAML::Node config_data_ = file_system->get_config_data();

        if(!config_data_["register"])
        {
            throw config_parsing_error("Expected key 'register' not found in configuration.");   
        }

        const YAML::Node register_ =  config_data_["register"];

        std::string entry_type_;

        for(auto it = register_.begin(); it != register_.end(); ++it)
        {

            for(const std::string& s : {"external_object", "data_product", "object"})
            {
                if(it->second[s] && it->second[s].as<std::string>() == alias)
                {
                    const YAML::Node to_register_ = it->second;

                    if(entry_type_ == "external_object")
                    {
                        register_external_object(to_register_);
                    }

                    return;
                }
            }

            entry_type_ = "";
        }

        throw config_parsing_error(
                "Failed to find an entry '"+alias+"' under a recognised key from "
                "[\"external_object\", \"data_product\", \"object\"]"
        );

    }

};