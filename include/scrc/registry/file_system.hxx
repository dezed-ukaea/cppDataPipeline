#ifndef __SCRC_FILESYSTEM_HXX__
#define __SCRC_FILESYSTEM_HXX__

#include <string>
#include <filesystem>
#include <yaml-cpp/yaml.h>

namespace SCRC
{
    YAML::Node parse_config_();

    class LocalFileSystem
    {
        private:
            const std::filesystem::path config_path_;
            const YAML::Node config_data_;
        public:
            LocalFileSystem(std::filesystem::path config_file_path);
            std::string get_namespace() const;
    };
};

#endif