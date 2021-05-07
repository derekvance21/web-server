#ifndef ECHOHANDLER_H
#define ECHOHANDLER_H

#include <string>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include "request_handler.h"

namespace http = boost::beast::http;

class EchoHandler : public RequestHandler
{
  public:
    EchoHandler(const string& location_path, const NginxConfig& config);
    http::response handle_request(const http::request<http::string_body> request);
    
};

#endif
