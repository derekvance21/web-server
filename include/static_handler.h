#ifndef STATICHANDLER_H
#define STATICHANDLER_H

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include "request_handler.h"
#include "not_found_handler.h"

namespace http = boost::beast::http;

class StaticHandler : public RequestHandler
{
  public:
    StaticHandler(const std::string& location_path, const NginxConfig& config);
    http::response<http::string_body> handle_request(const http::request<http::string_body>& request);
    std::string FormatResponse(std::string content_type,
			       std::string content_length,
			       std::string file_content);
  
    std::string GetContentType(std::string extension);
    int ReadFile(std::string fullpath, std::string& file_content);
    
};

#endif
