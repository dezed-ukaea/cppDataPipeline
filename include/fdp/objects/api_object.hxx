#ifndef __FDP_APIOBJECT_HXX__
#define __FDP_APIOBJECT_HXX__

#include <cstddef>
#include <json/value.h>
#include <string>

#include "fdp/utilities/logging.hxx"

namespace FDP {
/**
 * @brief Class for API objects
 *
 */
class ApiObject {
private:
  Json::Value obj_;

  ApiObject();

public:
  typedef std::shared_ptr<ApiObject> sptr;

  static sptr from_json(const Json::Value &j);

  // static copy( const ApiObject& src );

  /**
   * @brief Construct a new Api Object object
   *
   * @param uri uri of the api object e.g. http://127.0.0.1/object/1
   */

  static ApiObject::sptr construct(void);

  int add(const std::string &key, int value);
  int add(const std::string &key, float value);
  int add(const std::string &key, double value);
  int add(const std::string &key, const std::string &value);
  int add(const std::string &key, const ApiObject &value);

  int remove(const std::string &key);
  /**
   * @brief Get the object id from the uri
   *
   * @return int object id e.g. if the uri is http://127.0.0.1/object/1
   * the object id will be 1
   */
  int get_id();
  /**
   * @brief Get the object type from the uri
   *
   * @return std::string the object type (table)
   * e.g. if the uri is http://127.0.0.1/object/1 the object type will be object
   */
  static int get_id_from_string(std::string url);

  std::string get_type();
  /**
   * @brief Get the object uri
   *
   * @return std::string the uri of the object e.g. http://127.0.0.1/object/1
   */
  std::string get_uri() const;

  /**
   * @brief Get the value of a given key as string object
   *
   * @param key
   * @return std::string
   */
  std::string get_value_as_string(std::string key) const;

  /**
   * @brief Get the value of a given key as int object
   *
   * @param key
   * @return int
   */
  int get_value_as_int(std::string key) const;

  /**
   * @brief Get the first component of the object
   *
   * @return std::string
   */
  std::string get_first_component() const;

  /**
   * @brief Check to see if the object is empty
   *
   * @return true the object does not contain any data
   * @return false the object contains data
   */
  bool is_empty();
};
}; // namespace FDP

#endif
