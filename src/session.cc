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
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include "session.h"
#include "request_handler.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "not_found_handler.h"
#include "request.h"
#include "logger.h"
#include "config_parser.h"

namespace http = boost::beast::http;

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


RequestHandler* createHandler(std::string location, std::string handler, NginxConfig config_child) {
  if (handler == "StaticHandler") {
    return new StaticHandler(location, config_child);
  }
  if (handler == "EchoHandler") {
    return new EchoHandler(location, config_child);
  }
  // Should a NotFoundHandler take any arguments?
  return new NotFoundHandler(location, config_child);
}


/* Callback function: on read then format and send response back if successful */
int Session::send_response(const boost::system::error_code& error, size_t bytes_transferred)
{
  if (!error)
    {
      std::string req_string(data_);
      Logger::getInstance()->log_data_read(req_string);
      memset(data_, 0, 1024);

      // Use http::request_parser to parse our request and put it into an http::request
      http::request_parser<http::string_body> req_parser;
      req_parser.eager(true);
      boost::system::error_code ec;
      req_parser.put(boost::asio::buffer(req_string),ec);
      if(ec){
        std::cout << "Request parsing error: " << ec.message() << std::endl;
      }
      http::request<http::string_body> req = req_parser.get();
      // Get target and convert to string
      std::ostringstream oss;
      oss << req.target();
      std::string req_path = oss.str();

      // incase our request path is not present in out location map, use this 404 handler configured to "/"
      NginxConfig config_child;
      std::string handler = "NotFoundHandler";
      RequestHandler* request_handler = createHandler("/", handler, config_child);

      // iterate over location map to make appropriate request handlers
      std::map<std::string, std::pair<std::string, NginxConfig>>::reverse_iterator iter;
      for(iter = loc_map_.rbegin(); iter != loc_map_.rend(); iter++) {
        std::string loc = iter->first;
        handler = iter->second.first; 
        NginxConfig child_block = iter->second.second;

        // if req_path starts with loc - there's a match
        int pos = req_path.find(loc);
        if (pos != 0) {
          // if loc wasn't at the start of req_path - not a match
          continue;
        } 
        
        // we have found a match, so change request_handler to new location/type
        request_handler = createHandler(loc, handler, child_block);
      }
      
      // pass http::request into handle_request, called on our request_handler
      http::response<http::string_body> res = request_handler->handle_request(req);
      std::ostringstream response_stream;
      response_stream << res.body();
      std::string response_string = oss.str();

      // write response to server
      handle_write(response_string, handler);

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

