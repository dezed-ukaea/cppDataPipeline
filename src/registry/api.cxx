
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

std::filesystem::path Query::build_query() {
  APILogger->debug("API:Query: Building query string.");
  if (components_.empty()) {
    std::string query_path_ = string_repr_ + "/" + fragments_.string();
    if (!fragments_.empty())
      query_path_ += "/";
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

  const std::filesystem::path query_path_(query_str_);

  const std::string query_out_ =
      std::string((std::filesystem::path(string_repr_) / query_path_).string());

  APILogger->debug("API:Query: Built query string: {0}", query_out_);

  return query_out_;
}

std::string url_encode(std::string url) {
  CURL *curl_ = curl_easy_init();
  return curl_easy_escape(curl_, url.c_str(), 0);
}

CURL *API::setup_json_session_(const std::filesystem::path &addr_path,
                               std::string *response) {
  CURL *curl_ = curl_easy_init();
  APILogger->debug("API:JSONSession: Attempting to access: " +
                   addr_path.string());
  curl_easy_setopt(curl_, CURLOPT_URL, addr_path.string().c_str());
  curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 1);
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_str_);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, response);
  curl_easy_perform(curl_);
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

Json::Value API::request(const std::filesystem::path &addr_path,
                         long expected_response) {
  Json::Value root_;
  Json::CharReaderBuilder json_charbuilder_;

  long http_code;

  std::string response_str_;
  const std::filesystem::path search_str_ = url_root_ / addr_path;

  auto *session_ = setup_json_session_(search_str_, &response_str_);
  curl_easy_getinfo(session_, CURLINFO_RESPONSE_CODE, &http_code);

  const std::unique_ptr<Json::CharReader> json_reader_(
      json_charbuilder_.newCharReader());
  const auto response_str_len_ = static_cast<int>(response_str_.length());
  JSONCPP_STRING err;

  if (http_code == 0) {
    APILogger->error("API:Request: Request to '{0}' returned no response",
                     search_str_.string());
    throw rest_apiquery_error("No response was given");
  }

  else if (http_code != expected_response) {
    throw rest_apiquery_error("Request '" + search_str_.string() +
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
  return request(query.build_query(), expected_response);
}

Json::Value API::post(const std::filesystem::path &addr_path,
                      Json::Value &post_data, const std::string &key,
                      long expected_response) {
  const std::string url_path_ = (url_root_ / addr_path).string() + "/";
  const std::string data_ = json_to_string(post_data);
  APILogger->debug("API:Post: Post Data\n{0}", data_);
  long return_code_;
  std::string response_;
  CURL *curl_ = curl_easy_init();

  struct curl_slist *headers = NULL;
  curl_slist_append(headers, "Content-Type: application/json");

  if (!key.empty()) {
    curl_slist_append(headers,
                      (std::string("Authorization: token ") + key).c_str());
  }

  curl_easy_setopt(curl_, CURLOPT_URL, url_path_.c_str());
  curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
  curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &return_code_);
  curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, data_.c_str());
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_str_);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response_);

  curl_easy_perform(curl_);

  if (return_code_ == 0) {
    APILogger->error("API:Post: Post to '{0}' returned no response", url_path_);
    throw rest_apiquery_error("No response was given");
  }

  else if (return_code_ == 404) {
    throw rest_apiquery_error("'" + addr_path.string() + "' does not exist");
  }

  else if (return_code_ == 409) {
    throw rest_apiquery_error("Entry already exists");
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

  curl_easy_cleanup(curl_);
  curl_global_cleanup();

  return results_;
}
}; // namespace FDP