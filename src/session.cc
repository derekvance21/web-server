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
#include "session.h"
#include "server.h"
#include "response.h"
#include <sstream>

using boost::asio::ip::tcp;

session::session(boost::asio::io_service& io_service)
  : socket_(io_service){};

tcp::socket& session::socket()
{
  return socket_;
}

/* Main function: starts off the reading from client */
void session::start()
{
  handle_read();
}

/* Reads into data_ when message sent */
void session::handle_read()
{
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
			  boost::bind(&session::send_response, this,
				      boost::asio::placeholders::error,
				      boost::asio::placeholders::bytes_transferred));
}

/* Callback function: on read then format and send response back if successful */
int session::send_response(const boost::system::error_code& error,
			  size_t bytes_transferred)
{
  if (!error)
    {
      // format response
      response response_obj(std::to_string(bytes_transferred),
			    std::string(data_));
      std::string response_msg = response_obj.GetResponse();
      memset(data_, 0, 1024);
      
      // write response to socket
      handle_write(response_msg);

      // success exit code
      return 0;
    }
  else
    {
      delete this;
      return 1;
    }
}

/* Writes response_msg back to socket (called on successfull read) */
void session::handle_write(std::string response_msg)
{
  // write response to socket
  boost::asio::async_write(socket_,
			   boost::asio::buffer(response_msg),
			   boost::bind(&session::loopback_read, this,
				       boost::asio::placeholders::error));
}

/* Callback function: after successful write to socket, loop again waiting for message */
void session::loopback_read(const boost::system::error_code& error)
{
  if (!error)
    {
      //handle_read();
    }
  else
    {
      delete this;
    }
}

