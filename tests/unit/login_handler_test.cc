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
  // create redirect response
  http::response<http::string_body> redirect_res;
  redirect_res.result(http::status::ok);
  redirect_res.set(http::field::content_type, "html/text");
  // Instantiate the request handler (Login Handler)
  std::string temp = "/login";
  LoginHandler req_handler(temp, out_config, cookies, redirect_res);

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
  // create redirect response
  http::response<http::string_body> redirect_res;
  redirect_res.result(http::status::ok);
  redirect_res.set(http::field::content_type, "html/text");
  // Instantiate the request handler (Login Handler)
  std::string temp = "/login";
  LoginHandler req_handler(temp, out_config, cookies, redirect_res);

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
  // create redirect response
  http::response<http::string_body> redirect_res;
  redirect_res.result(http::status::ok);
  redirect_res.set(http::field::content_type, "text/plain");
  // Instantiate the request handler (Login Handler)
  std::string temp = "/login";
  LoginHandler req_handler(temp, out_config, cookies, redirect_res);

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
  // create redirect response
  http::response<http::string_body> redirect_res;
  redirect_res.result(http::status::ok);
  redirect_res.set(http::field::content_type, "html/text");
  // Instantiate the request handler (Login Handler)
  std::string temp = "/login";
  LoginHandler req_handler(temp, out_config, cookies, redirect_res);

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


TEST_F(LoginHandlerTest, MaxCookieReachedTest)
{
  // create redirect reponse
  http::response<http::string_body> redirect_res;
  redirect_res.result(http::status::ok);
  redirect_res.set(http::field::content_type, "html/text");
  // Instantiate the request handler (Login Handler)
  std::string temp = "/login";
  LoginHandler req_handler(temp, out_config, cookies, redirect_res);

  bool is_valid = false;
  
  // Fill in the queue with cookies until max number
  for (int i = 0; i < 15; i++){
    std::string cookie = "dummy-cookie-" + std::to_string(i+1);
    is_valid = req_handler.set_cookie(cookie);
  }

  // add in one past the limit
  is_valid = req_handler.set_cookie("real-cookie");

  // Get the cookies stored and check the front and back cookies
  auto handler_cookies = req_handler.get_cookies();
  std::string front = handler_cookies.front();
  std::string back = handler_cookies.back();

  std::cerr << front << " " << back << std::endl; 

  EXPECT_TRUE(front == "dummy-cookie-2");
  EXPECT_TRUE(back == "real-cookie");

  // Server will not respond to unauthenticated /echo request
  EXPECT_TRUE(is_valid == true);
}
