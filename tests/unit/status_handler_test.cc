#include "gtest/gtest.h"
#include "request_handler.h"
#include "status_handler.h"
#include "status.h"
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <cstdlib>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class StatusHandlerTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */

TEST_F(StatusHandlerTest, StatusResponseTest)
{
  std::map<std::string, std::pair<std::string, NginxConfig>> loc_map;

  // Initialize a status object with requests/handlers
  Status* status = new Status(loc_map);

  status->insert_request("/echo", 200);
  status->insert_request("/echo", 200);
  status->insert_request("/static/hello.html", 200);
  status->insert_request("/garbage", 404);

  status->insert_handler("EchoHandler", "/echo");
  status->insert_handler("StaticHandler", "/static");
  status->insert_handler("StatusHandler", "/status");

  // Initialize the status handler and get the response
  StatusHandler status_handler("some location", out_config, status);

  http::request<http::string_body> req;
  req.body() = "GET /status HTTP/1.1\r\n";
  
  http::response<http::string_body> res = status_handler.handle_request(req);

  // Response returned by the handler 
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;
  std::string response_gotten = osresponse_gotten.str();

  // Expected response formating
  std::string response_expect = "";
  response_expect += "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 455\r\n\r\n";
  response_expect += "Server Status Information...\n\nURL Requested Count: 4\n\nList of URLs and their Status Code...\n\n";
  response_expect += "| URL | - | Response Status Code | - | Number of requests |\n";
  response_expect += "/echo - 200 - 2 request(s) received\n";
  response_expect += "/garbage - 404 - 1 request(s) received\n";
  response_expect += "/static/hello.html - 200 - 1 request(s) received\n";
  response_expect += "\nExisting Request Handlers and their Prefixes...\n\n";
  response_expect += "EchoHandler - List of Prefixes: \n/echo\n\n";
  response_expect += "StaticHandler - List of Prefixes: \n";
  response_expect += "/static\n\nStatusHandler - List of Prefixes: \n/status\n\n";

  std::cerr << response_gotten << std::endl;

  // Fails for now: need to find exact expected response paying attention to the \n and \r\n
  EXPECT_TRUE(response_gotten == response_expect);
}


TEST_F(StatusHandlerTest, StatusNoResponseTest)
{
  std::map<std::string, std::pair<std::string, NginxConfig>> loc_map;

  // Initialize a status object with requests/handlers
  Status* status = new Status(loc_map);

  status->insert_handler("EchoHandler", "/echo");
  status->insert_handler("StaticHandler", "/static");
  status->insert_handler("StatusHandler", "/status");

  // Initialize the status handler and get the response
  StatusHandler status_handler("some location", out_config, status);

  http::request<http::string_body> req;
  req.body() = "GET /status HTTP/1.1\r\n";
  
  http::response<http::string_body> res = status_handler.handle_request(req);

  // Response returned by the handler 
  std::ostringstream osresponse_gotten;
  osresponse_gotten << res;
  std::string response_gotten = osresponse_gotten.str();

  // Expected response formating
  std::string response_expect = "";
  response_expect += "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 297\r\n\r\n";
  response_expect += "Server Status Information...\n\nURL Requested Count: 0\n\nList of URLs and their Status Code...\n\n";
  response_expect += "No requests received yet.\n";
  response_expect += "\nExisting Request Handlers and their Prefixes...\n\n";
  response_expect += "EchoHandler - List of Prefixes: \n/echo\n\n";
  response_expect += "StaticHandler - List of Prefixes: \n";
  response_expect += "/static\n\nStatusHandler - List of Prefixes: \n/status\n\n";

  std::cerr << response_gotten << std::endl;

  // Fails for now: need to find exact expected response paying attention to the \n and \r\n
  EXPECT_TRUE(response_gotten == response_expect);
}
