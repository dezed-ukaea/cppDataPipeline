#include "scrc/utilities/logging.hxx"

namespace SCRC
{
    std::shared_ptr<spdlog::logger> create_logger_()
    {
        if(!spdlog::get("SCRCAPI"))
        {
            return spdlog::stdout_color_st("SCRCAPI");
        }
        else
        {
            return spdlog::get("SCRCAPI");
        }
        
    }
}