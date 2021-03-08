
#include "scrc/registry/api.hxx"

namespace SCRC
{
    size_t write_func_(void* ptr, size_t size, size_t nmemb, std::string* data)
    {
        data->append((char*)ptr, size * nmemb);
        return size * nmemb;
    }

    CURL* API::setup_session_(std::filesystem::path addr_path, std::string* response)
    {
        CURL* curl_ = curl_easy_init();
        curl_easy_setopt(curl_, CURLOPT_URL, addr_path.string().c_str());
        curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 1);
        curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_func_);
        curl_easy_setopt(curl_, CURLOPT_WRITEDATA, response);
        curl_easy_perform(curl_);
        curl_easy_cleanup(curl_);
        curl_global_cleanup();

        return curl_;
    }

    Json::Value API::request(std::filesystem::path addr_path)
    {
        Json::Value root_;
        Json::CharReaderBuilder json_charbuilder_;

        std::string response_str_;
        const std::filesystem::path search_str_ = url_root_ / addr_path;

        auto* session_ = setup_session_(search_str_, &response_str_);
        
        const std::unique_ptr<Json::CharReader> json_reader_(json_charbuilder_.newCharReader());
        const auto response_str_len_ = static_cast<int>(response_str_.length());
        JSONCPP_STRING err;
        
        if (!json_reader_->parse(
                response_str_.c_str(),
                response_str_.c_str() + response_str_len_, &root_,
                &err)) {
            throw std::runtime_error("Failed to retrieve information from JSON response string");
        }

        return root_["results"];
    }
};