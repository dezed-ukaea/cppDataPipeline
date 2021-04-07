
#include "scrc/registry/api.hxx"

namespace SCRC
{
    size_t write_str_(void* ptr, size_t size, size_t nmemb, std::string* data)
    {
        data->append((char*)ptr, size * nmemb);
        return size * nmemb;
    }

    
    size_t write_file_(void *ptr, size_t size, size_t nmemb, FILE *stream)
    {
        size_t written = fwrite(ptr, size, nmemb, stream);
        return written;
    }

    void Query::append(std::string key, std::string value)
    {
        if(std::find(valid_filters_.begin(), valid_filters_.end(), key) == valid_filters_.end())
        {
            throw std::invalid_argument(
                "Search term '"+key+"' is not a valid filter for type '"+string_repr_+"'"
            );
        }
        components_[key] = value;
    }

    std::filesystem::path Query::build_query()
    {
        APILogger->debug("API:Query: Building query string...");
        if(components_.empty())
        {
            std::string query_path_ = std::filesystem::path(string_repr_) / fragments_;
            if(!fragments_.empty()) query_path_ += "/";
            return query_path_;
        }

        std::stringstream out_url_query_;
        std::vector<std::string> as_strs_;
        
        for(auto& component: components_)
        {
            as_strs_.push_back(component.first+"="+component.second);
        }
    
        std::copy(as_strs_.begin(), as_strs_.end(), std::ostream_iterator<std::string>(out_url_query_, "&"));

        const std::filesystem::path query_path_(out_url_query_.str());

        const std::string query_out_ = std::string(std::filesystem::path(string_repr_) / query_path_) + "/";

        APILogger->debug("API:Query: Built query string: {0}", query_out_);

        return query_out_;
    }

    CURL* API::setup_json_session_(std::filesystem::path addr_path, std::string* response)
    {
        CURL* curl_ = curl_easy_init();
        APILogger->debug("API:JSONSession: Attempting to access: "+addr_path.string());
        curl_easy_setopt(curl_, CURLOPT_URL, addr_path.string().c_str());
        curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 1);
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_str_);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, response);
        curl_easy_perform(curl_);
        curl_easy_cleanup(curl_);
        curl_global_cleanup();

        return curl_;
    }

    CURL* API::setup_download_session_(std::filesystem::path addr_path, FILE* file)
    {
        CURL* curl_ = curl_easy_init();
        APILogger->debug("API:DownloadSession: Attempting to access: "+addr_path.string());
        curl_easy_setopt(curl_, CURLOPT_URL, addr_path.string().c_str());
        curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 1);
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_file_);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, file);
        curl_easy_perform(curl_);
        curl_easy_cleanup(curl_);
        curl_global_cleanup();
        return curl_;
    }

    Json::Value API::request(std::filesystem::path addr_path, long expected_response)
    {
        APILogger->debug("API: Sending request to '{0}'", addr_path.string());
        Json::Value root_;
        Json::CharReaderBuilder json_charbuilder_;

        long http_code;

        std::string response_str_;
        const std::filesystem::path search_str_ = url_root_ / addr_path;

        auto* session_ = setup_json_session_(search_str_, &response_str_);
        curl_easy_getinfo (session_, CURLINFO_RESPONSE_CODE, &http_code);
        
        const std::unique_ptr<Json::CharReader> json_reader_(json_charbuilder_.newCharReader());
        const auto response_str_len_ = static_cast<int>(response_str_.length());
        JSONCPP_STRING err;

        if(http_code != expected_response)
        {
            throw std::runtime_error(
                "Request '"+search_str_.string()+
                "' returned exit code "+std::to_string(http_code)+
                " but expected "+std::to_string(expected_response)
            );
        }
        
        if (!json_reader_->parse(
                response_str_.c_str(),
                response_str_.c_str() + response_str_len_, &root_,
                &err)) {
            throw std::runtime_error("Failed to retrieve information from JSON response string");
        }

        return root_["results"];
    }

    Json::Value API::query(Query query, long expected_response)
    {
        return request(query.build_query(), expected_response);
    }

    void API::download(std::filesystem::path remote_addr, std::filesystem::path output_loc)
    {
        Json::CharReaderBuilder json_charbuilder_;

        long http_code;

        std::string response_str_;
        const std::filesystem::path search_str_ = url_root_ / remote_addr;

        FILE* file_ = fopen(output_loc.c_str(), "wb");

        auto* session_ = setup_download_session_(search_str_, file_);

        fclose(file_);        

    }
};