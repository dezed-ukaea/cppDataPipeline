
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
        size_t written_n_ = fwrite(ptr, size, nmemb, stream);
        return written_n_;
    }

    void Query::append(std::string key, std::string value)
    {
        if(std::find(valid_filters_.begin(), valid_filters_.end(), key) == valid_filters_.end())
        {
            throw std::invalid_argument(
                "Search term '"+key+"' is not a valid filter for type '"+string_repr_+"'"
            );
        }

        if(value.empty())
        {
            throw std::invalid_argument(
                "Assigning an empty string to query tag '"+key+"' is not allowed"
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
            as_strs_.push_back(component.first+"="+url_encode(component.second));
        }
    
        std::string query_str_;

        // Check if the number of query components is greater than 1, if it is
        // combine them with '&'
        if(as_strs_.size() > 1)
        {
            std::copy(as_strs_.begin(), as_strs_.end(), std::ostream_iterator<std::string>(out_url_query_, "&"));
            
            // Add '?' to state it is a query and remove last added '&' from end of URL
            query_str_ = "?"+out_url_query_.str().substr(0, out_url_query_.str().size()-1);
        }
        else
        {
            // Add '?' to state it is a query
            query_str_ = "?"+as_strs_[0];
        }

        const std::filesystem::path query_path_(query_str_);

        const std::string query_out_ = std::string(std::filesystem::path(string_repr_) / query_path_);

        APILogger->debug("API:Query: Built query string: {0}", query_out_);

        return query_out_;
    }

    std::string url_encode(std::string url)
    {
        CURL* curl_ = curl_easy_init();
        return curl_easy_escape(curl_, url.c_str(), 0);
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

    void API::download_file(std::filesystem::path url, std::filesystem::path out_path)
    {
        CURL* curl_ = curl_easy_init();
        FILE* file_ = fopen(out_path.c_str(), "wb");
        APILogger->debug("API: Downloading file '{0}' -> '{1}'", url.string(), out_path.string());
        curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_file_);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, file_);
        curl_easy_perform(curl_);
        fclose(file_);      
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
            throw rest_apiquery_error(
                "Request '"+search_str_.string()+
                "' returned exit code "+std::to_string(http_code)+
                " but expected "+std::to_string(expected_response)
            );
        }
        
        if (!json_reader_->parse(
                response_str_.c_str(),
                response_str_.c_str() + response_str_len_, &root_,
                &err)) {
            throw rest_apiquery_error("Failed to retrieve information from JSON response string");
        }

        return (root_["results"]) ? root_["results"] : root_;
    }

    Json::Value API::query(Query query, long expected_response)
    {
        return request(query.build_query(), expected_response);
    }
};