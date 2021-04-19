#ifndef __SCRC_METADATA_HXX__
#define __SCRC_METADATA_HXX__

#include <iomanip>
#include <ctime>
#include <sstream>
#include <filesystem>
#include <fstream>

#include "digestpp.hpp"

namespace SCRC
{
    std::string calculate_hash_from_file(const std::filesystem::path&);

    std::string calculate_hash_from_string(const std::string& input);

    std::string generate_run_id(std::filesystem::path config_file_path);

    std::string current_time_stamp();

};

#endif