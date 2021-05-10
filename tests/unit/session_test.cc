#include "gtest/gtest.h"
#include "session.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/core.hpp>
#include <sstream>


namespace http = boost::beast::http;

class SessionFixtureTest : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */

TEST_F(SessionFixtureTest, BasicSessionTest){

  // Set up new session and run io_service
  Session* my_session = new Session(io_service);
  io_service.run();

  // Data to be tested
  std::string data = "Hello server!";

  // This is misleading - this write isn't ever read by session - 
  // send_response will be using an empty string as data_ 
  // Write data to socket to simulate client sending message
  boost::asio::async_write(my_session->socket_,
			   boost::asio::buffer(data),
			   [my_session](const boost::system::error_code& e, std::size_t s)
			   { });

  // Get exit code from the send_response function and make sure it is valid (0)
  int code = my_session->send_response(boost::system::error_code(), 0);

  EXPECT_TRUE(code == 0);
}


TEST_F(SessionFixtureTest, EmptyMessageSessionTest){

  // Set up new session and run io_service
  Session* my_session = new Session(io_service);
  io_service.run();

  // Data to be tested
  std::string data = "";

  // Write data to socket to simulate client sending message
  boost::asio::async_write(my_session->socket_,
			   boost::asio::buffer(data),
			   [my_session](const boost::system::error_code& e, std::size_t s)
			   { });

  // Get exit code from the send_response function and make sure it is valid (0)
  int code = my_session->send_response(boost::system::error_code(), 0);

  EXPECT_TRUE(code == 0);
}


TEST_F(SessionFixtureTest, MoreThanMaxMessageSessionTest){

  // Set up new session and run io_service
  Session* my_session = new Session(io_service);
  io_service.run();

  // Data to be tested (more than 1024)
  std::string data(2000, 'U');
  

  // Write data to socket to simulate client sending message
  boost::asio::async_write(my_session->socket_,
			   boost::asio::buffer(data),
			   [my_session](const boost::system::error_code& e, std::size_t s)
			   { });

  // Get exit code from the send_response function and make sure it is valid (0)
  int code = my_session->send_response(boost::system::error_code(), 0);

  EXPECT_TRUE(code == 0);
}


TEST_F(SessionFixtureTest, ErrorLoopBackSessionTest){

  // Set up new session and run io_service
  Session* my_session = new Session(io_service, true);
  io_service.run();


  // Get exit code from the send_response function and make sure it is valid (0)
  int code = my_session->loopback_read(make_error_code(boost::system::errc::not_connected), 0);

  EXPECT_TRUE(code == 1);
}


TEST_F(SessionFixtureTest, SuccessLoopBackSessionTest){

  // Set up new session and run io_service
  Session* my_session = new Session(io_service, true);
  io_service.run();


  // Get exit code from the send_response function and make sure it is valid (0)
  int code = my_session->loopback_read(make_error_code(boost::system::errc::success), 0);

  EXPECT_TRUE(code == 0);
}


TEST_F(SessionFixtureTest, ErrorResponseSessionTest){

  // Set up new session and run io_service
  Session* my_session = new Session(io_service);
  io_service.run();

  // Data to be tested (more than 1024)
  std::string data(20, 'U');
  

  // Write data to socket to simulate client sending message
  boost::asio::async_write(my_session->socket_,
			   boost::asio::buffer(data),
			   [my_session](const boost::system::error_code& e, std::size_t s)
			   { });

  // Get exit code from the send_response function and make sure it is valid (0)
  int code = my_session->send_response(make_error_code(boost::system::errc::not_connected), 0);

  EXPECT_TRUE(code == 1);
}


TEST_F(SessionFixtureTest, RequestParsingTest){

  // Set up new session and run io_service
  Session* my_session = new Session(io_service);
  io_service.run();

  std::string req_str = "GET /echo HTTP/1.1\r\nHost: team\r\n\r\n";
  
  http::request<http::string_body> req = my_session->FormatRequest(req_str);

  std::ostringstream oss;
  oss << req.target();
  std::string req_path = oss.str();

  EXPECT_TRUE(req_path == "/echo");
}
