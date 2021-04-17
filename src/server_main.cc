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
#include "session.h"
#include "config_parser.h"
#include <string>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
     std::cerr << "Usage: webserver <path to config file>\n"; 
     return 1;
    }
    
    NginxConfigParser config_parser;
    NginxConfig config;
    
    if(!config_parser.Parse(argv[1], &config)) {
      std::cerr << "## Error ##: Invalid Config File.\n";
      return 1;
    }
    
    std::string config_string = config.ToString();
    
    // call GetPort function to parse config file and retrieve port number 
    int port_num = config.GetPort();
    if(port_num == -1)
      return 1;
    
    boost::asio::io_service io_service;
    server s(io_service, port_num);

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
