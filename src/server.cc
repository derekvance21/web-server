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
#include "logger.h"

using boost::asio::ip::tcp;

// overloaded constructor that doesnt start_accept
Server::Server(boost::asio::io_service& io_service, NginxConfig& config, bool test_flag)
	: io_service_(io_service),
	  acceptor_(io_service, tcp::endpoint(tcp::v4(), config.GetPort())),
	  test_flag(test_flag),
    config_(config)
{
  loc_map_ = config.GetLocationMap();
}

int Server::start_accept()
{
  Session* new_session = new Session(io_service_, config_, false, loc_map_);
  acceptor_.async_accept(new_session->socket_,
			 boost::bind(&Server::handle_accept, this, new_session,
				     boost::asio::placeholders::error));
  return 0;
}

int Server::handle_accept(Session* new_session, const boost::system::error_code& error)
{
  if (!error) {
    new_session->start();
  }
  else {
    delete new_session;
    return -1;
  }

  // loop back to start_accept and wait for the next session
  if (!test_flag)
    start_accept();
  
  return 0;
}
