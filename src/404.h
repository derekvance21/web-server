#ifndef NOT_FOUND_H
#define NOT_FOUND_H

#include <string>
#include "response.h"

class NotFoundResponse : public Response
{
  public:
    std::string GetResponse();
};

#endif
