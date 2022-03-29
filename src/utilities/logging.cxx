#include "fdp/utilities/logging.hxx"
#include <iostream>

#include "spdlog/sinks/stdout_color_sinks.h"
namespace FairDataPipeline {
namespace logging {

    MsgLogger::MsgLogger( enum LOG_LEVEL msg_lvl, Sink& sink ) 
        : _msg_lvl( msg_lvl ), _sink(sink)
    {
    }

    MsgLogger Logger::info()
    {
        MsgLogger msg_logger( INFO, _sink ); 
        msg_logger << "[INFO    ]";
        return msg_logger;
    }
    
    MsgLogger Logger::debug()
    {
        MsgLogger msg_logger( DEBUG, _sink ); 
        msg_logger << "[ DEBUG  ]";
        return msg_logger;
    }
    
    MsgLogger Logger::trace()
    {
        MsgLogger msg_logger( TRACE, _sink ); 
        msg_logger << "[TRACE   ]";
        return msg_logger;
    }
    
    MsgLogger Logger::warn()
    {
        MsgLogger msg_logger( WARN, _sink  );
        msg_logger << "[WARN    ]";
        return msg_logger;
    }
    
    MsgLogger Logger::critical()
    {
        MsgLogger msg_logger( WARN, _sink ); 
        msg_logger << "[CRITICAL]";
        return msg_logger;
    }
    
    MsgLogger Logger::error()
    {
        MsgLogger msg_logger( ERROR, _sink ); 
        msg_logger << "[ERROR   ]";
        return msg_logger;
    }
}


    logger::sptr logger::_instance = NULL;

    static logger::sptr _create_logger() 
    {
        logger::sptr the_logger = NULL;

        if (!spdlog::get("FDPAPI")) 
        {
            the_logger = spdlog::stdout_color_st("FDPAPI");
        } else
        {
            the_logger = spdlog::get("FDPAPI");
        }

        return the_logger;
    }

    logger::sptr logger::get_logger()
    {
#if  1
        if( NULL == logger::_instance )
        {
            logger::_instance = _create_logger();
            spdlog::set_default_logger(logger::_instance);
        }

        return logger::_instance;
#else

        static Logger logger( INFO, std::cout );
        return logger;
#endif

    }

} // namespace FairDataPipeline
