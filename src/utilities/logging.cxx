#include "fdp/utilities/logging.hxx"

namespace FDP {

    /*! **************************************************************************
     * @brief Create a the global logger pointer
     * @author K. Zarebski (UKAEA)
     *
     * @return a shared pointer to the global logger instance
     ****************************************************************************/
    static logger::sptr create_logger() {
        if (!spdlog::get("FDPAPI")) {
            return spdlog::stdout_color_st("FDPAPI");
        } else {
            return spdlog::get("FDPAPI");
        }
    }

    const logger::sptr APILogger = create_logger();
} // namespace FDP
