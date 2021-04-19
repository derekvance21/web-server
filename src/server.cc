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

using boost::asio::ip::tcp;

// overloaded constructor that doesnt start_accept
server::server(boost::asio::io_service& io_service, short port, bool start_accept)
	: io_service_(io_service),
	  acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
	  accept_error (0)
{}

server::server(boost::asio::io_service& io_service, short port)
  : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    accept_error (0)
{
  start_accept();
}

int server::get_error() {
  return accept_error;
}

int server::start_accept()
{
  session* new_session = new session(io_service_);
  acceptor_.async_accept(new_session->socket(),
			 boost::bind(&server::handle_accept, this, new_session,
				     boost::asio::placeholders::error));
  return 0;
}

int server::handle_accept(session* new_session, const boost::system::error_code& error)
{
  if (!error) {
    new_session->start();
  }
  else {
    accept_error = -1;
    delete new_session;
    return -1;
  }
  start_accept();
  return 0;
}
