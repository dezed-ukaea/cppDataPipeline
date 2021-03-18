#ifndef __SCRC_REGISTRY_OPTIONS_HXX__
#define __SCRC_REGISTRY_OPTIONS_HXX__

#include <string>

namespace SCRC
{
    namespace DataProduct
    {
        enum Filters
        {
            UPDATED_BY,
            LAST_UPDATED,
            OBJECT,
            NAMESPACE,
            NAME,
            VERSION
        };

        std::string to_string(Filters filter);
    };

    namespace Object
    {
        enum Filters
        {
            COMPONENTS,
            CODE_REPO_OF,
            CONFIG_OF,
            SUBMISSION_SCRIPT_OF,
            EXTERNAL_OBJECT,
            QUALITY_CONTROL,
            KEYWORDS,
            AUTHORS,
            LICENSES,
            DATA_PRODUCT,
            CODE_REPO_RELEASE,
            METADATA,
            UPDATED_BY,
            LAST_UPDATED,
            STORAGE_LOCATION,
            DESCRIPTION,
            FILE_TYPE,
            ISSUES
        };

        std::string to_string(Filters filter);

    };
};

#endif