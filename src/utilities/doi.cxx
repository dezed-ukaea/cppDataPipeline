#include "scrc/utilities/doi.hxx"

namespace SCRC
{
    bool DOI::check_prefix_(std::string& prefix)
    {
        const std::regex prefix_reg_{"10.[0-9.]+"};

        const bool check_ = std::regex_match(prefix, prefix_reg_);

        if(!check_)
        {
            throw std::runtime_error("'"+prefix+"' is not a valid data object identifier prefix");
        }

        return check_;
    }

    bool DOI::check_suffix_(std::string& suffix)
    {
        const std::regex suffix_reg_{"[0-9A-Za-z_.]+"};

        const bool check_ = std::regex_match(suffix, suffix_reg_);

        if(!check_)
        {
            throw std::runtime_error("'"+suffix+"' is not a valid data object identifier suffix");
        }

        return check_;
    }

    DOI doi_from_string(std::string doi_id)
    {
        if(doi_id.find("/") == std::string::npos)
        {
            throw std::runtime_error("Expected '/' in DOI identifier");
        }

        const size_t slash_point_ = doi_id.find("/");

        return DOI(
            doi_id.substr(0, slash_point_),
            doi_id.substr(slash_point_+1, doi_id.size()-1)
        );
    }
};