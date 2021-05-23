#include <sstream>
#include <string>
#include "request_handler.h"
#include "health_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

HealthHandler::HealthHandler(const std::string& location_path, const NginxConfig& config)
  : RequestHandler(location_path, config)
{}

// TODO: create and return an http::response object instead of a string
http::response<http::string_body> HealthHandler::handle_request(const http::request<http::string_body>& request)
{
  // Get version from the request
  size_t version = request.version();

  std::string body = "OK\n";
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
