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
#include <map>
#include "request_handler.h"
#include "config_parser.h"
#include "status.h"

using boost::asio::ip::tcp;
class NginxConfig;

class Session
{
public:
  typedef std::map<std::string, std::pair<std::string,NginxConfig>> loc_map_type;

  Session(boost::asio::io_service& io_service, 
          Status* status,
          bool test_flag = false,
          const loc_map_type& loc_map = loc_map_type());
  // on invalid req_string, returns false. Otherwise, req_string parsed and result put into passed-by-reference req
  bool FormatRequest(std::string req_string, http::request<http::string_body>& req);
  RequestHandler* createHandler(std::string location, std::string handler, NginxConfig config_child);
  http::response<http::string_body> url_dispatcher(http::request<http::string_body> req);
  http::response<http::string_body> BadRequest();
  int send_response(const boost::system::error_code& error, size_t bytes_transferred);
  int loopback_read(const boost::system::error_code& error, size_t bytes_transferred);
  void handle_read();
  void handle_write(std::string response_msg, std::string type);
  void start();

  tcp::socket socket_;


private:
  loc_map_type loc_map_;
  Status* status_;
  bool test_flag;
  enum { max_length = 1024 };
  char data_[max_length];
};

#endif
