#ifndef __FDP_EXCEPTIONS_HXX__
#define __FDP_EXCEPTIONS_HXX__

#include <stdexcept>
#include <string>

namespace fdp {
class config_parsing_error : public std::runtime_error {
public:
  config_parsing_error(const std::string& message)
      : std::runtime_error(message) {}
};

class rest_apiquery_error : public std::runtime_error {
public:
  rest_apiquery_error(const std::string& message)
      : std::runtime_error(message) {}
};

class json_parse_error : public std::runtime_error {
public:
  json_parse_error(const std::string& message) : std::runtime_error(message) {}
};

class validation_error : public std::runtime_error {
public:
  validation_error(const std::string& message) : std::runtime_error(message) {}
};

class sync_error : public std::runtime_error {
public:
  sync_error(const std::string& message) : std::runtime_error(message) {}
};

class write_error : public std::runtime_error {
public:
  write_error(const std::string& message) : std::runtime_error(message) {}
};

}; // namespace fdp

#endif
