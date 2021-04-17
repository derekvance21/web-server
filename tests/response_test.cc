#include "gtest/gtest.h"
#include "../src/response.h"

#include <string>

class ResponseGeneratorTest : public ::testing::Test {
  protected:
    response res;
};


/* ---------------- */
/* Unit Tests Below */
/* ---------------- */

TEST_F(ResponseGeneratorTest, BasicResponse)
{
  // Set-Up Response Message To Be Sent
  res.SetData("This is Team Juzang!");
  res.SetBytesTransferred(std::to_string(20));

  // Get Both Expected/Received Response
  std::string response_gotten = res.GetResponse();
  std::string response_expect = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 20\r\n\r\nThis is Team Juzang!";

  // Both Should Be Equal
  EXPECT_TRUE(response_gotten == response_expect);
}

