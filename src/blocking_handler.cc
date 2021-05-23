#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include "request_handler.h"
#include "blocking_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

BlockingHandler::BlockingHandler(const std::string& location_path, const NginxConfig& config)
  : RequestHandler(location_path, config)
{}


http::response<http::string_body> BlockingHandler::handle_request(const http::request<http::string_body>& request)
{
  // Sleep for 15 sec
  std::chrono::seconds duration(15);
  std::this_thread::sleep_for(duration);
  
  // Get version, body and data length from the request
  std::string body = "Testing Multithreading Properties.";
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
