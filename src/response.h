#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>
#include <boost/asio.hpp>

class response
{
  public:
    response();
    response(const std::string& bytes_transferred, const std::string& data);
    int SetBytesTransferred(std::string bytes_transferred);
    int SetData(std::string data);
    std::string GetResponse();

  private:
    std::string bytes_transferred;
    enum { max_length = 1024 };
    std::string data;
};

#endif
