#ifndef __SCRC_API_HXX__
#define __SCRC_API_HXX__

#include <algorithm>
#include <curl/curl.h>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <json/reader.h>
#include <map>
#include <string>
#include <vector>

#include "scrc/exceptions.hxx"
#include "scrc/utilities/json.hxx"
#include "scrc/utilities/logging.hxx"

namespace SCRC {
const std::string REMOTE_API_ROOT = "https://data.scrc.uk/api";
const std::string LOCAL_API_ROOT = "http://localhost:8000/api";

size_t write_func_(void *ptr, size_t size, size_t nmemb, std::string *data);

class Query {
private:
  const std::vector<std::string> valid_filters_;
  std::string string_repr_;
  std::map<std::string, std::string> components_;
  const std::filesystem::path fragments_;

public:
  Query(std::string string_repr, std::vector<std::string> filters)
      : string_repr_(string_repr), valid_filters_(filters) {}
  Query(std::string string_repr, std::filesystem::path query_path)
      : string_repr_(string_repr), fragments_(query_path) {}
  void append(std::string key, std::string value);
  std::filesystem::path build_query();
};

class API {
public:
  API(std::string url_root = LOCAL_API_ROOT) : url_root_(url_root) {}
  Json::Value request(const std::filesystem::path &addr_path,
                      long expected_response = 200);
  Json::Value query(Query query, long expected_response = 200);
  std::filesystem::path post(const std::filesystem::path &addr_path,
                             Json::Value &post_data,
                             long expected_response = 201);
  std::string get_url_root() const { return url_root_; }
  void download_file(const std::filesystem::path &url,
                     std::filesystem::path out_path);

private:
  std::string url_root_;
  CURL *setup_json_session_(const std::filesystem::path &addr_path,
                            std::string *response);
  CURL *setup_download_session_(const std::filesystem::path &addr_path,
                                FILE *file);
};

std::string url_encode(std::string url);

}; // namespace SCRC

#endif