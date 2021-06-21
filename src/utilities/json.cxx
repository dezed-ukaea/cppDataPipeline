#include "fdp/utilities/json.hxx"

namespace FDP {
std::string json_to_string(Json::Value &json_data) {
  Json::StreamWriterBuilder json_str_builder_;
  return Json::writeString(json_str_builder_, json_data);
}
} // namespace FDP