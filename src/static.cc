#include <sstream>
#include <string>
#include "response.h"

StaticResponse::StaticResponse(const std::string& resource_path, const std::string& static_path)
  : resource_path_(resource_path), static_path_(static_path)
{}

std::string StaticResponse::GetResponse()
{
  try {
    std::stringstream response_msg;
    response_msg << "HTTP/1.1 200 OK\r\n";
    response_msg << "Content-Type: text/plain\r\n";
    response_msg << "Content-Length: " << data.length();
    response_msg << "\r\n\r\n";
    response_msg << data;

    return response_msg.str();
  } catch(int error) {
    return "";
  }
}
