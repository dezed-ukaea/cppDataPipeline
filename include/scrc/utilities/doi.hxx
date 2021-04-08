#ifndef __DOI_SCRC_HXX__
#define __DOI_SCRC_HXX__

#include <string>
#include <regex>

#include "scrc/exceptions.hxx"

namespace SCRC
{
    // Digital Object Identifier
    class DOI
    {
        private:
            const std::string registrant_id_ = "";
            const std::string object_id_ = "";
            bool check_prefix_(std::string& prefix);
            bool check_suffix_(std::string& suffix);
        public:
            DOI() {};
            DOI(std::string reg_id, std::string obj_id) : 
                registrant_id_(reg_id),
                object_id_(obj_id)
            {
                check_prefix_(reg_id);
                check_suffix_(obj_id);
            }

            std::string to_string() const
            {
                return registrant_id_+"/"+object_id_;
            }

            explicit operator bool()
            {
                return !registrant_id_.empty() && !object_id_.empty();
            }
    };

    DOI doi_from_string(std::string doi_id);
};

#endif