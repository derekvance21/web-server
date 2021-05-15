#ifndef STATUSHANDLER_H
#define STATUSHANDLER_H

#include <string>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include "request_handler.h"
#include "status.h"

namespace http = boost::beast::http;

class StatusHandler : public RequestHandler
{
  public:
    StatusHandler(const std::string& location_path, const NginxConfig& config, Status* status);
    http::response<http::string_body> handle_request(const http::request<http::string_body>& request);
    int get_urls_count(std::map<std::string, std::vector<int>> requests);

  private:
    Status* status_;
};

#endif
