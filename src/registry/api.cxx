
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

    CURL* API::setup_json_session_(std::filesystem::path addr_path, std::string* response)
    {
        CURL* curl_ = curl_easy_init();
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