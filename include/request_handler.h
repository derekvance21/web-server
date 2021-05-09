#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <string>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include "config_parser.h"

namespace http = boost::beast::http;

class RequestHandler
{
  public:
    RequestHandler(const std::string& location_path, const NginxConfig& config)
      : location_path(location_path), config(config) {}

    virtual http::response<http::string_body> handle_request(const http::request<http::string_body>& request) = 0;
    std::string GetBody(const http::request<http::string_body>& request);

  protected:
    http::request<http::string_body> request; // the request sent to the server
    std::string location_path;                // the location path listed in the config file
    NginxConfig config;                       // the post-parsed, block-scoped Nginx config object containing the request handler's arguments
};

#endif
