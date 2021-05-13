#include "request_handler.h"
#include "reverse_proxy_handler.h"
#include "http_client.h"
#include "logger.h"
#include <boost/beast/http.hpp>
#include <sstream>

namespace http = boost::beast::http;       // from <boost/beast/http.hpp>

ReverseProxyHandler::ReverseProxyHandler(const std::string& location_path, const NginxConfig& config)
  : RequestHandler(location_path, config)
{
  client_ = new HTTPClient();
  host_ = "";
  endpoint_ = "";
  read_config();
}

// Constructor that takes in an HTTPClient for dependency injection
ReverseProxyHandler::ReverseProxyHandler(
    const std::string& location_path, const NginxConfig& config, HTTPClient* client)
: RequestHandler(location_path, config), client_(client)
{
  host_ = "";
  endpoint_ = "";
  port_ = "";
  read_config();
}

ReverseProxyHandler::~ReverseProxyHandler() {
  delete client_;
}

void ReverseProxyHandler::read_config() {
  // Set the endpoint and host from the config
  // For example, if the destination is http://www.washington.edu/static
  // The endpoint will be /static and host will be www.washington.edu

  if (!config.statements_.empty() &&
      config.statements_[0]->tokens_.size() == 3 &&
      config.statements_[0]->tokens_[0] == "destination") {

    std::string destination = config.statements_[0]->tokens_[1];
    parse_url(destination, host_, endpoint_);
    port_ = config.statements_[0]->tokens_[2];
  }
}

void ReverseProxyHandler::parse_url(std::string url, std::string &host, std::string &endpoint)
{
  // Do not support https
  std::string https_string = "https://";
  if (url.substr(0, https_string.length()) == https_string) {
    Logger::getInstance()->log_error("Proxy handler cannot support HTTPS");
  }
  else {
    // Remove http:// if it is specified
    std::string http_string = "http://";
    if (url.substr(0, http_string.length()) == http_string) {
      url = url.substr(http_string.length());
    }

    // Remove trailing slashes
    while (url.back() == '/'){
      url.pop_back();
    }

    // Set host as string before first slash
    // Set endpoint as string after (and including) first slash
    int first_slash_position = url.find("/");
    if (first_slash_position != std::string::npos){
        host_ = url.substr(0, first_slash_position);
        endpoint = url.substr(first_slash_position);
    } else {
        host = url;
    }
  }
}

http::response<http::string_body> ReverseProxyHandler::internal_server_error()
{
  http::response<http::string_body> res;
  res.result(http::status::internal_server_error);
  res.body() = "Internal server error.";
  res.prepare_payload();
  return res;
}

http::response<http::string_body> ReverseProxyHandler::handle_request(const http::request<http::string_body>& request)
{
  if (host_ == "" || port_ == "") {
    Logger::getInstance()->log_error("Invalid host for proxy handler, check configuration");
    return internal_server_error();
  }

  std::string proxy_target = endpoint_ + std::string{request.target().substr(location_path.size())};
  if (proxy_target == "") proxy_target = "/";
  http::request<http::string_body> proxy_request{request};
  proxy_request.target(proxy_target);
  proxy_request.set(http::field::host, host_);

  Logger::getInstance()->log_info("Making proxy request to " + host_ + " with target " + proxy_target + " at port " + port_);
  std::ostringstream stream;
  stream << proxy_request;
  std::string proxy_request_string = stream.str();
  Logger::getInstance()->log_data_write(proxy_request_string, "Proxy request");
  boost::system::error_code ec;
  http::response<http::string_body> res = client_->make_request(host_, port_, proxy_request, ec);

  // not_connected happens sometimes
  // so don't bother reporting it.
  if (ec) {
    BOOST_LOG_TRIVIAL(error) << "Unable to make reverse proxy request: " << ec.message();
    return internal_server_error();
  }

  return res;
}