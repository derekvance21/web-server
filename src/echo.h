#ifndef ECHO_H
#define ECHO_H

#include <string>
#include <boost/asio.hpp>
#include "response.h"

class EchoResponse : public Response
{
  public:
    EchoResponse(const std::string& data);
    std::string GetResponse();

  private:
    std::string data_;
};

#endif
