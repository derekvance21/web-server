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
};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */

TEST_F(LoginHandlerTest, StatusCodeResponseTest)
{
  // Set-Up Response Message To Be Sent
  std::string temp = "/login";
  LoginHandler req_handler(temp, out_config);

  http::request<http::string_body> req;
  req.method(http::verb::post);
  req.body() = "password=wrong_password";
  
  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);

  // Both Should Be Equal
  EXPECT_FALSE(res.result_int() == 200);

  req.body() = "password=juzang-password";
  res = req_handler.handle_request(req);

  EXPECT_TRUE(res.result_int() == 200);
}


