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
#include <sstream>
#include <map>
#include "session.h"
#include "response.h"
#include "echo.h"
#include "static.h"
#include "404.h"
#include "request.h"
#include "logger.h"

using boost::asio::ip::tcp;


Session::Session(boost::asio::io_service& io_service, bool test_flag, const loc_map_type& loc_map )
  : socket_(io_service), test_flag(test_flag), loc_map_(loc_map) {}


/* Main function: starts off the reading from client */
void Session::start()
{
  handle_read();
}

/* Reads into data_ when message sent */
void Session::handle_read()
{
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
			  boost::bind(&Session::send_response, this,
				      boost::asio::placeholders::error,
				      boost::asio::placeholders::bytes_transferred));
    // set up logging
}

/* Callback function: on read then format and send response back if successful */
int Session::send_response(const boost::system::error_code& error, size_t bytes_transferred)
{
  if (!error)
    {
      std::string req_string(data_);
      Logger::getInstance()->log_data_read(req_string);
      memset(data_, 0, 1024);

      NotFoundResponse res_404;
      std::string response_msg = res_404.GetResponse();
      Request req(req_string);
      req.ExtractPath();
      std::string req_path = req.GetPath();
      std::string handle_type = "404";
      
      for (std::map<std::string,std::string>::reverse_iterator iter = loc_map_.rbegin(); iter != loc_map_.rend(); iter++) {
        std::string loc = iter->first;
        std::string route = iter->second; // could be $echo for echoing or a path for static
        // if req_path starts with loc - there's a match
        int pos = req_path.find(loc);
        if (pos != 0) {
          // if loc wasn't at the start of req_path - not a match
          continue;
        } else if (route == "$echo") {
          // Iniitalize an EchoRequest object, assign response_msg to GetResponse(), break
          EchoResponse res_echo(req_string);
          response_msg = res_echo.GetResponse();
          handle_type = "ECHO";
          break;
        } else {
          // Initialize a StaticResponse object, assign response_msg to GetResponse(), break
          std::string file_path = req_path.substr(loc.length(), std::string::npos);
          std::string fullpath = route + file_path;
          StaticResponse res_static(fullpath);
          response_msg = res_static.GetResponse();
          handle_type = "STATIC";
          break;
        }
      }

      // write response to socket
      handle_write(response_msg, handle_type);

      // success exit code
      return 0;
    }
  else
    {
      Logger::getInstance()->log_session_end();
      delete this;
      return 1;
    }
}

/* Writes response_msg back to socket (called on successfull read) */
void Session::handle_write(std::string response_msg, std::string type)
{
  Logger::getInstance()->log_data_write(response_msg, type);
  // write response to socket
  boost::asio::async_write(socket_,
			   boost::asio::buffer(response_msg),
			   boost::bind(&Session::loopback_read, this,
				       boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred));
}

/* Callback function: after successful write to socket, loop again waiting for message */
// https://www.boost.org/doc/libs/1_38_0/doc/html/boost_asio/reference/basic_stream_socket/async_read_some.html
// above link says the function handler has to include the size_t parameter
int Session::loopback_read(const boost::system::error_code& error, size_t bytes_transferred)
{
  if (!error)
    {
      if (!test_flag) 
        handle_read();
      return 0;
    }
  else
    {
      delete this;
      return 1;
    }
}

