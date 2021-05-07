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

      //TODO: Add argument list to response
      virtual http::response<http::dynamic_body> handle_request(const http::request<http::string_body> request) = 0;

  protected:
    //TODO: Add argument list to request
    http::request<http::string_body> request;
    std::string location_path;
    NginxConfig config;
};

#endif
