#include "scrc/utilities/semver.hxx"


namespace SCRC
{
    version::version(const std::string version_str)
    {
        const std::string delim_ = ".";
        const std::string meta_delim_ = "+";
        const std::string rel_delim_ = "-";

        std::string v_proc_ = version_str;

        size_t pos = 0;
        std::vector<int> components_;

        const std::string alpha_str_ = "alpha";
        const std::string beta_str_ = "beta";
        const std::string rel_cand_str_ = "rc";

        // Check if any metadata attached to version number
        if((pos = version_str.find(meta_delim_)) != std::string::npos)
        {
            meta_data_ = v_proc_.substr(pos+1, v_proc_.size());
            v_proc_.erase(pos, v_proc_.size());
            pos = 0;
        }

        // Check if release string on version number
        if((pos = v_proc_.find(rel_delim_)) != std::string::npos)
        {
            std::string rel_ = "";
            rel_ = v_proc_.substr(pos+1, v_proc_.size());
            v_proc_.erase(pos, v_proc_.size());

            if(rel_.substr(0, alpha_str_.size()) == alpha_str_)
            {
                meta_tag_ = meta_tag::ALPHA;
                if(rel_.size() > alpha_str_.size())
                {
                    tag_v_ = std::stoi(rel_.substr(alpha_str_.size()+1, rel_.size()));
                }
            }

            else if(rel_.substr(0, beta_str_.size()) == beta_str_)
            {
                meta_tag_ = meta_tag::BETA;
                if(rel_.size() > beta_str_.size())
                {
                    tag_v_ = std::stoi(rel_.substr(beta_str_.size()+1, rel_.size()));
                }
            }

            else if(rel_.substr(0, rel_cand_str_.size()) == rel_cand_str_)
            {
                meta_tag_ = meta_tag::RELEASE_CANDIDATE;
                if(rel_.size() > rel_cand_str_.size())
                {
                    tag_v_ = std::stoi(rel_.substr(rel_cand_str_.size()+1, rel_.size()));
                }
            }

            v_proc_.erase(pos, v_proc_.size());
            pos = 0;
        }

        while((pos = v_proc_.find(delim_)) != std::string::npos)
        {
            components_.push_back(std::stoi(v_proc_.substr(0, pos)));
            v_proc_.erase(0, pos + delim_.length());
        }

        components_.push_back(std::stoi(v_proc_));

        if(components_.size() != 3)
        {
            throw std::runtime_error("Expected three numeric components in main tag of semantic version, got "+std::to_string(components_.size()));
        }

        major_ = components_[0];
        minor_ = components_[1];
        patch_ = components_[2];
    }

    std::string version::get_tag_str() const
    {
        switch(meta_tag_)
        {
            case(meta_tag::ALPHA):
                return "alpha";
            case(meta_tag::BETA):
                return "beta";
            case(meta_tag::RELEASE_CANDIDATE):
                return "rc";
            default:
                return "";

        };
    }
}