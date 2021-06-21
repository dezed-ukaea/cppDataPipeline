#include "fdp/utilities/logging.hxx"

namespace FDP {
std::shared_ptr<spdlog::logger> create_logger_() {
  if (!spdlog::get("FDPAPI")) {
    return spdlog::stdout_color_st("FDPAPI");
  } else {
    return spdlog::get("FDPAPI");
  }
}
} // namespace FDP