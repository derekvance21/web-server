#include <sstream>
#include <string>
#include "response.h"

std::string NotFoundResponse::GetResponse()
{
  try {
    std::stringstream response_msg;
    response_msg << "HTTP/1.1 404 Not Found\r\n";
    response_msg << "\r\n\r\n";

    return response_msg.str();
  } catch(int error) {
    return "";
  }
}
