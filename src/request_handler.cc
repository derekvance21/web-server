#include <sstream>
#include <string>
#include "request_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;


/* Helper: Returns the body of a request as a string */
std::string RequestHandler::GetBody(const http::request<http::string_body>& request)
{
  std::ostringstream oss;
  oss << request.body();
  return oss.str();
}
