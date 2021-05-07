/*! **************************************************************************
 * @file scrc/utilities/logging.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-06
 * @brief File contains pointers for creating global logger during session
 *
 * This file should be included in all SCRC header files as it allows all
 * debug/error information to be output to the same logger and hence the
 * same terminal session. It uses the SPDLog library.
 ****************************************************************************/
#ifndef __SCRC_LOGGING_HXX__
#define __SCRC_LOGGING_HXX__

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

namespace SCRC {
/*! **************************************************************************
 * @brief Create a the global logger pointer
 * @author K. Zarebski (UKAEA)
 * 
 * @return a shared pointer to the global logger instance
 ****************************************************************************/
std::shared_ptr<spdlog::logger> create_logger_();

/**
 * @brief Global logger used across all SCRC scripts
 * @author K. Zarebski (UKAEA)
 * 
 */
const std::shared_ptr<spdlog::logger> APILogger = create_logger_();
} // namespace SCRC

#endif