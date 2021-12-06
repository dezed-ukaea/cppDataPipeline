#ifndef __FDP_APIOBJECT_HXX__
#define __FDP_APIOBJECT_HXX__

#include <string>
#include <cstddef>

#include "fdp/utilities/logging.hxx"

namespace FDP {
    /**
     * @brief Class for API objects
     * 
     */
    class ApiObject {
        private:
            std::string uri_;
        public:
            /**
             * @brief Construct a new Api Object object
             * 
             * @param uri uri of the api object e.g. http://127.0.0.1/object/1
             */
            ApiObject(std::string uri) : uri_(uri){}
            /**
             * @brief Get the object id from the uri
             * 
             * @return int object id e.g. if the uri is http://127.0.0.1/object/1 
             * the object id will be 1
             */
            int get_id();
            /**
             * @brief Get the object type from the uri
             * 
             * @return std::string the object type (table) 
             * e.g. if the uri is http://127.0.0.1/object/1 the object type will be object 
             */
            static int get_id_from_string(std::string url);

            std::string get_type();
            /**
             * @brief Get the object uri
             * 
             * @return std::string the uri of the object e.g. http://127.0.0.1/object/1
             */
            std::string get_uri() const {return uri_;}
    };
};

#endif