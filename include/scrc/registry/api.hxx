#ifndef __SCRC_API_HXX__
#define __SCRC_API_HXX__

#include <json/reader.h>
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <filesystem>
#include <map>
#include <algorithm>
#include <iterator> 
#include <vector>

#include "scrc/utilities/logging.hxx"
#include "scrc/exceptions.hxx"

namespace SCRC
{
    const std::string REMOTE_API_ROOT = "https://data.scrc.uk/api";
    const std::string LOCAL_API_ROOT = "http://localhost:8000/api";

    size_t write_func_(void* ptr, size_t size, size_t nmemb, std::string* data);


    class Query
    {
        private:
            const std::vector<std::string> valid_filters_;
            std::string string_repr_;
            std::map<std::string, std::string> components_;
            const std::filesystem::path fragments_;
        public:
            Query(std::string string_repr, std::vector<std::string> filters) :
                string_repr_(string_repr),
                valid_filters_(filters) {}
            Query(std::string string_repr, std::filesystem::path query_path) :
                string_repr_(string_repr),
                fragments_(query_path) {}
            void append(std::string key, std::string value);
            std::filesystem::path build_query();
    };

    class API
    {
        public:
            API(std::string url_root=LOCAL_API_ROOT) : url_root_(url_root) {}
            Json::Value request(std::filesystem::path addr_path, long expected_response = 200);
            void download(std::filesystem::path remote_addr, std::filesystem::path output_loc);
            Json::Value query(Query query, long expected_response = 200);
        private:
            std::string url_root_;
            CURL* setup_json_session_(std::filesystem::path addr_path, std::string* response);
            CURL* setup_download_session_(std::filesystem::path addr_path, FILE* file);
    };
};

#endif