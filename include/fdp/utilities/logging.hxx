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
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

        const std::string& to_string( enum  LOG_LEVEL lvl );

        struct ISink
        {
            typedef std::shared_ptr< ISink > sptr;
            virtual int log( enum LOG_LEVEL msg_lvl, const std::string& msg ) = 0;
            virtual ~ISink(){};
        };

        class Logger;

        class SinkFormatter;
        typedef std::shared_ptr< SinkFormatter > SinkFormatterPtr;

        struct ISinkFormatter
        {
            typedef std::shared_ptr< ISinkFormatter > sptr;
            virtual std::string header(enum LOG_LEVEL msg_lvl, Logger* logger )= 0;
            virtual ~ISinkFormatter(){;}
        };


        class Sink : public ISink
        {
            public:
                typedef std::shared_ptr< Sink > sptr;


                enum LOG_LEVEL log_level();
                void log_level( enum LOG_LEVEL log_lvl );

                bool should_log( enum LOG_LEVEL msg_lvl );

                void set_formatter( ISinkFormatter::sptr fmtr ){_fmtr = fmtr;}
                
                int execute( Logger* logger, enum LOG_LEVEL msg_lvl, const std::string& s );

            protected:
                Sink( enum LOG_LEVEL log_lvl );

            private:


                enum LOG_LEVEL _log_lvl;
                ISinkFormatter::sptr _fmtr;
        };


        class OStreamSink : public Sink
        {
            public:
                typedef std::shared_ptr< OStreamSink > sptr;

                static sptr create( enum LOG_LEVEL lvl, std::ostream& os );

                int log( enum LOG_LEVEL msg_lvl, const std::string& s);

            private:

                OStreamSink(enum LOG_LEVEL log_lvl, std::ostream& os);

                std::ostream& _os;
        };

        class CompositeSink : public Sink
        {
            public:
                typedef std::shared_ptr< CompositeSink > sptr;


                static sptr create( enum LOG_LEVEL lvl ){ return sptr( new CompositeSink( lvl ) ); }


                int log( enum LOG_LEVEL msg_lvl, const std::string& s);
                void add_sink( Sink::sptr sink ){ _sinks.push_back( sink ); }

            private:
                std::vector< Sink::sptr > _sinks;

                CompositeSink(enum LOG_LEVEL log_lvl) : Sink(log_lvl){ ;}
        };


        class SinkFormatter : public ISinkFormatter
        {
            public:
                typedef std::shared_ptr< SinkFormatter > sptr;

                static sptr create(){ return sptr( new SinkFormatter() ); }

                ~SinkFormatter()
                {
                }

                std::string header( enum LOG_LEVEL msg_lvl, Logger* logger );

            private:
                SinkFormatter(){}
        };

        class Logger
        {
            public:
                typedef std::shared_ptr< Logger > sptr;
                class MsgBuilder
                {
                    public:
                        friend class Logger;
                        typedef std::shared_ptr< MsgBuilder > sptr;

                        ~MsgBuilder();

                        template< typename T >
                            MsgBuilder& operator<<( const T& s)
                            {
                                if( _logger->sink()->should_log( _msg_lvl ) )
                                {
                                    _oss << s;
                                }
                                return *this;
                            }

                    private:
                        MsgBuilder( enum LOG_LEVEL msg_lvl, Logger* logger );
                        MsgBuilder( const MsgBuilder& rhs );

                        Logger* _logger;
                        enum LOG_LEVEL _msg_lvl;

                        std::ostringstream _oss;
                };

                static sptr create( enum LOG_LEVEL lvl, Sink::sptr sink, std::string name="" );

                MsgBuilder info();
                MsgBuilder debug();
                MsgBuilder trace();
                MsgBuilder warn();
                MsgBuilder critical();
                MsgBuilder error();

                Sink::sptr sink(){ return _sink; }
                const std::string& name() const { return _name;}

                void set_level( enum LOG_LEVEL lvl){ _log_lvl = lvl;}
                enum LOG_LEVEL get_level() const { return _log_lvl;}

            protected:
                Logger( enum LOG_LEVEL lvl, Sink::sptr sink, std::string name ) ;

            private:

                enum LOG_LEVEL _log_lvl;
                Sink::sptr _sink;
                std::string _name;
        };
    }

    class logger
    {
        public:
            typedef std::shared_ptr< logging::Logger > logger_sptr;

            static logger_sptr get_logger();
        private:
            static logger_sptr _instance;
    };

} // namespace FairDataPipeline



#endif
