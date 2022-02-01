#ifndef __FDP_IOOBJECT_HXX__
#define __FDP_IOOBJECT_HXX__

#include <string>
#include <cstddef>
#include <json/value.h>

#include "fdp/utilities/logging.hxx"
#include "fdp/objects/api_object.hxx"

namespace FDP {
    /**
     * @brief Class for API objects
     * 
     */
    class IOObject {
        private:
            std::string data_product_;
            std::string use_data_product_;
            std::string use_component_ = "None";
            std::string use_version_;
            std::string use_namespace_;
            ghc::filesystem::path path_;
            std::string data_product_description_ = "None";
            std::string component_description_ = "None";
            bool public_ = false;

            ApiObject::sptr component_obj_;
            ApiObject::sptr data_product_obj_;

        public:
            IOObject(){};
            IOObject(std::string data_product,
                    std::string use_data_product,
                    std::string use_version,
                    std::string use_namespace,
                    ghc::filesystem::path path,
                    std::string data_product_description,
                    bool isPublic)
                :
                    data_product_(data_product),
                    use_data_product_(use_data_product),
                    use_version_(use_version),
                    use_namespace_(use_namespace),
                    path_(path),
                    data_product_description_(data_product_description),
                    public_(isPublic){};

            IOObject(std::string data_product,
                        std::string use_data_product,
                        std::string use_version,
                        std::string use_namespace,
                        ghc::filesystem::path path,
                        ApiObject &component_obj,
                        ApiObject &data_product_obj) 
                :
                    data_product_(data_product),
                    use_data_product_(use_data_product),
                    use_version_(use_version),
                    use_namespace_(use_namespace),
                    path_(path),
                    component_obj_(std::make_shared<ApiObject>(component_obj)),
                    data_product_obj_(std::make_shared<ApiObject>(data_product_obj)){};

            std::string get_data_product() {return data_product_;}
            std::string get_use_data_product() {return use_data_product_;}
            std::string get_use_component() {return use_component_;}
            std::string get_use_version() {return use_version_;}
            std::string get_use_namespace() {return use_namespace_;}
            ghc::filesystem::path get_path() {return path_;}
            std::string get_data_product_description() {return data_product_description_;}
            std::string get_component_description() {return component_description_;}
            bool is_public() {return public_;}

            ApiObject::sptr get_component_object() {return component_obj_;}
            ApiObject::sptr get_data_product_object() {return data_product_obj_;}

            void set_component_object(ApiObject& component_obj){component_obj_ = std::make_shared<ApiObject>(component_obj);}
            void set_data_product_object(ApiObject &data_product_obj){data_product_obj_ = std::make_shared<ApiObject>(data_product_obj);}
    };
};

#endif
