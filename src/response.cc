#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "response.h"

using boost::asio::ip::tcp;

response::response()
{
  this->bytes_transferred = "1";
  this->data = "";
}

response::response(const std::string& bytes_transferred, const std::string& data)
{
  this->bytes_transferred = bytes_transferred;
  this->data = data;
}

int response::SetData(std::string data)
{
  try {
    // Set data and return 0 on successfully setting
    this->data = data;
    return 0;

  } catch(int error){
    return 1;
  }
  
}

int response::SetBytesTransferred(std::string bytes_transferred)
{
  try {
    // Set bytes_transferred and return 0 on successfully setting
    this->bytes_transferred = bytes_transferred;
    return 0;

  } catch(int error){
    return 1;
  }
}

std::string response::GetResponse()
{
  try {
    std::stringstream response_msg;
    response_msg << "HTTP/1.1 200 OK\r\n";
    response_msg << "Content-Type: text/plain\r\n";
    response_msg << "Content-Length: " << this->bytes_transferred;
    response_msg << "\r\n\r\n";
    response_msg << this->data;

    return response_msg.str();
    
  } catch(int error) {
    return "";
  }
}
