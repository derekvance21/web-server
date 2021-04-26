#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "logger.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>
#include <boost/signals2.hpp>

using boost::asio::ip::tcp;
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace attrs = boost::log::attributes;
using namespace logging::trivial;

Logger* Logger::instance = 0;

Logger* Logger::getInstance(){
    if(Logger::instance == 0){
        Logger::instance = new Logger();
    }
    return Logger::instance;
}


Logger::Logger()
{
    init();
    logging::add_common_attributes();
}


void Logger::init()
{
    std::string log_format = "Thread [%ThreadID%] @ time [%TimeStamp%]: \n          [%Severity%]: %Message%";
    
    logging::add_file_log
    (
        keywords::file_name = "sample_%N.log",                                        /*< file name pattern >*/
        keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
        keywords::format = log_format, 
        keywords::auto_flush = true
    );

    logging::add_console_log(std::cout, boost::log::keywords::format = log_format);

    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );

}

void Logger::log_startup(std::string log_message){
    BOOST_LOG_SEV(slg, info) << "Server started, connecting to port " << log_message;
}

void Logger::log_error(std::string log_message){
    BOOST_LOG_SEV(slg, error) << "Error " << log_message;
}

void Logger::log_termination(){
    BOOST_LOG_SEV(slg, info) << "Server terminated\n";
}

void Logger::log_server_initialization(std::string port){
    BOOST_LOG_SEV(slg, info) << "Server initializaing, port: " << port << "\n";
}

void Logger::log_server_initialization_failure(std::string message){
    BOOST_LOG_SEV(slg, error) << "Server initializaing failure: " << message;
}

void Logger::log_data_read(){
    BOOST_LOG_SEV(slg, info) << "Reading data...\n";
}

void Logger::log_data_write_echo(std::string log_message){
     BOOST_LOG_SEV(slg, info) << "Writing data [ECHO]:\n" << log_message << "\n";
}

void Logger::log_data_write_static(std::string log_message){
     BOOST_LOG_SEV(slg, info) << "Writing data [STATIC]:\n" << log_message << "\n";
}

void Logger::log_session_end(){
     BOOST_LOG_SEV(slg, info) << "Session Terminating.\n";
}
