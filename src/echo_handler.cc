#include <sstream>
#include <string>
#include "request_handler.h"
#include "echo_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

EchoHandler::EchoHandler(const std::string& location_path, const NginxConfig& config)
  : RequestHandler(location_path, config)
{}

// TODO: create and return an http::response object instead of a string
http::response<http::string_body> EchoHandler::handle_request(const http::request<http::string_body>& request)
{
  // Get version, body and data length from the request
  size_t version = request.version();

  // Changed: instead of just body, echo back the whole request
  std::ostringstream body_stream;
  body_stream << request;
  std::string body = body_stream.str();
  
  size_t content_length = body.length();
  
  /* Generate the response */
  http::response<http::string_body> res;

  // Complete necessary fields
  res.version(version);
  res.result(http::status::ok);
  res.set(http::field::content_type, "text/plain");
  res.set(http::field::content_length, content_length);
  res.body() = body;

  // Return properly formatted response
  return res;
}
