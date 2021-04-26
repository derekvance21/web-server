#ifndef STATIC_H
#define STATIC_H

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "response.h"
#include "request.h"

class StaticResponse : public Response
{
  public:
    StaticResponse(const std::string& data);
    std::string GetResponse();
    std::string FormatResponse(std::string content_type,
			       std::string content_length,
			       std::string file_content);
  
    std::string GetContentType(std::string extension);
    std::string ReadFile(std::string fullpath);

  private:
    std::string fullpath_;
};

#endif
