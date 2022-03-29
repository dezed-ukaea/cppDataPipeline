/*! **************************************************************************
 * @file FairDataPipeline/utilities/logging.hxx
 * @author K. Zarebski (UKAEA)
 * @date 2021-05-06
 * @brief File contains pointers for creating global logger during session
 *
 * This file should be included in all FDP header files as it allows all
 * debug/error information to be output to the same logger and hence the
 * same terminal session. It uses the SPDLog library.
 ****************************************************************************/
#ifndef __FDP_LOGGING_HXX__
#define __FDP_LOGGING_HXX__

#include <iostream>
#include <sstream>
#include <string>

#include "spdlog/spdlog.h"
#if 0
#define TRACE 0
#define DEBUG 1
#define INFO 2
#define WARN 3
#define ERROR 4
#define CRITICAL 5
#define OFF 6
#endif


namespace FairDataPipeline {
    namespace logging {

        enum LOG_LEVEL
        {
            TRACE = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            CRITICAL = 5,
            OFF = 6
        };

        struct ISink
        {
                virtual int log( enum LOG_LEVEL msg_lvl, const std::string& s) = 0;
                virtual ~ISink(){};
        };

        class Sink : public ISink
        {
            public:
                Sink( enum LOG_LEVEL log_lvl ) :  ISink(), _log_lvl(log_lvl) {}

                virtual int log( enum LOG_LEVEL msg_lvl, const std::string& s){return 1;}

                template< typename T >
                    int execute( enum LOG_LEVEL msg_lvl, const T& s)
                    {
                        if( this->should_log( msg_lvl ) )
                        {
                            std::ostringstream oss;
                            oss << s;

                            this->log( msg_lvl, oss.str() );
                        }
                        return 0;
                    }

                enum LOG_LEVEL log_level(){ return _log_lvl;}
                void log_level( enum LOG_LEVEL log_lvl ){ _log_lvl = log_lvl;}

                bool should_log( enum LOG_LEVEL msg_lvl ){ return msg_lvl >= this->log_level();}

            private:

                enum LOG_LEVEL _log_lvl;
        };


        class OStreamSink : public Sink
        {
            public:
                OStreamSink(enum LOG_LEVEL log_lvl, std::ostream& os) : Sink(log_lvl), _os(os){ ;}

                int log( enum LOG_LEVEL msg_lvl, const std::string& s)
                {
                    if( this->should_log( msg_lvl ) )
                        _os << s;

                    return 0;
                }

            private:
                std::ostream& _os;
        };

        class CompositeSink : public Sink
        {
            public:
                CompositeSink(enum LOG_LEVEL log_lvl) : Sink(log_lvl){ ;}

                int log( enum LOG_LEVEL msg_lvl, const std::string& s)
                {
                    if( this->should_log( msg_lvl ) )
                    {
                        for( int i = 0; i < _sinks.size(); ++i )
                        {
                            Sink* sink = _sinks[i];
                            sink->log( msg_lvl, s );
                        }
                    }

                    return 0;
                }

                void add_sink( Sink* sink ){ _sinks.push_back( sink ); }

            private:
                std::vector< Sink* > _sinks;
        };



        class MsgLogger
        {
            public:
                MsgLogger( enum LOG_LEVEL msg_lvl, Sink& sink );

                template< typename T >
                    MsgLogger& operator<<( const T& s)
                    {
                        this->_sink.execute( _msg_lvl, s );
                        return *this;
                    }

            private:
                enum LOG_LEVEL _msg_lvl;
                Sink& _sink;
        };

        class Logger
        {
            public:

                Logger( enum LOG_LEVEL lvl, Sink& sink ) 
                    : _sink(sink)
                {
                    _log_lvl = lvl;
                }

                MsgLogger info();
                MsgLogger debug();
                MsgLogger trace();
                MsgLogger warn();
                MsgLogger critical();
                MsgLogger error();

            private:

                enum LOG_LEVEL _log_lvl;
                Sink& _sink;
        };
    }

    class logger
    {
        public:
            enum LOG_LEVEL
            {
                log_level_trace = 0,
                log_level_debug = 1,
                log_level_info = 2,
                log_level_warn = 3,
                log_level_error = 4,
                log_level_critical = 5,
                log_level_off = 6
            };

            typedef std::shared_ptr<spdlog::logger> sptr;

            static sptr get_logger();
        private:
            static sptr _instance;
    };

} // namespace FairDataPipeline



#endif
