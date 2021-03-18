#include "scrc/registry/options.hxx"

namespace SCRC
{
    std::string DataProduct::to_string(DataProduct::Filters filter)
    {
        const std::string strings_[6] = {
            "updated_by",
            "last_updated",
            "object",
            "namespace",
            "name",
            "version"
        };

        return strings_[int(filter)];
    }

    std::string Object::to_string(Object::Filters filter)
    {
            const std::string strings_[18] = {
                "components",
                "code_repo_of",
                "config_of",
                "submission_script_of",
                "external_object",
                "quality_control",
                "keywords",
                "authors",
                "licences",
                "data_product",
                "code_repo_release",
                "metadata",
                "updated_by",
                "last_updated",
                "storage_location",
                "description",
                "file_type",
                "issues"
            };

            return strings_[int(filter)];
        }
};