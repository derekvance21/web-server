#include "gtest/gtest.h"
#include "../src/server.h"
#include "../src/session.h"
#include <boost/asio.hpp>
#include <sys/socket.h>

#include <string>
using boost::asio::ip::tcp;

int LOCALPORT = 8080;
//const boost::system::error_code SUCCESS = boost::system::error_code::success;

class ServerGeneratorTest : public ::testing::Test {
  protected:   
    boost::asio::io_service io_service;

};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */

TEST_F(ServerGeneratorTest, HandleAccept)
{
  server s(io_service, LOCALPORT);
  // start_accept() always returns 0
  // so really this is just checking that the initialization doesn't throw an exception
  EXPECT_TRUE(s.start_accept() == 0);
}

/*
TEST_F(ServerGeneratorTest, HandleAcceptError)
{
  // Set up server
  //boost::asio::io_service io_service;
  boost::asio::io_service io_service;
  boost::asio::ip::tcp::socket socket(io_service);
  boost::asio::ip::tcp::endpoint endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 8080);
  socket.connect(endpoint);
  server s(io_service, LOCALPORT);
  io_service.run();
  session* new_session = new session(s.io_service_);
  //check for error in handle_accept
  int error_code = s.handle_accept(new_session, make_error_code(boost::system::errc::not_connected));
  EXPECT_TRUE(error_code == -1);
}*/




TEST_F(ServerGeneratorTest, HandleAcceptErrorTest)
{
  session* my_session = new session(io_service);
  
  server s(io_service, LOCALPORT, true);
  
  int error_code = s.handle_accept(my_session, make_error_code(boost::system::errc::not_connected));
  EXPECT_TRUE(error_code == -1);
}


TEST_F(ServerGeneratorTest, HandleAcceptSuccessTest)
{
  session* my_session = new session(io_service);

  server s(io_service, LOCALPORT, true);
  io_service.run();
  
  int error_code = s.handle_accept(my_session, make_error_code(boost::system::errc::success));
  EXPECT_TRUE(error_code == 0);
}
