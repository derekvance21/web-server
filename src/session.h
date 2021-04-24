//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef _session_h_
#define _session_h_

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class session
{
public:
  session(boost::asio::io_service& io_service, bool test_flag = false);
  int send_response(const boost::system::error_code& error, size_t bytes_transferred);
  int loopback_read(const boost::system::error_code& error, size_t bytes_transferred);
  void handle_read();
  void handle_write(std::string response_msg);
  void start();
  tcp::socket socket_;


private:
  bool test_flag;
  enum { max_length = 1024 };
  char data_[max_length];
};

#endif
