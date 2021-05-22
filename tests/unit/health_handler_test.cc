#include "gtest/gtest.h"
#include "request_handler.h"
#include "health_handler.h"
#include <string>
#include <sstream>
#include <iostream>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class HealthHandlerTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */

TEST_F(HealthHandlerTest, StatusCodeResponseTest)
{
  // Set-Up Response Message To Be Sent
  std::string temp = "/health";
  HealthHandler req_handler(temp, out_config);

  http::request<http::string_body> req;
  
  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);

  // Both Should Be Equal
  EXPECT_TRUE(res.result_int() == 200);
}


TEST_F(HealthHandlerTest, BasicResponse)
{
  // Set-Up Response Message To Be Sent
  std::string temp = "/health";
  HealthHandler req_handler(temp, out_config);

  // Generate base request
  http::request<http::string_body> req;
  
  // Get Both Expected/Received Response
  http::response<http::string_body> res = req_handler.handle_request(req);
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;

  std::string response_gotten = osresponse_gotten.str();
  std::string response_expect = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 3\r\n\r\nOK\n";

  // Both Should Be Equal
  EXPECT_TRUE(response_gotten == response_expect);
}
