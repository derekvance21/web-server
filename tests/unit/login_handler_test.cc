#include "gtest/gtest.h"
#include "request_handler.h"
#include "login_handler.h"
#include <string>
#include <sstream>
#include <iostream>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class LoginHandlerTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
    std::deque<std::string> cookies;
};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */

TEST_F(LoginHandlerTest, StatusCodeLoginResponseTest)
{
  // Instantiate the request handler (Login Handler)
  std::string temp = "/login";
  LoginHandler req_handler(temp, out_config, cookies);

  // Set the body with the correct password
  http::request<http::string_body> req;
  req.method(http::verb::post);
  req.body() = "password=juzang-password";
  
  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);

  // Both Should Be Equal
  EXPECT_TRUE(res.result_int() == 200);
}

TEST_F(LoginHandlerTest, StatusCodeDeniedResponseTest)
{
  // Instantiate the request handler (Login Handler)
  std::string temp = "/login";
  LoginHandler req_handler(temp, out_config, cookies);

  // Set the body with an incorrect password
  http::request<http::string_body> req;
  req.method(http::verb::post);
  req.body() = "password=wrong_password";
  
  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);

  // Both Should Be Equal
  EXPECT_TRUE(res.result_int() == 401);
}

TEST_F(LoginHandlerTest, StatusCodeGetEchoRequestTest)
{
  // Instantiate the request handler (Login Handler)
  std::string temp = "/login";
  LoginHandler req_handler(temp, out_config, cookies);

  // Send an echo get request 
  http::request<http::string_body> req;

  // Format the request
  std::string req_string = "GET /echo HTTP/1.1\r\n";
  http::request_parser<http::string_body> req_parser;
  boost::system::error_code ec;
  req_parser.eager(true);
  req_parser.put(boost::asio::buffer(req_string), ec);
  req = req_parser.get();

  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;

  std::string response_gotten = osresponse_gotten.str();
  std::string response_expect = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 22\r\n\r\nGET /echo HTTP/1.1\r\n\r\n";

  // Both Should Be Equal (login page successful)
  EXPECT_TRUE(res.result_int() == 200);

  // Server will not respond to unauthenticated /echo request
  EXPECT_TRUE(osresponse_gotten.str() != response_expect);
}

TEST_F(LoginHandlerTest, StatusCodeGetStaticRequestTest)
{
  // Instantiate the request handler (Login Handler)
  std::string temp = "/login";
  LoginHandler req_handler(temp, out_config, cookies);

  // Send an echo get request 
  http::request<http::string_body> req;

  // Format the request
  std::string req_string = "GET /static/hello.html HTTP/1.1\r\n";
  http::request_parser<http::string_body> req_parser;
  boost::system::error_code ec;
  req_parser.eager(true);
  req_parser.put(boost::asio::buffer(req_string), ec);
  req = req_parser.get();

  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;

  std::string response_gotten = osresponse_gotten.str();
  std::string response_expect = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 21\r\n\r\nThis is Team Juzang!\n";

  // Both Should Be Equal (login page successful)
  EXPECT_TRUE(res.result_int() == 200);

  // Server will not respond to unauthenticated /echo request
  EXPECT_TRUE(osresponse_gotten.str() != response_expect);
}
