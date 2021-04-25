//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef _server_h_
#define _server_h_

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <map>
#include "session.h"

using boost::asio::ip::tcp;

class NginxConfig;

class server
{
public:
  server(boost::asio::io_service& io_service, NginxConfig& config, bool test_flag = false );
  int start_accept();
  int handle_accept(session* new_session, const boost::system::error_code& error);
  
private:
  // location map - key is the location set by config, value is the location to fetch those resources from
  // special case: if value is $echo, then use echoing response 
  std::map<std::string, std::string> loc_map_;
  boost::asio::io_service& io_service_;
  tcp::acceptor acceptor_;
  bool test_flag;
};

#endif
