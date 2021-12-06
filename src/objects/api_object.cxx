#include "fdp/objects/api_object.hxx"

namespace FDP {
    /**
     * @brief Returns the object id from the object uri_
     * 
     * @return int of the object id
     */
    int ApiObject::get_id(){
       return get_id_from_string(uri_);
    }

    int ApiObject::get_id_from_string(std::string url){
        if (url.back() == '/')        {
            url = url.substr(0, url.size() - 1);
        }
        std::size_t id_pos = url.find_last_of("/");
        return std::stoi(url.substr(id_pos + 1));
    }
    /**
     * @brief Returns the object type from the object uri_ (table)
     * 
     * @return std::string The object type
     */
    std::string ApiObject::get_type(){
        std::size_t id_pos = uri_.find_last_of("/");
        std::string uri_prefix = uri_.substr(0, id_pos);
        id_pos = uri_prefix.find_last_of("/");
        return uri_prefix.substr(id_pos + 1);
    }
};