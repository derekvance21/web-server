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
  proxy_request.set(http::field::user_agent, "teamjuzang/1.0");
  proxy_request.set(http::field::accept, "*/*");


  Logger::getInstance()->log_info("Making proxy request to " + host_ + " with target " + proxy_target + " at port " + port_);
  std::ostringstream stream;
  stream << proxy_request;
  std::string proxy_request_string = stream.str();
  Logger::getInstance()->log_data_write(proxy_request_string, "Proxy request");
  boost::system::error_code ec;
  http::response<http::string_body> res = client_->make_request(host_, port_, proxy_request, ec);

/*Redirect status codes:
  301: Moved Permanently
  302: Found
  303: See Other
  307: Temporary Redirect
  308: Permanent Redirect */
  
  const int REDIRECTION_CODES[5] = {301, 302, 303, 307, 308};
  const unsigned short ALLOWED_REDIRECTS = 10; 
  unsigned short current_redirects = 1; 

  while ( std::find(std::begin(REDIRECTION_CODES), std::end(REDIRECTION_CODES), res.result_int()) != std::end(REDIRECTION_CODES) && current_redirects < ALLOWED_REDIRECTS)
  {

    current_redirects++;
    std::string new_destination = std::string(res.base().at("Location"));

    //If redirect location starts with HTTPS, server cannot handle it
    if (new_destination.rfind("https", 0) == 0) 
    {
      Logger::getInstance()->log_info("Unable to redirect to HTTPS page");
      return internal_server_error();
    }

    parse_url(new_destination, host_, endpoint_);
    if (endpoint_ == "") endpoint_ = "/";

    proxy_request.target(endpoint_);
    proxy_request.set(http::field::host, host_);

    res = client_->make_request(host_, port_, proxy_request, ec);
    if (ec) {
      Logger::getInstance()->log_error("Unable to make redirected reverse proxy request: " + ec.message());
      return internal_server_error();
    }

  }

  if (current_redirects == ALLOWED_REDIRECTS)
  {
      Logger::getInstance()->log_error("Exceeded allowed limit of redirects");
      return internal_server_error();
  }

  // not_connected happens sometimes
  // so don't bother reporting it.
  if (ec) {
    Logger::getInstance()->log_error("Unable to make reverse proxy request: " + ec.message());
    return internal_server_error();
  }

  return res;
}