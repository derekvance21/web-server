#include "gtest/gtest.h"
#include "request.h"
#include <boost/asio.hpp>
#include <sys/socket.h>

#include <string>
using boost::asio::ip::tcp;


class RequestTest : public ::testing::Test {
  protected:   

};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */
TEST_F(RequestTest, EmptyParsingTest)
{
  Request request("");
  
  int code = request.ParseRequest();

  std::string method = request.GetMethod();
  std::string path = request.GetPath();
  std::string version = request.GetVersion();
  std::string body = request.GetBody();

  EXPECT_TRUE(method == "");
  EXPECT_TRUE(path == "");
  EXPECT_TRUE(version == "");
  EXPECT_TRUE(body == "");
}

TEST_F(RequestTest, BasicParsingTest)
{
  Request request("GET /hello.html HTTP/1.1\r\n\r\n<html>Hello, World!</html>");
  
  int code = request.ParseRequest();
  
  std::string method = request.GetMethod();
  std::string path = request.GetPath();
  std::string version = request.GetVersion();
  std::string body = request.GetBody();

  EXPECT_TRUE(method == "GET");
  EXPECT_TRUE(path == "/hello.html");
  EXPECT_TRUE(version == "HTTP/1.1");
  EXPECT_TRUE(body == "<html>Hello, World!</html>");
}
