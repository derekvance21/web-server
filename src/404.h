#ifndef ECHO_H
#define NOT_FOUND_H

#include <string>
#include "response.h"

class NotFoundResponse : public Response
{
  public:
    NotFoundResponse();
    std::string GetResponse();
};

#endif
