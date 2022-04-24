#include <iostream>
#include <map>
#ifdef _WIN32
    #include <windows_sys/time.h>
#else
    #include <sys/time.h>
#endif
#include <time.h>
#include <ctime>

#include "fdp/utilities/logging.hxx"

namespace FairDataPipeline {
    namespace logging {

        const std::string& to_string( enum  LOG_LEVEL lvl )
        {
            static const std::string strs[] = {"TRACE","DEBUG","INFO","WARN","ERROR","CRITICAL","OFF"};
            return strs[ lvl ];
        }

        enum LOG_LEVEL to_LOG_LEVEL( const std::string& s )
        {
            enum LOG_LEVEL lvl = LOG_LEVEL::OFF;
            static std::map< std::string, enum LOG_LEVEL > m = {
                {"TRACE", LOG_LEVEL::TRACE }
               , {"DEBUG", LOG_LEVEL::DEBUG }
               , {"INFO", LOG_LEVEL::INFO }
               , {"WARN", LOG_LEVEL::WARN }
               , {"ERROR", LOG_LEVEL::ERROR }
               , {"CRITICAL", LOG_LEVEL::CRITICAL }
               , {"OFF", LOG_LEVEL::OFF }
            };

            auto it = m.find( s );

            if( it != m.end() )
                lvl = it->second;

            return lvl;
        }

        static std::string curtime()
        {
            timeval curTime;
            gettimeofday( &curTime, NULL );
            int milli = curTime.tv_usec / 1000;
            char buffer[ 80 ];
            std::time_t _tv_sec = curTime.tv_sec;
            strftime( buffer, 80, "%Y-%m-%d %H:%M:%S", localtime( &_tv_sec ));
            std::string currentTime( 84, 0 );

            sprintf( &currentTime[0], "%s.%03d", buffer, milli );
            return std::move( currentTime );
        }

        Logger::Logger( enum LOG_LEVEL lvl, Sink::sptr sink, std::string name ) 
            : _sink(sink), _name(name)
        {
            _log_lvl = lvl;
        }

        Logger::sptr Logger::create( enum LOG_LEVEL lvl, Sink::sptr sink, std::string name )
        { 
            return sptr( new Logger(lvl, sink, name));
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
            if( _oss.tellp() > 0 )
            {
                *this << "\n";

                std::string msg = _oss.str();

                this->_logger->sink()->execute( _logger, _msg_lvl, msg );
            }
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

        int Sink::execute( Logger* logger, enum LOG_LEVEL msg_lvl, const std::string& msg )
        {
            if( this->should_log( msg_lvl ) )
            {
                if( NULL != _fmtr )
                {
                    //std::ostringstream oss;
                    //oss << _fmtr->header( msg_lvl, logger );
                    //oss << msg;
                    //
                    //this->log( msg_lvl, oss.str() );
                    std::string s = _fmtr->header(msg_lvl, logger);
                    s += msg;

                    this->log( msg_lvl, s );
                }
                else
                    this->log( msg_lvl, msg );
            }
            return 0;
        }

        OStreamSink::sptr OStreamSink::create( enum LOG_LEVEL lvl, std::ostream& os )
        {
            return sptr( new OStreamSink( lvl, os ) ); 
        }

        int OStreamSink::log( enum LOG_LEVEL msg_lvl, const std::string& s)
        {
            _os << s;

            return 0;
        }

        OStreamSink::OStreamSink(enum LOG_LEVEL log_lvl, std::ostream& os) : Sink(log_lvl), _os(os)
        {
        }
                

        std::string SinkFormatter::header( enum LOG_LEVEL msg_lvl, Logger* logger )
        {
            std::string dt = curtime();
            std::ostringstream oss;

            oss << "[" << dt << "] ";

            if( !(logger->name().empty()) )
                oss << "[" << logger->name() << "] ";

            oss << "[" << to_string( msg_lvl ) << "] ";

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


    logger::logger_sptr logger::_instance = NULL;
    
    logger::logger_sptr logger::get_logger()
    {
        if( NULL == logger::_instance )
        {
            enum logging::LOG_LEVEL log_lvl = logging::OFF;

            if( const char* env = std::getenv( "FDP_LOG_LEVEL" ) )
                log_lvl = logging::to_LOG_LEVEL( env );

            auto sink = logging::OStreamSink::create( log_lvl, std::cout );
            logger::_instance = logging::Logger::create( log_lvl, sink, "FDPAPI" );
        }

        return logger::_instance;
    }

} // namespace FairDataPipeline
