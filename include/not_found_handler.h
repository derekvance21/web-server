#ifndef NOTFOUNDHANDLER_H
#define NOTFOUNDHANDLER_H

#include <string>
#include <boost/beast/http.hpp>
#include "request_handler.h"

namespace http = boost::beast::http;

class NotFoundHandler : public RequestHandler
{
  public:
    NotFoundHandler(const std::string& location_path, const NginxConfig& config);
    http::response<http::dynamic_body> handle_request(const http::request<http::string_body> request);
};

#endif
