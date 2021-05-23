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
#include <thread>
#include <vector>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  try
  {
    // Instantiate the logger
    Logger* instance = Logger::getInstance();

    // Check for valid usage of webserver
    if (argc != 2)
    {
      std::cerr << "Usage: webserver <path to config file>\n";
      instance->log_error("Usage: webserver <path to config file>\n");
      return 1;
    }

    // Initialize config parser and nginx config objects
    NginxConfigParser config_parser;
    NginxConfig config;
    
    if(!config_parser.Parse(argv[1], &config)) {
      std::cerr << "## Error ##: Invalid Config File.\n";
      instance->log_error("## Error ##: Invalid Config File.\n");
      return 1;
    }

    // Instantiate our main server and configure multithreading to handle
    // multiple request in separate threads
    boost::asio::io_service io_service;
    Server s(io_service, config);
    s.start_accept();

    // Set up multithreading
    int max_threads = 16;
    std::vector<std::thread> threads;

    // Allocate max threads in vector without initialization
    threads.reserve(max_threads);

    // Spin up all the threads
    int threadID = 0;
    for (; threadID < max_threads; threadID++) {
      std::thread thread([&io_service]() { io_service.run(); });
      threads.push_back(std::move(thread));
    }

    // Join all threads when finished
    for (int i = 0; i < threadID; i++) {
      threads[i].join();
    }
    
    instance->log_termination();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
    return 1;
  }

  return 0;
}

