#include "gtest/gtest.h"
#include "server.h"
#include "session.h"
#include "config_parser.h"
#include "status.h"
#include <boost/asio.hpp>
#include <sys/socket.h>

#include <string>
using boost::asio::ip::tcp;


class ServerGeneratorTest : public ::testing::Test {
  protected:   
    ServerGeneratorTest() {
      NginxConfigParser config_parser;
      config_parser.Parse("example_config/config_port", &config);
    }
    boost::asio::io_service io_service;
    NginxConfig config; 
    std::deque<std::string> cookies;
};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */

TEST_F(ServerGeneratorTest, HandleAccept)
{
  Server s(io_service, config);
  // start_accept() always returns 0
  // so really this is just checking that the initialization doesn't throw an exception
  EXPECT_TRUE(s.start_accept() == 0);
}

TEST_F(ServerGeneratorTest, HandleAcceptErrorTest)
{
  Server s(io_service, config, true);
  Status* my_status = s.get_status();
  Session* my_session = new Session(io_service, my_status, cookies);
  
  int error_code = s.handle_accept(my_session, make_error_code(boost::system::errc::not_connected));
  EXPECT_TRUE(error_code == -1);
}


TEST_F(ServerGeneratorTest, HandleAcceptSuccessTest)
{
  Server s(io_service, config, true);
  Status* my_status = s.get_status();
  Session* my_session = new Session(io_service, my_status, cookies);

  io_service.run();
  
  int error_code = s.handle_accept(my_session, make_error_code(boost::system::errc::success));
  EXPECT_TRUE(error_code == 0);
}
