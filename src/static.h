#ifndef STATIC_H
#define STATIC_H

#include <string>
#include <boost/asio.hpp>
#include "response.h"

class StaticResponse : public Response
{
  public:
    StaticResponse(const std::string& resource_path, const std::string& static_path);
    std::string GetResponse();

  private:
    std::string resource_path_; // what the client requested
    std::string static_path_; // where to look for that resource
};

#endif
