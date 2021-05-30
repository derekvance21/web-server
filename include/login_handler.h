#ifndef LOGINHANDLER_H
#define LOGINHANDLER_H

#include <string>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <deque>
#include "request_handler.h"

namespace http = boost::beast::http;

class LoginHandler : public RequestHandler
{
  public:
    LoginHandler(const std::string& location_path, const NginxConfig& config, std::deque<std::string>& cookies);
    http::response<http::string_body> handle_request(const http::request<http::string_body>& request);
    http::response<http::string_body> handle_get_request(const http::request<http::string_body>& request);
    http::response<http::string_body> handle_post_request(const http::request<http::string_body>& request);
    int ReadFile(std::string fullpath, std::string& file_content);
    std::string generate_cookie();
    bool set_cookie(std::string cookie);
  private:
    std::deque<std::string>& cookies_;
};

#endif