#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <boost/asio.hpp>

class response
{
  public:
    response(const std::string& data);
    std::string GetResponse();

  private:
    std::string data;
};

#endif
