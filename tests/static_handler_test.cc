#include "gtest/gtest.h"
#include "request.h"
#include "static_handler.h"
#include <boost/asio.hpp>

#include <string>
using boost::asio::ip::tcp;


class StaticHandlerTest : public ::testing::Test {
  protected:   

};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */
TEST_F(StaticHandlerTest, BasicResponseTest)
{
  std::string data = "GET /static/hello.html HTTP/1.1\r\n\r\n";
  
  //Request request(data);
  
  //int code = request.ParseRequest();

  std::string path = "./static_folder/hello.html"; //request.GetPath();

  StaticResponse static_handler(path);

  std::string response = "";
  response = static_handler.GetResponse();

  std::cerr << response << std::endl;

  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 21\r\n\r\nThis is Team Juzang!\n";

  std::cerr << expected_response << std::endl;

  EXPECT_TRUE(response == expected_response);
}
