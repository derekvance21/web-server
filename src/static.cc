#include <sstream>
#include <string>
#include "static.h"


StaticResponse::StaticResponse(const std::string& resource_path, const std::string& static_path)
  : resource_path_(resource_path), static_path_(static_path)
{}

std::string StaticResponse::GetResponse()
{
  try {
    std::stringstream response_msg;
    response_msg << "HTTP/1.1 200 OK\r\n";
    response_msg << "\r\n";
    // response_msg << "Content-Type: text/plain\r\n";
    // response_msg << "Content-Length: " << data_.length() << "\r\n";
    // response_msg << data_;

    return response_msg.str();
  } catch(int error) {
    return "";
  }
}
