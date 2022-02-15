/*! **************************************************************************
 * @file fdp/registry/api.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-05
 * @brief File containing classes and methods for connecting to the RestAPI
 *
 * The classes and methods within this file are used to access the FDP
 * Data Pipeline RestAPI, they handle sending of requests and retrieval of
 * data as JSON strings
 ****************************************************************************/
#ifndef __FDP_API_HXX__
#define __FDP_API_HXX__

#include <algorithm>
#include <curl/curl.h>
#include <ghc/filesystem.hpp>
#include <iostream>
#include <iterator>
#include <json/reader.h>
#include <map>
#include <regex>
#include <string>
#include <vector>

#include "fdp/exceptions.hxx"
#include "fdp/objects/api_object.hxx"
#include "fdp/utilities/json.hxx"
#include "fdp/utilities/logging.hxx"

namespace FDP {
/*! **************************************************************************
 * @enum RESTAPI
 * @brief selection of either local or remote pipeline running
 * @author K. Zarebski (UKAEA) & R. Field
 *
 ****************************************************************************/
enum class RESTAPI {
  REMOTE, /*!< Run from remote registry */
  LOCAL   /*!< Run from local registry */
};

/*! **************************************************************************
 * @brief function for writing to file from a CURL call
 * @author K. Zarebski (UKAEA)
 *
 * @param ptr
 * @param size
 * @param nmemb
 * @param data
 * @return size_t
 ****************************************************************************/
size_t write_func_(void *ptr, size_t size, size_t nmemb, std::string *data);

/*! **************************************************************************
 * @class API
 * @brief a class which handles the fetching and posting of information to the
 * FDP data pipeline RestAPI
 * @author K. Zarebski (UKAEA) & R. Field
 *
 * The API class has know specific knowledge about the RestAPI but rather
 * provides the interface for sending/receiving data as JSON strings
 *****************************************************************************/
class API {
public:
    typedef std::shared_ptr< API > sptr;

  /*! *************************************************************************
   * @brief construct an API object using the given URL as the root
   * @author K. Zarebski (UKAEA)
   *
   * @param url_root the root of the query address, e.g. localhost:8000/api
   ***************************************************************************/
  API(std::string url_root)
      : url_root_(API::append_with_forward_slash(url_root)) {}

  /**
   * @brief sends the given 'packet' of information to the RestAPI
   * 
   * @param addr_path the api endpoint a.k.a the table name e.g. "coderun"
   * @param expected_response the expected return HTTP code
   * @param token 
   * @return Json::Value JSON object containing the information returned by the
   *request
   */
  Json::Value get_request(const std::string &addr_path,
                      long expected_response = 200, std::string token = "");

  Json::Value patch(const std::string addr_path, Json::Value &post_data,
                      const std::string &token, long expected_response = 200);
    /*! *************************************************************************
   * @brief post information from a JSON value object to the RestAPI
   * @author K. Zarebski (UKAEA) & R. Field
   *
   * @param addr_path the api endpoint a.k.a the table name e.g. "coderun"
   * @param post_data the data to transmit as a Json::Value object
   * @param key api token
   * @param expected_response the expected HTTP return code, default is 201
   * @return the result of the API post as a JSON object
   ***************************************************************************/
  Json::Value post(const std::string addr_path, Json::Value &post_data,
                   const std::string &token, long expected_response = 201);

  Json::Value post_storage_root(Json::Value &post_data, const std::string &token);

  Json::Value get_by_json_query(const std::string &addr_path,
                                  Json::Value &query_data,
                                  long expected_response = 200,
                                  std::string token = "");

  /**
   * @brief get an object from the api by it's id
   *
   * @param table Api endpoint (table)
   * @param id id of the object
   * @param expected_response expected responce of the request (default 200)
   * @return Json::Value
   */
  Json::Value get_by_id(const std::string &table, int const &id,
                      long expected_response = 200, std::string token = "");

  /*! *************************************************************************
   * @brief returns the root URL for the RestAPI used by the API instance
   * @author K. Zarebski (UKAEA)
   *
   * @return URL of the RestAPI query endpoint
   ***************************************************************************/
  std::string get_url_root() const { return url_root_; }

  /**
   * @brief Formats a json object into a string representation which can be used
   * as a url endpoint query
   *
   * @param json_value the json object to be converted typically returned by the
   * post method
   * @return std::string
   */
  std::string json_to_query_string(Json::Value &json_value);

  /**
   * @brief escapes space " " characters within a string with it's html
   * character code (%20)
   *
   * @param str string containing space characters
   * @return std::string string with space characters represented by html
   * character code "%20"
   */
  std::string escape_space(std::string &str);

  /**
   * @brief Static function to append a given string with a "/" unless the
   * string is empty or already ends with a "/"
   *
   * @param string String to be appended with "/"
   * @return std::string Appended string
   */
  static std::string append_with_forward_slash(std::string string);

  /**
   * @brief Remove the leading forward slash from a given string
   * 
   * @param str 
   * @return std::string 
   */
  static std::string remove_leading_forward_slash(std::string str);

private:
  std::string url_root_;
  CURL *setup_json_session_(std::string &addr_path, std::string *response,
                            long &http_code, std::string token = "");
  CURL *setup_download_session_(const ghc::filesystem::path &addr_path,
                                FILE *file);  
                      
  Json::Value post_patch_request(const std::string addr_path, Json::Value &post_data,
                      const std::string &token, long expected_response, bool PATCH = false);

  // Legacy Method
  Json::Value get_request(const ghc::filesystem::path &addr_path,
                      long expected_response = 200, std::string token = "");

  void download_file(const ghc::filesystem::path &url,
                     ghc::filesystem::path out_path);
};

std::string url_encode(std::string url);

}; // namespace FDP

#endif
