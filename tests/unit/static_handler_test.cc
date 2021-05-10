#include "gtest/gtest.h"
#include "static_handler.h"
#include "config_parser.h"
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <sstream>

namespace http = boost::beast::http;

#include <string>
using boost::asio::ip::tcp;


class StaticHandlerTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
    http::request<http::string_body> request;
};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */
TEST_F(StaticHandlerTest, HTMLResponseTest)
{
  parser.Parse("example_config/config_file", &out_config);
    
  // Set-Up Response Message To Be Sent
  std::string location_path = "/static";
  StaticHandler req_handler(location_path, out_config);

  // Generate base request
  http::request<http::string_body> req;
  req.target("/static/hello.html");

  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;

  std::string response_gotten = osresponse_gotten.str();
  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 21\r\n\r\nThis is Team Juzang!\n";
  

  EXPECT_TRUE(response_gotten == expected_response);
}

TEST_F(StaticHandlerTest, SingleQuotedConfigHTMLResponseTest)
{
  parser.Parse("example_config/config_file_single", &out_config);
    
  // Set-Up Response Message To Be Sent
  std::string location_path = "/static";
  StaticHandler req_handler(location_path, out_config);

  // Generate base request
  http::request<http::string_body> req;
  req.target("/static/hello.html");

  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;

  std::string response_gotten = osresponse_gotten.str();
  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 21\r\n\r\nThis is Team Juzang!\n";
  

  EXPECT_TRUE(response_gotten == expected_response);
}


TEST_F(StaticHandlerTest, EmptyFileResponseTest)
{
  parser.Parse("example_config/config_file", &out_config);
  
  // Set-Up Response Message To Be Sent
  std::string location_path = "/static";
  StaticHandler req_handler(location_path, out_config);

  // Generate base request
  http::request<http::string_body> req;
  req.target("/static/empty.html");

  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;

  std::string response_gotten = osresponse_gotten.str();
  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 0\r\n\r\n";

  EXPECT_TRUE(response_gotten == expected_response);
}


TEST_F(StaticHandlerTest, TxTResponseTest)
{
  parser.Parse("example_config/config_file", &out_config);
  
  std::string path = "/static/file.txt";

  // Set-Up Response Message To Be Sent
  std::string location_path = "/static";
  StaticHandler req_handler(location_path, out_config);

  // Generate base request
  http::request<http::string_body> req;
  req.target(path);

  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;

  std::string response_gotten = osresponse_gotten.str();
  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 20\r\n\r\nThis is a txt file!\n";

  EXPECT_TRUE(response_gotten == expected_response);
}


TEST_F(StaticHandlerTest, NonExistantFileResponseTest)
{
  parser.Parse("example_config/config_file", &out_config);
  
  std::string path = "/static/non-existant.txt";

  // Set-Up Response Message To Be Sent
  std::string location_path = "/static";
  StaticHandler req_handler(location_path, out_config);

  // Generate base request
  http::request<http::string_body> req;
  req.target(path);

  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;

  std::string response_gotten = osresponse_gotten.str();
  std::string expected_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 42\r\n\r\nThis requested resource could not be found";

  EXPECT_TRUE(response_gotten == expected_response);
}


TEST_F(StaticHandlerTest, JPEGResponseTest)
{
  parser.Parse("example_config/config_file", &out_config);
  
  std::string path = "/static/test_jpeg.jpeg";

  // Set-Up Response Message To Be Sent
  std::string location_path = "/static";
  StaticHandler req_handler(location_path, out_config);

  // Generate base request
  http::request<http::string_body> req;
  req.target(path);

  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;

  std::string response_gotten = osresponse_gotten.str();
  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: image/jpeg\r\nContent-Length: 8123\r\n\r\n";

  EXPECT_TRUE(response_gotten.substr(0, expected_response.length()) == expected_response);
}


TEST_F(StaticHandlerTest, PNGResponseTest)
{
  parser.Parse("example_config/config_file", &out_config);
  
  std::string path = "/static/download.png";

  // Set-Up Response Message To Be Sent
  std::string location_path = "/static";
  StaticHandler req_handler(location_path, out_config);

  // Generate base request
  http::request<http::string_body> req;
  req.target(path);

  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;

  std::string response_gotten = osresponse_gotten.str();
  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: image/png\r\nContent-Length: 9205\r\n\r\n";

  EXPECT_TRUE(response_gotten.substr(0, expected_response.length()) == expected_response);
}


TEST_F(StaticHandlerTest, ZIPResponseTest)
{
  parser.Parse("example_config/config_file", &out_config);
  
  std::string path = "/static/file.zip";

  // Set-Up Response Message To Be Sent
  std::string location_path = "/static";
  StaticHandler req_handler(location_path, out_config);

  // Generate base request
  http::request<http::string_body> req;
  req.target(path);

  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;

  std::string response_gotten = osresponse_gotten.str();
  std::string expected_response = "HTTP/1.1 200 OK\r\nContent-Type: application/zip\r\nContent-Length: 0\r\n\r\n";

  EXPECT_TRUE(response_gotten == expected_response);
}

