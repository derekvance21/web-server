#include <sstream>
#include <string>
#include "request_handler.h"
#include "not_found_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

NotFoundHandler::NotFoundHandler(const std::string& location_path, const NginxConfig& config)
  : RequestHandler(location_path, config)
{}


http::response<http::string_body> NotFoundHandler::handle_request(const http::request<http::string_body>& request)
{
  // Get version, body and data length from the request
  std::string body = "This requested resource could not be found";
  size_t content_length = body.length();
  size_t version = request.version();

  /* Generate the response */
  http::response<http::string_body> res;

  // Complete necessary fields
  res.version(version);
  res.result(http::status::not_found);
  res.set(http::field::content_type, "text/plain");
  res.set(http::field::content_length, content_length);
  res.body() = body;

  // Return properly formatted response
  return res;
    
}

/* Generates the path for the echo request requested */
std::string NotFoundHandler::GetPath(const http::request<http::string_body>& request){
  std::ostringstream path_stream;
  path_stream << request.target();
  return path_stream.str();
}
