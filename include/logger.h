#ifndef LOGGER_H
#define LOGGER_H

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/signals2.hpp>
#include <string>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
using namespace logging::trivial;


class Logger
{
    private:
        Logger();
    public:
        static Logger* instance;
        static Logger* getInstance();
        void init();
        src::severity_logger< severity_level > slg;
        void log_startup(std::string log_message);
        void log_termination();
        void log_error(std::string log_message);
        void log_server_initialization(std::string log_message);
        void log_server_initialization_failure(std::string log_message);
        void log_data_read(std::string& log_message);
        void log_data_write(std::string log_message, std::string type);
        void log_info(std::string log_message);
        void log_session_end();
};

#endif
