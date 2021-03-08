#ifndef __SCRCAPI_HXX__
#define __SCRCAPI_HXX__

#include <json/reader.h>
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <filesystem>

namespace SCRC
{
    const std::string API_ROOT = "https://data.scrc.uk/api";

    size_t write_func_(void* ptr, size_t size, size_t nmemb, std::string* data);

    class API
    {
        public:
            API(std::string url_root=API_ROOT) : url_root_(url_root) {}
            Json::Value request(std::filesystem::path addr_path);
        private:
            std::string url_root_;
            CURL* setup_session_(std::filesystem::path addr_path, std::string* response);
    };
};

#endif