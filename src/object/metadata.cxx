#include "scrc/objects/metadata.hxx"

namespace SCRC
{
    std::string calculate_hash_from_file(const std::filesystem::path& file_path)
    {
        if(!std::filesystem::exists(file_path))
        {
            throw std::invalid_argument(
                "File '"+file_path.string()+"' not found"
            );
        }

        std::ifstream file_(file_path, std::ios_base::in|std::ios_base::binary);

        const std::string hash_ = digestpp::sha1().absorb(file_).hexdigest();

        file_.close();

        return hash_;
    }

    std::string calculate_hash_from_string(const std::string& input)
    {
        return digestpp::sha1().absorb(input).hexdigest();
    }

    std::string generate_run_id(std::filesystem::path config_file_path)
    {
        const std::string config_hash_ = calculate_hash_from_file(config_file_path);
        auto t_ = std::time(nullptr);
        auto tm_ = *std::gmtime(&t_);

        std::ostringstream oss_;
        oss_ << std::put_time(&tm_, "%Y%m%d%H%M%S");
        const std::string time_str_ = oss_.str();

        return config_hash_ + time_str_;

    }

    std::string current_time_stamp()
    {
        auto t_ = std::time(nullptr);
        auto tm_ = *std::localtime(&t_);

        std::ostringstream oss_;
        oss_ << std::put_time(&tm_, "%Y-%m-%d %H:%M:%S %Z");

        return oss_.str();

    }
};