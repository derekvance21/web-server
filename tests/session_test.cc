#include "gtest/gtest.h"
#include "session.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>


class SessionFixtureTest : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */

TEST_F(SessionFixtureTest, BasicSessionTest){

  // Set up new session and run io_service
  session* my_session = new session(io_service);
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
  session* my_session = new session(io_service);
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
  session* my_session = new session(io_service);
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
  session* my_session = new session(io_service, true);
  io_service.run();


  // Get exit code from the send_response function and make sure it is valid (0)
  int code = my_session->loopback_read(make_error_code(boost::system::errc::not_connected), 0);

  EXPECT_TRUE(code == 1);
}


TEST_F(SessionFixtureTest, SuccessLoopBackSessionTest){

  // Set up new session and run io_service
  session* my_session = new session(io_service, true);
  io_service.run();


  // Get exit code from the send_response function and make sure it is valid (0)
  int code = my_session->loopback_read(make_error_code(boost::system::errc::success), 0);

  EXPECT_TRUE(code == 0);
}


TEST_F(SessionFixtureTest, ErrorResponseSessionTest){

  // Set up new session and run io_service
  session* my_session = new session(io_service);
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
