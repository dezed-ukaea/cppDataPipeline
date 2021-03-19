#include "scrc/registry/file.hxx"

namespace SCRC
{
    std::string calculate_hash(std::filesystem::path file_path)
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
};