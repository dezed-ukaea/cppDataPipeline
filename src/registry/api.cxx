#include "fdp/registry/api.hxx"

namespace FDP {
size_t write_str_(void *ptr, size_t size, size_t nmemb, std::string *data) {
  data->append((char *)ptr, size * nmemb);
  return size * nmemb;
}

size_t write_file_(void *ptr, size_t size, size_t nmemb, FILE *stream) {
  size_t written_n_ = fwrite(ptr, size, nmemb, stream);
  return written_n_;
}

void Query::append(std::string key, std::string value) {
  if (std::find(valid_filters_.begin(), valid_filters_.end(), key) ==
      valid_filters_.end()) {
    throw std::invalid_argument("Search term '" + key +
                                "' is not a valid filter for type '" +
                                string_repr_ + "'");
  }

  if (value.empty()) {
    throw std::invalid_argument("Assigning an empty string to query tag '" +
                                key + "' is not allowed");
  }

  components_[key] = value;
}

std::string Query::build_query() {
  APILogger->debug("API:Query: Building query string.");
  if (components_.empty()) {
    std::string query_path_ = API::append_with_forward_slash(string_repr_) +
                              API::append_with_forward_slash(fragments_.string());
    APILogger->debug("API:Query: Built query string: {0}", query_path_);
    return query_path_;
  }

  std::stringstream out_url_query_;
  std::vector<std::string> as_strs_;

  for (auto &component : components_) {
    as_strs_.push_back(component.first + "=" + url_encode(component.second));
  }

  std::string query_str_;

  // Check if the number of query components is greater than 1, if it is
  // combine them with '&'
  if (as_strs_.size() > 1) {
    std::copy(as_strs_.begin(), as_strs_.end(),
              std::ostream_iterator<std::string>(out_url_query_, "&"));

    // Add '?' to state it is a query and remove last added '&' from end of URL
    query_str_ =
        "?" + out_url_query_.str().substr(0, out_url_query_.str().size() - 1);
  } else {
    // Add '?' to state it is a query
    query_str_ = "?" + as_strs_[0];
  }
  const std::string query_out_ = API::append_with_forward_slash(string_repr_) +
                                 API::append_with_forward_slash(query_str_);

  APILogger->debug("API:Query: Built query string: {0}", query_out_);

  return query_out_;
}

std::string url_encode(std::string url) {
  CURL *curl_ = curl_easy_init();
  return curl_easy_escape(curl_, url.c_str(), 0);
}

CURL *API::setup_json_session_(std::string &addr_path, std::string *response,
                               long &http_code, std::string token) {
  CURL *curl_ = curl_easy_init();


  if (!token.empty()) {
    APILogger->debug("Adding token: {0} to headers", token);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(
        headers, (std::string("Authorization: token ") + token).c_str());
    curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
  }

  APILogger->debug("API:JSONSession: Attempting to access: " + addr_path);
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

void API::download_file(const std::filesystem::path &url,
                        std::filesystem::path out_path) {
  CURL *curl_ = curl_easy_init();
  FILE *file_ = fopen(out_path.string().c_str(), "wb");
  APILogger->debug("API: Downloading file '{0}' -> '{1}'", url.string(),
                   out_path.string());
  curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_file_);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, file_);
  curl_easy_perform(curl_);
  fclose(file_);
}

CURL *API::setup_download_session_(const std::filesystem::path &addr_path,
                                   FILE *file) {
  CURL *curl_ = curl_easy_init();
  APILogger->debug("API:DownloadSession: Attempting to access: " +
                   addr_path.string());
  curl_easy_setopt(curl_, CURLOPT_URL, addr_path.string().c_str());
  curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_file_);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, file);
  curl_easy_perform(curl_);
  curl_easy_cleanup(curl_);
  curl_global_cleanup();
  return curl_;
}

Json::Value API::get_request(const std::filesystem::path &addr_path,
                         long expected_response, std::string token) {
  return get_request(addr_path.string(), expected_response);
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
  const auto response_str_len_ = static_cast<int>(response_str_.length());
  JSONCPP_STRING err;

  if (http_code == 0) {
    APILogger->error("API:Request: Request to '{0}' returned no response",
                     search_str_);
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
    APILogger->error("API:Query: Response string '{0}' is not JSON parsable. "
                     "Return Code was {1}",
                     response_str_, http_code);
    throw rest_apiquery_error(
        "Failed to retrieve information from JSON response string");
  }

  return (root_.isMember("results")) ? root_["results"] : root_;
}

Json::Value API::query(Query query, long expected_response) {
  return get_request(query.build_query(), expected_response);
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
  std::filesystem::path queryPath = query;
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
  APILogger->debug("API:Post: Post Data\n{0}", data_);
  long return_code_;
  std::string response_;
  CURL *curl_ = curl_easy_init();

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");

  if (!token.empty()) {
    APILogger->debug("Adding token: {0} to headers", token);
    headers = curl_slist_append(
        headers, (std::string("Authorization: token ") + token).c_str());
  }

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
    APILogger->error("API:Post: Post to '{0}' returned no response", url_path_);
    throw rest_apiquery_error("No response was given");
  }

  curl_easy_cleanup(curl_);
  curl_global_cleanup();

  if (http_code == 404) {
    throw rest_apiquery_error("'" + addr_path + "' does not exist");
  }

  else if (http_code == 409) {
    APILogger->info("API:Post Entry Exists attempting to return entry");
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

  const auto response_str_len_ = static_cast<int>(response_.length());
  const std::unique_ptr<Json::CharReader> json_reader_(
      json_charbuilder_.newCharReader());
  JSONCPP_STRING err;

  if (!json_reader_->parse(response_.c_str(),
                           response_.c_str() + response_str_len_, &root_,
                           &err)) {
    APILogger->error("API:Post: Response string '{0}' is not JSON parsable. "
                     "Return Code was {1}",
                     response_, return_code_);
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

}; // namespace FDP