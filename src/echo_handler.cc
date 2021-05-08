#include <sstream>
#include <string>
#include "echo_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

EchoHandler::EchoHandler(const std::string& location_path, const NginxConfig& config)
  : RequestHandler(location_path, config)
{}

// TODO: create and return an http::response object instead of a string
http::response<http::dynamic_body> EchoHandler::handle_request(const http::request<http::string_body> request)
{
  //try {
    /*std::stringstream response_msg;
    response_msg << "HTTP/1.1 200 OK\r\n";
    response_msg << "Content-Type: text/plain\r\n";
    response_msg << "Content-Length: " << data_.length();
    response_msg << "\r\n\r\n";
    response_msg << data_;
*/
    //return response_msg.str();
    http::response<http::dynamic_body> res(http::status::ok, 11);
    return res;
  /*} catch(int error) {
    return "";
  }*/
}
