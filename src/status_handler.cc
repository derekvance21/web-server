#include <sstream>
#include <string>
#include "request_handler.h"
#include "status_handler.h"
#include "status.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

StatusHandler::StatusHandler(const std::string& location_path, const NginxConfig& config, Status* status)
  : RequestHandler(location_path, config), status_(status)
{}

// TODO: return status of webserver
// call status_->get_requests() and status_->get_request_handlers()
// and iterate through the maps and format their contents as the 
// returned http::response body
http::response<http::string_body> StatusHandler::handle_request(const http::request<http::string_body>& request)
{
  http::response<http::string_body> res;
  return res;
}
