#ifndef STATUS_H
#define STATUS_H

#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <map>
#include <cstdlib>
#include "config_parser.h"

namespace http = boost::beast::http;
using boost::asio::ip::tcp;

// A status object stores information about the server's configured 
// request handlers and the incoming requests
class Status 
{
    public:
        Status(std::map<std::string, std::pair<std::string, NginxConfig>> loc_map);
        std::map<std::string, std::vector<int>> get_requests();
        std::map<std::string, std::vector<std::string>> get_request_handlers();
        void insert_request(std::string, int);
        void insert_handler(std::string, std::string);

    private:
        std::map<std::string, std::pair<std::string, NginxConfig>> loc_map_;
        std::map<std::string, std::vector<int>> requests_; // map of url and response code
        std::map<std::string, std::vector<std::string>> request_handlers_;   // map of handler and url 
};

#endif
