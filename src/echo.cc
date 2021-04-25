#include <sstream>
#include <string>
#include "echo.h"

EchoResponse::EchoResponse(const std::string& data)
  : data_(data)
{}

std::string EchoResponse::GetResponse()
{
  try {
    std::stringstream response_msg;
    response_msg << "HTTP/1.1 200 OK\r\n";
    response_msg << "Content-Type: text/plain\r\n";
    response_msg << "Content-Length: " << data_.length();
    response_msg << "\r\n\r\n";
    response_msg << data_;

    return response_msg.str();
  } catch(int error) {
    return "";
  }
}
