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
#include <thread>
#include <deque>
#include "server.h"
#include "session.h"
#include "config_parser.h"
#include "logger.h"
#include "status.h"

using boost::asio::ip::tcp;

// overloaded constructor that doesnt start_accept
Server::Server(boost::asio::io_service& io_service, NginxConfig& config, bool test_flag)
	: io_service_(io_service),
	  acceptor_(io_service, tcp::endpoint(tcp::v4(), config.GetPort())),
	  test_flag(test_flag)
{
  loc_map_ = config.GetLocationMap();
  status_ = new Status(loc_map_);
  record_handlers();
}

/* Helper: records the handlers from location map into the status object */
/* Done only once at startup since the config file is unchanged after */
int Server::record_handlers()
{
  // Loop through the location map and get the location and name of handler and store it
  for (std::map<std::string, std::pair<std::string, NginxConfig>>::iterator it = loc_map_.begin(); it != loc_map_.end(); ++it) {

    try {
      std::string location = it->first;
      std::string handler = it->second.first;

      status_->insert_handler(handler, location);

    } catch (int error) {
      return 1;
    }
  }

  // Return 0 on success
  return 0;
}

int Server::start_accept()
{
  Session* session = new Session(io_service_, status_, cookies_, false, loc_map_);
  acceptor_.async_accept(session->socket_,
			 boost::bind(&Server::handle_accept, this, session,
				     boost::asio::placeholders::error));
  return 0;
}

int Server::handle_accept(Session* session, const boost::system::error_code& error)
{
  if (!error) {
    session->start();
  }
  else {
    delete session;
    return -1;
  }

  // loop back to start_accept and wait for the next session
  if (!test_flag)
    start_accept();
  
  return 0;
}

Status* Server::get_status() {
  return status_;
}
