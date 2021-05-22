#ifndef HEALTHHANDLER_H
#define HEALTHHANDLER_H

#include <string>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include "request_handler.h"

namespace http = boost::beast::http;

class HealthHandler : public RequestHandler
{
  public:
    HealthHandler(const std::string& location_path, const NginxConfig& config);
    http::response<http::string_body> handle_request(const http::request<http::string_body>& request);
    std::string GetPath(const http::request<http::string_body>& request);
};

#endif
