#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <boost/beast/http.hpp>
#include "reverse_proxy_handler.h"
#include "http_client.h"
#include "config_parser.h"

using ::testing::_;
using ::testing::Return;

class MockHTTPClient : public HTTPClient {
    public:
      MockHTTPClient(){}
      ~MockHTTPClient(){}
      MOCK_METHOD4(make_request,
                  http::response<http::string_body>(
                      const std::string&,
                      const std::string&,
                      const http::request<http::string_body>&,
                      boost::system::error_code&));
                    
};

class ReverseProxyHandlerTest : public ::testing::Test {
  protected:
    void SetUp() override {
      parser.Parse("example_config/config_proxy", &out_config);
      mock_client = new MockHTTPClient();
    }
    NginxConfigParser parser;
    NginxConfig out_config;
    http::request<http::string_body> request;
    MockHTTPClient* mock_client;
};

TEST_F(ReverseProxyHandlerTest, Basic)
{
  // Mock client just returns a 200 OK
  mock_client = new MockHTTPClient();
  http::response<http::string_body> proxy_res;
  proxy_res.result(http::status::ok);
  proxy_res.body() = "200 OK";
  EXPECT_CALL(*mock_client, make_request("34.105.8.173", "80", _, _))
        .Times(1).WillOnce(Return(proxy_res));

  std::string location_path = "/proxy";
  ReverseProxyHandler req_handler(location_path, out_config, mock_client);
  http::request<http::string_body> req;
  req.method(http::verb::get);
  req.target(location_path);

  http::response<http::string_body> res = req_handler.handle_request(req);

  EXPECT_EQ(http::status::ok, res.result());
  EXPECT_EQ(proxy_res.body(), res.body());
}

TEST_F(ReverseProxyHandlerTest, DoesNotSupportHTTPS)
{
  out_config.statements_[0]->tokens_[1] = "https://34.105.8.173";
  std::string location_path = "/proxy";
  ReverseProxyHandler req_handler(location_path, out_config, mock_client);
  http::request<http::string_body> req;
  req.method(http::verb::get);
  req.target("/proxy");

  http::response<http::string_body> res = req_handler.handle_request(req);

  EXPECT_EQ(http::status::internal_server_error, res.result());
}

TEST_F(ReverseProxyHandlerTest, TrailingSlash)
{
  out_config.statements_[0]->tokens_[1] = "34.105.8.173///";

  // Mock client just returns a 200 OK
  mock_client = new MockHTTPClient();
  http::response<http::string_body> proxy_res;
  proxy_res.result(http::status::ok);
  proxy_res.body() = "200 OK";
  EXPECT_CALL(*mock_client, make_request("34.105.8.173", "80", _, _))
        .Times(1).WillOnce(Return(proxy_res));

  std::string location_path = "/proxy";
  ReverseProxyHandler req_handler(location_path, out_config, mock_client);
  http::request<http::string_body> req;
  req.method(http::verb::get);
  req.target(location_path);

  http::response<http::string_body> res = req_handler.handle_request(req);

  EXPECT_EQ(http::status::ok, res.result());
  EXPECT_EQ(proxy_res.body(), res.body());
}

TEST_F(ReverseProxyHandlerTest, StartsWithHTTP)
{
  out_config.statements_[0]->tokens_[1] = "http://34.105.8.173";
  
  // Mock client just returns a 200 OK
  mock_client = new MockHTTPClient();
  http::response<http::string_body> proxy_res;
  proxy_res.result(http::status::ok);
  proxy_res.body() = "200 OK";
  EXPECT_CALL(*mock_client, make_request("34.105.8.173", "80", _, _))
        .Times(1).WillOnce(Return(proxy_res));

  std::string location_path = "/proxy";
  ReverseProxyHandler req_handler(location_path, out_config, mock_client);
  http::request<http::string_body> req;
  req.method(http::verb::get);
  req.target(location_path);

  http::response<http::string_body> res = req_handler.handle_request(req);

  EXPECT_EQ(http::status::ok, res.result());
  EXPECT_EQ(proxy_res.body(), res.body());
}

TEST_F(ReverseProxyHandlerTest, NonRootEndpoint)
{
  out_config.statements_[0]->tokens_[1] = "http://34.105.8.173/static";
  
  // Mock client just returns a 200 OK
  mock_client = new MockHTTPClient();
  http::response<http::string_body> proxy_res;
  proxy_res.result(http::status::ok);
  proxy_res.body() = "200 OK";
  EXPECT_CALL(*mock_client, make_request("34.105.8.173", "80", _, _))
        .Times(1).WillOnce(Return(proxy_res));

  std::string location_path = "/proxy";
  ReverseProxyHandler req_handler(location_path, out_config, mock_client);
  http::request<http::string_body> req;
  req.method(http::verb::get);
  req.target(location_path);

  http::response<http::string_body> res = req_handler.handle_request(req);

  EXPECT_EQ(http::status::ok, res.result());
  EXPECT_EQ(proxy_res.body(), res.body());
}

TEST_F(ReverseProxyHandlerTest, ConfigurePort)
{
  out_config.statements_[0]->tokens_[1] = "http://34.105.8.173/static";
  out_config.statements_[0]->tokens_[2] = "8080";
  
  // Mock client just returns a 200 OK
  mock_client = new MockHTTPClient();
  http::response<http::string_body> proxy_res;
  proxy_res.result(http::status::ok);
  proxy_res.body() = "200 OK";
  EXPECT_CALL(*mock_client, make_request("34.105.8.173", "8080", _, _))
        .Times(1).WillOnce(Return(proxy_res));

  std::string location_path = "/proxy";
  ReverseProxyHandler req_handler(location_path, out_config, mock_client);
  http::request<http::string_body> req;
  req.method(http::verb::get);
  req.target(location_path);

  http::response<http::string_body> res = req_handler.handle_request(req);

  EXPECT_EQ(http::status::ok, res.result());
  EXPECT_EQ(proxy_res.body(), res.body());
}