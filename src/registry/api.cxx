#include "fdp/registry/api.hxx"

#include <regex>

namespace FairDataPipeline {
static size_t write_str_(char *ptr, size_t size, size_t nmemb, void* userdata ) {
    std::string* data = static_cast< std::string* >( userdata );
    data->append((char *)ptr, size * nmemb);
    return size * nmemb;
}

static size_t write_file_(char*ptr, size_t size, size_t nmemb, void* userdata ) {
    FILE* stream = static_cast< FILE* >( userdata );
    size_t written_n_ = fwrite(ptr, size, nmemb, stream);
    return written_n_;
}

API::sptr API::construct( const std::string& url_root )
{
    return API::sptr( new API( url_root ) );
}

std::string url_encode( const std::string& url) {
  CURL *curl_ = curl_easy_init();
  curl_easy_setopt(curl_, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
  return curl_easy_escape(curl_, url.c_str(), 0);
}

CURL *API::setup_json_session_(std::string &addr_path, std::string *response,
                               long &http_code, std::string token) {
  CURL *curl_ = curl_easy_init();
  curl_easy_setopt(curl_, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);


  if (!token.empty()) {
    logger::get_logger()->debug() 
        << "Adding token: " 
        << token
        << " to headers";
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, (std::string("Authorization: token ") + token).c_str());
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
  }

  logger::get_logger()->debug() 
      << "API:JSONSession: Attempting to access: " << addr_path;
  curl_easy_setopt(curl_, CURLOPT_URL, addr_path.c_str());
  curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_str_);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, response);
  CURLcode res = curl_easy_perform(curl_);
  if (res == CURLE_OK) {
    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);
  } else {
    http_code = 0;
  }
  curl_easy_cleanup(curl_);
  curl_global_cleanup();

  return curl_;
}

void API::download_file(const ghc::filesystem::path &url,
                        ghc::filesystem::path out_path) {
  CURL *curl_ = curl_easy_init();
  FILE *file_ = fopen(out_path.string().c_str(), "wb");

  logger::get_logger()->debug() 
      << "API: Downloading file '"
      << url.string()
      << "' -> '" << out_path.string() << "'", url.string();
  curl_easy_setopt(curl_, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);                 
  curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_file_);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, file_);
  curl_easy_perform(curl_);
  fclose(file_);
}

CURL *API::setup_download_session_(const ghc::filesystem::path &addr_path,
                                   FILE *file) {
  CURL *curl_ = curl_easy_init();

  logger::get_logger()->debug() 
      << "API:DownloadSession: Attempting to access: " << addr_path.string();

  curl_easy_setopt(curl_, CURLOPT_URL, addr_path.string().c_str());
  curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_file_);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, file);
  curl_easy_perform(curl_);
  curl_easy_cleanup(curl_);
  curl_global_cleanup();
  return curl_;
}

Json::Value API::get_request(const ghc::filesystem::path &addr_path,
                         long expected_response, std::string token) {
  std::string addr_path_ = std::regex_replace(addr_path.string(), std::regex(std::string("\\\\")), "/");
  return get_request(addr_path_, expected_response);
}

Json::Value API::get_request(const std::string &addr_path, long expected_response, std::string token) {
  Json::Value root_;
  Json::CharReaderBuilder json_charbuilder_;

  long http_code;

  std::string search_str_ = url_root_ + addr_path;

  std::string response_str_;

  auto *session_ = setup_json_session_(search_str_, &response_str_, http_code, token);

  const std::unique_ptr<Json::CharReader> json_reader_(
      json_charbuilder_.newCharReader());
  const auto response_str_len_ = response_str_.length();
  JSONCPP_STRING err;

  if (http_code == 0) {
    logger::get_logger()->error() 
        << "API:Request: Request to '"
        << search_str_
        << "' returned no response";
    throw rest_apiquery_error("No response was given");
  }

  else if (http_code != expected_response) {
    throw rest_apiquery_error("Request '" + search_str_ +
                              "' returned exit code " +
                              std::to_string(http_code) + " but expected " +
                              std::to_string(expected_response));
  }

  if (!json_reader_->parse(response_str_.c_str(),
                           response_str_.c_str() + response_str_len_, &root_,
                           &err)) {
    logger::get_logger()->error() 
        << "API:Query: Response string '"
        << response_str_
        << "' is not JSON parsable. Return Code was "
        << http_code;
    throw rest_apiquery_error(
        "Failed to retrieve information from JSON response string");
  }

  return (root_.isMember("results")) ? root_["results"] : root_;
}

Json::Value API::get_by_json_query(const std::string &addr_path,
                                Json::Value &query_data,
                                long expected_response, std::string token) {

  // Check for API root in urls
  std::string q_ = append_with_forward_slash(addr_path) + json_to_query_string(query_data);
  return get_request(q_, expected_response, token);
}

Json::Value API::get_by_id(const std::string &table, int const &id,
                         long expected_response, std::string token) {
  std::string query = table + "/" + std::to_string(id) + "/";
  ghc::filesystem::path queryPath = query;
  return get_request(queryPath, expected_response, token);
}

std::string API::json_to_query_string(Json::Value &json_value) {
  // Start the string with a ?
  std::string rtn = "?";
  // Need to remove the api address from any values using regex
  std::string regex_string = "(" + url_root_ + ")([A-Za-z_]+)\\/([0-9]+)\\/";
  // Check the json value is not empty
  if (json_value.size() > 0) {
    // Iterate through the json keys
    for (auto key : json_value.getMemberNames()) {
      // Get the current value
      if (json_value.get(key, "").isArray()) {
        // Check to see if they current value is an array (e.g. inputs or
        // outputs) and if so iterate through the array
        for (Json::Value::ArrayIndex i = 0; i != json_value.get(key, "").size();
             i++) {
          // add the key and value to the return string after removing the api
          // address with regex
          rtn += key + "=" +
                 std::regex_replace(json_value.get(key, "")[i].asString(),
                                    std::regex(regex_string), "$3") +
                 "&";
        }
      } else {
        // if it's not an array add the key and value to the return string after
        // removing the api address with regex
        rtn += key + "=" +
               std::regex_replace(json_value.get(key, "").asString(),
                                  std::regex(regex_string), "$3") +
               "&";
      }
    }
  }
  // Return the string with spaces converted to html characters
  return escape_space(rtn);
}

std::string API::escape_space(std::string &str) {
  // Using regex replace space with html character (%20)
  return std::string(std::regex_replace(str, std::regex(" "), "%20"));
}

Json::Value API::post(std::string addr_path, Json::Value &post_data,
                      const std::string &token, long expected_response) {
  return API::post_patch_request(addr_path, post_data, token, expected_response, false);
}

Json::Value API::post_storage_root(Json::Value &post_data, const std::string &token){
  if (post_data["local"]){
    if(post_data["local"].as<bool>()){
        post_data["root"] = "file://" + post_data["root"].as<std::string>();
    }
  }
  return API::post_patch_request("storage_root", post_data, token, 201, false);
}

Json::Value API::patch(std::string addr_path, Json::Value &post_data,
                       const std::string &token, long expected_response) {
  return API::post_patch_request(addr_path, post_data, token, expected_response, true);
}

Json::Value API::post_patch_request(std::string addr_path, Json::Value &post_data,
                         const std::string &token, long expected_response,
                         bool PATCH) {
  const std::string url_path_ =
      url_root_ + API::append_with_forward_slash(addr_path);
  const std::string data_ = json_to_string(post_data);
  logger::get_logger()->debug() << "API:Post: Post Data\n" << data_;
  long return_code_;
  std::string response_;
  CURL *curl_ = curl_easy_init();

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");

  if (!token.empty()) {
    logger::get_logger()->debug()
        << "Adding token: "
        << token
        << " to headers";
    headers = curl_slist_append(
        headers, (std::string("Authorization: token ") + token).c_str());
  }
  curl_easy_setopt(curl_, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
  curl_easy_setopt(curl_, CURLOPT_URL, url_path_.c_str());
  curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
  if (PATCH) {
    curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, "PATCH");
  }

  curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data_.c_str());
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_str_);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response_);

  CURLcode res = curl_easy_perform(curl_);
  long http_code;

  if (res == CURLE_OK) {
    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);
  } else {
    curl_easy_cleanup(curl_);
    curl_global_cleanup();
    logger::get_logger()->error() 
        << "API:Post: Post to '"
        <<url_path_
        << "' returned no response";
    throw rest_apiquery_error("No response was given");
  }

  curl_easy_cleanup(curl_);
  curl_global_cleanup();

  if (http_code == 404) {
    throw rest_apiquery_error("'" + addr_path + "' does not exist");
  }

  else if (http_code == 409) {
    logger::get_logger()->info() 
        << "API:Post Entry Exists attempting to return entry";
    return get_request(API::append_with_forward_slash(addr_path) +
                   json_to_query_string(post_data))[0];
  }

  else if (http_code != expected_response) {
    throw rest_apiquery_error(
        "API:Post: '" + url_path_ + "' returned exit code " +
        std::to_string(http_code) + " but expected " +
        std::to_string(expected_response) + " Responce: " + response_);
  }

  Json::Value root_;
  Json::CharReaderBuilder json_charbuilder_;

  const auto response_str_len_ = response_.length();
  const std::unique_ptr<Json::CharReader> json_reader_(
      json_charbuilder_.newCharReader());
  JSONCPP_STRING err;

  if (!json_reader_->parse(response_.c_str(),
                           response_.c_str() + response_str_len_, &root_,
                           &err)) {
    logger::get_logger()->error() 
        << "API:Post: Response string '"
        << response_
        << "' is not JSON parsable. Return Code was "
        << return_code_;
    throw rest_apiquery_error(
        "Failed to retrieve information from JSON response string");
  }

  Json::Value results_ = (root_.isMember("results")) ? root_["results"] : root_;

  return results_;
}

std::string API::append_with_forward_slash(std::string str) {
  if (str.empty()) {
    return str;
  }
  switch (str.back()) {
  case '/':
    return str;
  case '\\':
    return str.substr(0, str.size() - 1) + "/";
  }
  return str + "/";
}

std::string API::remove_leading_forward_slash(std::string str) {
  if (str.empty()) {
    return str;
  }
  switch (str.front()) {
  case '/':
    return str.substr(1, str.size());
  case '\\':
    return str.substr(1, str.size());
  }
  return str;
}

}; // namespace FairDataPipeline
