#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <string>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;
namespace net = boost::asio;        // from <boost/asio.hpp>

class HTTPClient
{
  public:
    HTTPClient();
    virtual ~HTTPClient();
    virtual http::response<http::string_body> make_request(const std::string& host, const std::string& port, const http::request<http::string_body>& request, boost::system::error_code& ec);
  private:
    net::io_context ioc_;
};

#endif