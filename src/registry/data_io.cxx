#include "fdp/registry/data_io.hxx"
#include "fdp/objects/config.hxx"
namespace FDP {
YAML::Node parse_yaml_(ghc::filesystem::path yaml_path) {
  APILogger->debug("LocalFileSystem: Reading configuration file '{0}'",
                   yaml_path.string().c_str());
  return YAML::LoadFile(yaml_path.string().c_str());
}

double read_point_estimate_from_toml(const ghc::filesystem::path var_address) {
  if (!ghc::filesystem::exists(var_address)) {
    throw std::runtime_error("File '" + var_address.string() +
                             "' could not be opened as it does not exist");
  }

  const auto toml_data_ = toml::parse(var_address.string());

  const std::string first_key_ = get_first_key_(toml_data_);

  if (!toml::get<toml::table>(toml_data_).at(first_key_).contains("type")) {
    throw std::runtime_error("Expected 'type' tag but none found");
  }

  if (static_cast<std::string>(
          toml_data_.at(first_key_).at("type").as_string()) !=
      "point-estimate") {
    throw std::runtime_error(
        "Expected 'point-estimate' for type but got '" +
        static_cast<std::string>(toml_data_.at("type").as_string()) + "'");
  }

  return (toml_data_.at(first_key_).at("value").is_floating())
             ? toml_data_.at(first_key_).at("value").as_floating()
             : static_cast<double>(
                   toml_data_.at(first_key_).at("value").as_integer());
}

std::string get_first_key_(const toml::value data_table) {
  for (auto const& i : data_table.as_table()) {
    return i.first;
  }

  return "";
}
}; // namespace FDP