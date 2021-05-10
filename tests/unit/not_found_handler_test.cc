#include "gtest/gtest.h"
#include "request_handler.h"
#include "not_found_handler.h"
#include <string>
#include <sstream>
#include <iostream>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;


class NotFoundHandlerTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */

TEST_F(NotFoundHandlerTest, StatusCodeResponseTest)
{
  // Set-Up Response Message To Be Sent
  std::string temp = "/badecho";
  NotFoundHandler req_handler(temp, out_config);

  http::request<http::string_body> req;
  
  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);

  // Both Should Be Equal
  EXPECT_TRUE(res.result_int() == 404);
}


TEST_F(NotFoundHandlerTest, BasicResponse)
{
  // Set-Up Response Message To Be Sent
  std::string temp = "/badecho";
  NotFoundHandler req_handler(temp, out_config);

  // Generate base request
  http::request<http::string_body> req;
  req.body() = "This is Team Juzang!";

  
  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;

  std::string response_gotten = osresponse_gotten.str();
  std::string response_expect = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\nContent-Length: 42\r\n\r\nThis requested resource could not be found";

  // Both Should Be Equal
  EXPECT_TRUE(response_gotten == response_expect);
}
