#include <sstream>
#include <string>
#include "not_found_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

NotFoundHandler::NotFoundHandler(const std::string& location_path, const NginxConfig& config)
  : RequestHandler(location_path, config)
{}


//TODO: create and return a http::response object instead of a string
http::response<http::string_body> NotFoundHandler::handle_request(const http::request<http::string_body>& request)
{
  //try {
    /*
    std::string body = "This requested resource could not be found";
    std::stringstream response_msg;
    response_msg << "HTTP/1.1 404 Not Found\r\n";
    response_msg << "Content-Type: text/plain\r\n";
    response_msg << "Content-Length: " << body.length() << "\r\n";
    response_msg << "\r\n";
    response_msg << body << "\r\n";
*/
    //return response_msg.str();
    http::response<http::string_body> res(http::status::ok, 11);
    return res;
    

    
  /*} catch(int error) {
    return "";
  }*/
}
