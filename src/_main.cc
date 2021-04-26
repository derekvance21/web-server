//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "server.h"
#include "config_parser.h"
#include "logger.h"
#include <string>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  try
  {
    // set up logging
    Logger* instance = Logger::getInstance();

    if (argc != 2)
    {
      std::cerr << "Usage: webserver <path to config file>\n";
      instance->log_error("Usage: webserver <path to config file>\n");
      return 1;
    }
    
    NginxConfigParser config_parser;
    NginxConfig config;
    
    if(!config_parser.Parse(argv[1], &config)) {
      std::cerr << "## Error ##: Invalid Config File.\n";
      instance->log_error("## Error ##: Invalid Config File.\n");
      return 1;
    }
    
    boost::asio::io_service io_service;
    server s(io_service, config);
    s.start_accept();
    io_service.run();
    instance->log_termination();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
