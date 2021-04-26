#include "gtest/gtest.h"
#include "request.h"
#include "static.h"
#include <boost/asio.hpp>

#include <string>
using boost::asio::ip::tcp;


class StaticHandlerTest : public ::testing::Test {
  protected:   

};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */
TEST_F(StaticHandlerTest, HTMLResponseTest)
{
  std::string path = "./static_folder/hello.html";

  StaticResponse static_handler(path);

  std::string response = "";
  response = static_handler.GetResponse();

  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 21\r\n\r\nThis is Team Juzang!\n";

  EXPECT_TRUE(response == expected_response);
}


TEST_F(StaticHandlerTest, EmptyFileResponseTest)
{
  std::string path = "./static_folder/empty.html";

  StaticResponse static_handler(path);

  std::string response = "";
  response = static_handler.GetResponse();

  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";

  EXPECT_TRUE(response == expected_response);
}


TEST_F(StaticHandlerTest, TxTResponseTest)
{
  std::string path = "./static_folder/file.txt";

  StaticResponse static_handler(path);

  std::string response = "";
  response = static_handler.GetResponse();

  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 20\r\n\r\nThis is a txt file!\n";

  EXPECT_TRUE(response == expected_response);
}


TEST_F(StaticHandlerTest, NonExistantFileResponseTest)
{
  std::string path = "./static_folder/non-existant.txt";

  StaticResponse static_handler(path);

  std::string response = "";
  response = static_handler.GetResponse();

  std::string expected_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 42\r\n\r\nThis requested resource could not be found\n";

  EXPECT_TRUE(response == expected_response);
}
