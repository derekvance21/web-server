#include "gtest/gtest.h"
#include "request.h"
#include "static_handler.h"
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

#include <string>
using boost::asio::ip::tcp;


class StaticHandlerTest : public ::testing::Test {
  protected:   
    http::request<http::string_body> request;
};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */
TEST_F(StaticHandlerTest, HTMLResponseTest)
{
  std::string path = "./static_folder/hello.html";

  StaticHandler static_handler(path);

  std::string response = "";
  response = static_handler.handle_request(request);

  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 21\r\n\r\nThis is Team Juzang!\n";

  EXPECT_TRUE(response == expected_response);
}


TEST_F(StaticHandlerTest, EmptyFileResponseTest)
{
  std::string path = "./static_folder/empty.html";

  StaticHandler static_handler(path);

  std::string response = "";
  response = static_handler.handle_request(request);

  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";

  EXPECT_TRUE(response == expected_response);
}


TEST_F(StaticHandlerTest, TxTResponseTest)
{
  std::string path = "./static_folder/file.txt";

  StaticHandler static_handler(path);

  std::string response = "";
  response = static_handler.handle_request(request);

  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 20\r\n\r\nThis is a txt file!\n";

  EXPECT_TRUE(response == expected_response);
}


TEST_F(StaticHandlerTest, NonExistantFileResponseTest)
{
  std::string path = "./static_folder/non-existant.txt";

  StaticHandler static_handler(path);

  std::string response = "";
  response = static_handler.handle_request(request);

  std::string expected_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 42\r\n\r\nThis requested resource could not be found\r\n";

  EXPECT_TRUE(response == expected_response);
}


TEST_F(StaticHandlerTest, JPEGResponseTest)
{
  std::string path = "./static_folder/test_jpeg.jpeg";

  StaticHandler static_handler(path);

  std::string response = "";
  response = static_handler.handle_request(request);

  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: 8123\r\n\r\n";

  EXPECT_TRUE(response.substr(0, expected_response.length()) == expected_response);
}


TEST_F(StaticHandlerTest, PNGResponseTest)
{
  std::string path = "./static_folder/download.png";

  StaticHandler static_handler(path);

  std::string response = "";
  response = static_handler.handle_request(request);

  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: 9205\r\n\r\n";

  EXPECT_TRUE(response.substr(0, expected_response.length()) == expected_response);
}


TEST_F(StaticHandlerTest, ZIPResponseTest)
{
  std::string path = "./static_folder/file.zip";

  StaticHandler static_handler(path);

  std::string response = "";
  response = static_handler.handle_request(request);

  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: application/zip\r\nContent-Length: 0\r\n\r\n";

  EXPECT_TRUE(response == expected_response);
}
