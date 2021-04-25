#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <boost/asio.hpp>

class Response
{
  public:
    virtual std::string GetResponse() = 0;

  private:
    std::string data;
};

#endif
