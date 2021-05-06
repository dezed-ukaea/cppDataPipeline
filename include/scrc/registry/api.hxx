/*! **************************************************************************
 * @file scrc/registry/api.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-05
 * @brief File containing classes and methods for connecting to the RestAPI
 *
 * The classes and methods within this file are used to access the SCRC
 * Data Pipeline RestAPI, they handle sending of requests and retrieval of
 * data as JSON strings
 ****************************************************************************/
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
/*! **************************************************************************
 * @enum RESTAPI
 * @brief selection of either local or remote pipeline running
 * @author K. Zarebski (UKAEA)
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
 * @class Query
 * @brief class for constructing queries to be sent to the RestAPI server
 * @author K. Zarebski (UKAEA)
 *
 * The Query class provides a base class for constructing query strings which
 * are then sent to the RestAPI to obtain information from the pipeline
 ****************************************************************************/
class Query {
private:
  const std::vector<std::string> valid_filters_;
  std::string string_repr_;
  std::map<std::string, std::string> components_;
  const std::filesystem::path fragments_;

public:
  /*! ************************************************************************
   * @brief Construct a new query instance from a category and set of filters
   * @author K. Zarebski (UKAEA)
   *
   * @param string_repr the RestAPI query type, e.g. 'object'
   * @param filters a list of components for filtering the results
   **************************************************************************/
  Query(std::string string_repr, std::vector<std::string> filters)
      : string_repr_(string_repr), valid_filters_(filters) {}

  /*! ************************************************************************
   * @brief Construct a new query instance from a category and a path
   * @author K. Zarebski (UKAEA)
   *
   * @param string_repr the RestAPI query type, e.g. 'object'
   * @param query_path a path for narrowing/filtering the query
   **************************************************************************/
  Query(std::string string_repr, std::filesystem::path query_path)
      : string_repr_(string_repr), fragments_(query_path) {}

  /*! ************************************************************************
   * @brief append a key-value pair to the query for filtering results
   * @author K. Zarebski (UKAEA)
   *
   * @param key the key to use, should be an available filter for the given
   * RestAPI query category
   * @param value the value to filter by
   **************************************************************************/
  void append(std::string key, std::string value);

  /*! ************************************************************************
   * @brief assembles the query into a URL to be requested from
   * @author K. Zarebski (UKAEA)
   *
   * @return std::filesystem::path the query as a URL
   **************************************************************************/
  std::filesystem::path build_query();
};

/*! **************************************************************************
 * @class API
 * @brief a class which handles the fetching and posting of information to the
 * SCRC data pipeline RestAPI
 * @author K. Zarebski (UKAEA)
 *
 * The API class has know specific knowledge about the RestAPI but rather
 * provides the interface for sending/receiving data as JSON strings
 *****************************************************************************/
class API {
public:
  /*! *************************************************************************
   * @brief construct an API object using the given URL as the root
   * @author K. Zarebski (UKAEA)
   *
   * @param url_root the root of the query address, e.g. localhost:8000/api
   ***************************************************************************/
  API(std::string url_root) : url_root_(url_root) {}

  /*! *************************************************************************
   * @brief sends the given 'packet' of information to the RestAPI
   * @author K. Zarebski (UKAEA)
   *
   * @param addr_path the relative path within the RestAPI to send to
   * @param expected_response the expected return HTTP code
   * @return Json::Value JSON object containing the information for the request
   ***************************************************************************/
  Json::Value request(const std::filesystem::path &addr_path,
                      long expected_response = 200);

  /*! *************************************************************************
   * @brief read a query object and send request to the RestAPI
   * @author K. Zarebski (UKAEA)
   *
   * @param query query object containing filters, and request information
   * @param expected_response the expected HTTP code to be returned
   ***************************************************************************/
  Json::Value query(Query query, long expected_response = 200);

  /*! *************************************************************************
   * @brief post information from a JSON value object to the RestAPI
   * @author K. Zarebski (UKAEA)
   *
   * @param addr_path relative URL path to post to
   * @param post_data the data to transmit
   * @param key authorisation key if required
   * @param expected_response the expected HTTP return code
   * @return the result of the API post as a JSON object
   ***************************************************************************/
  Json::Value post(const std::filesystem::path &addr_path,
                   Json::Value &post_data, const std::string &key,
                   long expected_response = 201);

  /*! *************************************************************************
   * @brief returns the root URL for the RestAPI used by the API instance
   * @author K. Zarebski (UKAEA)
   *
   * @return URL of the RestAPI query endpoint
   ***************************************************************************/
  std::string get_url_root() const { return url_root_; }

  /*! *************************************************************************
   * @brief download a file returned by a RestAPI query
   * @author K. Zarebski (UKAEA)
   *
   * @param url the relative address to the data pipeline object
   * @param out_path the path to download the file to on the local system
   ***************************************************************************/
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