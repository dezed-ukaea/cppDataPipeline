#ifndef __SCRC_LOGGING_HXX__
#define __SCRC_LOGGING_HXX__

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace SCRC
{
    std::shared_ptr<spdlog::logger> create_logger_();
    const std::shared_ptr<spdlog::logger> APILogger = create_logger_();
}

#endif 