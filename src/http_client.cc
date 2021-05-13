#include "http_client.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "logger.h"

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>
namespace net = boost::asio;        // from <boost/asio.hpp>
using tcp = net::ip::tcp;           // from <boost/asio/ip/tcp.hpp>

HTTPClient::HTTPClient()
{
}

HTTPClient::~HTTPClient()
{
}

http::response<http::string_body> HTTPClient::make_request(const std::string& host, const std::string& port, const http::request<http::string_body>& req, boost::system::error_code& ec) 
{
  // Adapted from https://www.boost.org/doc/libs/1_70_0/libs/beast/doc/html/beast/quick_start/http_client.html
  
  // These objects perform our I/O
  tcp::resolver resolver(ioc_);
  beast::tcp_stream stream(ioc_);
  try
  {
      // Look up the domain name
      auto const results = resolver.resolve(host, port, ec);
      if(ec) {
        throw std::invalid_argument("Unable to look up domain name");
      }

      // Make the connection on the IP address we get from a lookup
      stream.connect(results, ec);
      if(ec) {
        throw std::runtime_error("Unable to connect to remote host: " + host);
      }

      

      // Send the HTTP request to the remote host
      http::write(stream, req, ec);
      if(ec) {
        throw std::runtime_error("Unable to send HTTP request to remote host: " + host);
      }

      // This buffer is used for reading and must be persisted
      beast::flat_buffer buffer;

      // Receive the HTTP response
      http::response<http::string_body> res;
      http::read(stream, buffer, res, ec);
      if(ec) {
        throw std::runtime_error("Unable to receive HTTP response from remote host: " + host);
      }

      // Gracefully close the socket
      boost::system::error_code shutdown_ec;
      stream.socket().shutdown(tcp::socket::shutdown_both, shutdown_ec);
      if(shutdown_ec && shutdown_ec != beast::errc::not_connected){
          ec = shutdown_ec;
          throw std::runtime_error("Unable to close socket for proxy request");
      }

      // If we get here then the connection is closed gracefully
      return res;
  }
  catch(std::exception const& e)
  {
    boost::system::error_code shutdown_ec;
    stream.socket().shutdown(tcp::socket::shutdown_both, shutdown_ec);
    if(shutdown_ec && shutdown_ec != beast::errc::not_connected){
        ec = shutdown_ec;
    }
    Logger::getInstance()->log_error("Unable to make reverse proxy request");
    Logger::getInstance()->log_error(e.what());
    http::response<http::string_body> res;
    res.result(http::status::internal_server_error);
    return res;
  }
}