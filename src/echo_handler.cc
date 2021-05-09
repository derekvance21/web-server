#include <sstream>
#include <string>
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
  std::string body = GetBody(request);
  size_t content_length = body.length();
  size_t version = request.version();

  
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

/* Helper: Returns the body of a request as a string */
std::string EchoHandler::GetBody(const http::request<http::string_body>& request)
{
  std::ostringstream oss;
  oss << request.body();
  return oss.str();
}
