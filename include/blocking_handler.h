#ifndef BLOCKINGHANDLER_H
#define BLOCKINGHANDLER_H

#include <string>
#include <boost/beast/http.hpp>
#include "request_handler.h"

namespace http = boost::beast::http;

class BlockingHandler : public RequestHandler
{
  public:
    BlockingHandler(const std::string& location_path, const NginxConfig& config);
    http::response<http::string_body> handle_request(const http::request<http::string_body>& request);
};

#endif
