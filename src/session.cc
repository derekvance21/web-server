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
#include "status_handler.h"
#include "health_handler.h"
#include "blocking_handler.h"
#include "reverse_proxy_handler.h"
#include "not_found_handler.h"
#include "logger.h"
#include "config_parser.h"

namespace http = boost::beast::http;

using boost::asio::ip::tcp;


Session::Session(boost::asio::io_service& io_service, Status* status, bool test_flag, const loc_map_type& loc_map )
  : socket_(io_service), test_flag(test_flag), loc_map_(loc_map), status_(status) {}


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

http::response<http::string_body> Session::url_dispatcher(http::request<http::string_body> req) {
  // get the target path from request
  std::string req_path = std::string(req.target());
  
  // Instantiate the request handler
  std::string loc = "", handler = "NotFoundHandler";
  NginxConfig child_block;
  RequestHandler* request_handler = createHandler(loc, handler, child_block);

  // iterate over location map to make appropriate request handlers
  std::map<std::string, std::pair<std::string, NginxConfig>>::reverse_iterator iter;
  for(iter = loc_map_.rbegin(); iter != loc_map_.rend(); iter++) {
    loc = iter->first;
    handler = iter->second.first; 
    child_block = iter->second.second;

    // if req_path starts with loc - there's a match
    int pos = req_path.find(loc);
    if (pos != 0) {
      // if loc wasn't at the start of req_path - not a match
      handler = "NotFoundHandler";
      continue;
    } 
  }
      
  // pass http::request into handle_request, called on our request_handler
  http::response<http::string_body> res = request_handler->handle_request(req);
  
  // Get the status code and url to be stored in status_ object
  int res_status = res.result_int();

  std::ostringstream oss;
  oss << req.target();
  std::string url = oss.str();

  // Formatted log string for metric extraction
  std::ostringstream log_stream;
  log_stream << "[RepsonseMetrics] CODE:" << std::to_string(res_status) << " PATH:" << req_path << " HANDLER:" << handler;
  Logger::getInstance()->log_info(log_stream.str());
  
  // Insert into request map for calls to /status
  status_->insert_request(url, res_status);
  
  return res;
}


RequestHandler* Session::createHandler(std::string location, std::string handler, NginxConfig config_child) {
  if (handler == "StaticHandler") {
    return new StaticHandler(location, config_child);
  }
  if (handler == "EchoHandler") {
    return new EchoHandler(location, config_child);
  }
  if (handler == "StatusHandler") {
    return new StatusHandler(location, config_child, status_);
  }
  if (handler == "ReverseProxyHandler") {
    return new ReverseProxyHandler(location, config_child);
  }
  if (handler == "HealthHandler") {
    return new HealthHandler(location, config_child);
  }
  if (handler == "BlockingHandler") {
    return new BlockingHandler(location, config_child);
  }

  return new NotFoundHandler(location, config_child);
}

/* Reformats from string request to http::request object
  on invalid req_string, returns false. Otherwise, req_string parsed and result put into passed-by-reference req */
bool Session::FormatRequest(std::string req_string, http::request<http::string_body>& req){
      
  // Use http::request_parser to parse our request and put it into an http::request
  http::request_parser<http::string_body> req_parser;
  req_parser.eager(true);
      
  boost::system::error_code ec;
  req_parser.put(boost::asio::buffer(req_string), ec);
      
  if(ec) {
    std::cerr << "Request parsing error: " << ec.message() << std::endl;
    return false;
  }

  req = req_parser.get();
  return true;
}

/* Callback function: on read then format and send response back if successful */
int Session::send_response(const boost::system::error_code& error, size_t bytes_transferred)
{
  if (!error)
    {
      std::string req_string(data_);
      Logger::getInstance()->log_data_read(req_string);
      memset(data_, 0, 1024);

      http::request<http::string_body> req;
      bool valid_request = FormatRequest(req_string, req);

      http::response<http::string_body> res = valid_request ? url_dispatcher(req) : BadRequest();

      std::ostringstream response_stream;
      response_stream << res;
      std::string response_string = response_stream.str();

      // write response to client
      handle_write(response_string, "string");

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

http::response<http::string_body> Session::BadRequest() {
  std::string body = "Bad HTTP request given\n";
  http::response<http::string_body> res;

  res.result(http::status::bad_request);
  res.set(http::field::content_type, "text/plain");
  res.set(http::field::content_length, body.length());
  res.body() = body;

  return res;
}