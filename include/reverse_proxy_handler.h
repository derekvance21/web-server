#ifndef REVERSEPROXYHANDLER_H
#define REVERSEPROXYHANDLER_H

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include "request_handler.h"
#include "http_client.h"
namespace http = boost::beast::http;

class HTTPClient;

class ReverseProxyHandler : public RequestHandler
{
  public:
    ReverseProxyHandler(const std::string& location_path, const NginxConfig& config);
    ~ReverseProxyHandler();
    ReverseProxyHandler(const std::string& location_path, const NginxConfig& config, HTTPClient* client);
    http::response<http::string_body> handle_request(const http::request<http::string_body>& request);
  private:
    void read_config();
    void parse_url(std::string url, std::string &host, std::string &endpoint);
    http::response<http::string_body> internal_server_error();
    HTTPClient* client_;
    std::string host_; // www.washington.edu
    std::string endpoint_; // defaults to "" but would be "/static" if destination is "http://www.washington.edu/static"
    std::string port_;
};

#endif
