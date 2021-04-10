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
#include "session.hpp"
#include "server.hpp"
#include <sstream>

using boost::asio::ip::tcp;

session::session(boost::asio::io_service& io_service)
    : socket_(io_service){};

  tcp::socket& session::socket()
  {
    return socket_;
  }

  void session::start()
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&session::handle_read, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

  void session::handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      // format response
      std::stringstream response;
      response << "HTTP/1.1 200 OK\r\n";
      response << "Content-Type: text/plain\r\n";
      response << "Content-Length: " << bytes_transferred;
      response << "\r\n\r\n";
      response << data_;
      // write http response to socket
      boost::asio::async_write(socket_,
          boost::asio::buffer(response.str()),
          boost::bind(&session::handle_write, this,
            boost::asio::placeholders::error));
    }
    else
    {
      delete this;
    }
  }

  void session::handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      socket_.async_read_some(boost::asio::buffer(data_, max_length),		   
          boost::bind(&session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      delete this;
    }
  }

