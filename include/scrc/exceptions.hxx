#ifndef __SCRC_EXCEPTIONS_HXX__
#define __SCRC_EXCEPTIONS_HXX__

#include <stdexcept>
#include <string>

namespace SCRC
{
    class config_parsing_error: public std::runtime_error
    {
        public:
            config_parsing_error(char const* const message) :
                std::runtime_error(message) {}
    };

    class rest_api_query_error: public std::runtime_error
    {
        public:
            rest_api_query_error(const std::string message) :
                std::runtime_error(message) {}
    };

    class validation_error: public std::runtime_error
    {
        public:
            validation_error(const std::string message) :
                std::runtime_error(message) {}
    };
};

#endif