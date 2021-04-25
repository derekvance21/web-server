#include <sstream>
#include <string>
// #include "response.h"
#include "404.h"

std::string NotFoundResponse::GetResponse()
{
  try {
    std::string body = "This requested resource could not be found";
    std::stringstream response_msg;
    response_msg << "HTTP/1.1 404 Not Found\r\n";
    response_msg << "Content-Type: text/plain\r\n";
    response_msg << "Content-Length: " << body.length() << "\r\n";
    response_msg << "\r\n";
    response_msg << body << "\r\n";

    return response_msg.str();
    
  } catch(int error) {
    return "";
  }
}
