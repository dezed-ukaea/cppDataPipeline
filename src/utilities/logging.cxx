#include "fdp/utilities/logging.hxx"
#include <iostream>
#include <sys/time.h>
#include <time.h>
//#include <chrono>
//#include <chrono_io>

#include "spdlog/sinks/stdout_color_sinks.h"
namespace FairDataPipeline {
    namespace logging {
        const std::string& _LOG_LEVEL_2_str( enum  LOG_LEVEL lvl )
        {
            static const std::string strs[] = {"TRACE","DEBUG","INFO","WARN","ERROR","CRITICAL","OFF"};
            return strs[ lvl ];
        }

        std::string curtime()
        {
            timeval curTime;
            gettimeofday( &curTime, NULL );
            int milli = curTime.tv_usec / 1000;
            char buffer[ 80 ];
            strftime( buffer, 80, "%Y-%m-%d %H:%M:%S", localtime( &curTime.tv_sec ));
            std::string currentTime( 84, 0 );

            sprintf( &currentTime[0], "%s.%03d", buffer, milli );
            return std::move( currentTime );
        }

        Logger::MsgBuilder::MsgBuilder( enum LOG_LEVEL msg_lvl, Logger* logger ) 
            : _msg_lvl( msg_lvl ), _logger(logger)
        {
        }

        Logger::MsgBuilder::MsgBuilder( const Logger::MsgBuilder& rhs ) //: _sink( rhs._sink )
        {
            this->_msg_lvl = rhs. _msg_lvl;
            _logger = rhs._logger;
        }

        Logger::MsgBuilder::~MsgBuilder()
        {
            *this << "\n";

            std::string msg = _oss.str();

            this->_logger->sink()->execute( _logger, _msg_lvl, msg );
        }

        Logger::MsgBuilder Logger::info()
        {
            return std::move( MsgBuilder( INFO, this ) ); 
        }

        Logger::MsgBuilder Logger::debug()
        {
            return std::move( MsgBuilder( DEBUG, this ) ); 
        }

        Logger::MsgBuilder Logger::trace()
        {
            return std::move( MsgBuilder( TRACE, this ) ); 
        }

        Logger::MsgBuilder Logger::warn()
        {
            return std::move( MsgBuilder( WARN, this  ) );
        }

        Logger:: MsgBuilder Logger::critical()
        {
            return std::move( MsgBuilder( WARN, this ) ); 
        }

        Logger::MsgBuilder Logger::error()
        {
            return std::move(  MsgBuilder( ERROR, this ) ); 
        }

        Sink::Sink( enum LOG_LEVEL log_lvl ) 
            :  ISink(), _log_lvl(log_lvl) 
        {
            _fmtr = SinkFormatter::create();
        }

        enum LOG_LEVEL Sink::log_level(){ return _log_lvl;}

        void Sink::log_level( enum LOG_LEVEL log_lvl ){ _log_lvl = log_lvl;}

        bool Sink::should_log( enum LOG_LEVEL msg_lvl ){ return msg_lvl >= this->log_level();}

        int Sink::execute( Logger* logger, enum LOG_LEVEL msg_lvl, const std::string& s )
        {
            if( this->should_log( msg_lvl ) )
            {
                if( NULL != _fmtr )
                {
                    std::ostringstream oss;
                    oss << _fmtr->header( msg_lvl, logger );

                    oss << s;

                    this->log( msg_lvl, oss.str() );
                }
                else
                    this->log( msg_lvl, s );
            }
            return 0;
        }

        std::string Sink::apply_formatter( Logger* logger, enum LOG_LEVEL msg_lvl, const std::string& msg )
        {
            std::ostringstream oss;

            if( NULL != _fmtr )
            {
                oss << _fmtr->header( msg_lvl, logger );
            }
            oss << msg;
            return std::move( oss.str() );
        }


        std::string SinkFormatter::header( enum LOG_LEVEL msg_lvl, Logger* logger )
        {
            std::string dt = curtime();
            std::ostringstream oss;

            oss << "[" << dt << "] ";

            if( !(logger->name().empty()) )
                oss << "[" << logger->name() << "] ";


            oss << "[" << _LOG_LEVEL_2_str( msg_lvl ) << "] ";
            return std::move( oss.str() );
        }

        int CompositeSink::log( enum LOG_LEVEL msg_lvl, const std::string& s)
        {
            if( this->should_log( msg_lvl ) )
            {
                for( int i = 0; i < _sinks.size(); ++i )
                {
                    Sink::sptr sink = _sinks[i];
                    sink->log( msg_lvl, s );
                }
            }

            return 0;
        }



    }

    class SPDSink : public logging::Sink
    {
        public:
            SPDSink( logging::LOG_LEVEL lvl ) 
                : logging::Sink( lvl )
            {
                //_spd_log = logger::get_spd_logger();
            }


        private:
            //typedef logger::spd_sptr spd_ptr;
            //spd_ptr _spd_log;


    };


    logger::logger_sptr logger::_instance = NULL;
    
    logger::logger_sptr logger::get_logger()
    {
        if( NULL == logger::_instance )
        {
            auto sink = logging::OStreamSink::create( logging::LOG_LEVEL::INFO, std::cout );
            logger::_instance = logging::Logger::create( logging::LOG_LEVEL::INFO, sink, "FDPAPI" );
        }

        return logger::_instance;

    }


} // namespace FairDataPipeline
