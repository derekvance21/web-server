#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "response.h"

response::response(const std::string& data)
  : data(data)
{}

std::string response::GetResponse()
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
