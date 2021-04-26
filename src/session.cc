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


session::session(boost::asio::io_service& io_service, bool test_flag, const loc_map_type& loc_map )
  : socket_(io_service), test_flag(test_flag), loc_map_(loc_map) {}


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
    // set up logging
  Logger* instance = Logger::getInstance();
  instance->log_data_read();
}

/* Callback function: on read then format and send response back if successful */
int session::send_response(const boost::system::error_code& error, size_t bytes_transferred)
{
  Logger* instance = Logger::getInstance();
  if (!error)
    {
      // old way of doing it
      std::string data_string(data_);
      // EchoResponse response_obj(data_string);
      // in new way, response_msg should be default initialized to 404 not found response
      // that way, if the for loop below didn't get any matches, we can just write with response_msg
      // std::string response_msg = response_obj.GetResponse();
      memset(data_, 0, 1024);

      // new way - shouldn't break above
      // THIS DOESN'T ACTUALLY DO ANYTHING YET - IT'S BOILERPLATE CODE
      NotFoundResponse res_404;
      std::string response_msg = res_404.GetResponse();
      Request req(data_string);
      req.ExtractPath();
      std::string req_path = req.GetPath();
      bool echo = false;
      
      for (std::map<std::string,std::string>::reverse_iterator iter = loc_map_.rbegin(); iter != loc_map_.rend(); iter++) {
        std::string loc = iter->first;
        std::string val = iter->second; // could be $echo for echoing or a path for static
        // std::cerr << loc << ' ' << val << "\n";
        // if req_path starts with loc - there's a match
        // what about /static and /static1, and client asks for /static1/test.txt? In some sense we want longest-matching
        int pos = req_path.find(loc);
        if (pos != 0) {
          // if loc wasn't at the start of req_path - not a match
          continue;
        }
        if (val == "$echo") {
          // Iniitalize an EchoRequest object, assign response_msg to GetResponse(), break
          EchoResponse echo_req(data_string);
          response_msg = echo_req.GetResponse();
          echo = true;
          break;
        }
        std::string file_path = req_path.substr(loc.length(), std::string::npos);
        // Initialize a StaticResponse object, assign response_msg to GetResponse(), break

        std::string fullpath = val + file_path;
        std::cerr << fullpath << std::endl;

        StaticResponse static_res(fullpath);
        response_msg = static_res.GetResponse();
	
        break;
      }

      // set up logging...
      if(echo)
        instance->log_data_write_echo(response_msg);
      else
        instance->log_data_write_static(response_msg);

      // write response to socket
      handle_write(response_msg);

      // success exit code
      return 0;
    }
  else
    {
      instance->log_session_end();
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
				       boost::asio::placeholders::error,
               boost::asio::placeholders::bytes_transferred));
}

/* Callback function: after successful write to socket, loop again waiting for message */
// https://www.boost.org/doc/libs/1_38_0/doc/html/boost_asio/reference/basic_stream_socket/async_read_some.html
// above link says the function handler has to include the size_t parameter
int session::loopback_read(const boost::system::error_code& error, size_t bytes_transferred)
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

