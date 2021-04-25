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
}

/* Callback function: on read then format and send response back if successful */
int session::send_response(const boost::system::error_code& error, size_t bytes_transferred)
{
  if (!error)
    {
      // old way of doing it
      std::string data_string(data_);
      EchoResponse response_obj(data_string);
      // in new way, response_msg should be default initialized to 404 not found response
      // that way, if the for loop below didn't get any matches, we can just write with response_msg
      std::string response_msg = response_obj.GetResponse();
      memset(data_, 0, 1024);

      // new way - shouldn't break above
      // THIS DOESN'T ACTUALLY DO ANYTHING - IT'S BOILERPLATE CODE
      Request req(data_string);
      req.ExtractPath();
      std::string req_path = req.GetPath();
      for (std::map<std::string,std::string>::iterator iter = loc_map_.begin(); iter != loc_map_.end(); iter++) {
        std::string loc = iter->first;
        std::string val = iter->second; // could be $echo for echoing or a path for static
        // if req_path starts with loc - there's a match
        int pos = req_path.find(loc);
        if (pos != 0) {
          continue;
        }
        if (val == "$echo") {
          // Iniitalize an EchoReqeust object, assign response_msg to GetResponse(), break
          break;
        }
        std::string file_path = req_path.substr(loc.length(), std::string::npos);
        // Initialize a StaticResponse object, assign response_msg to GetResponse(), break
        break;
      }


      
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

