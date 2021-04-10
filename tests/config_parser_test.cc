#include "gtest/gtest.h"
#include "config_parser.h"


class NginxConfigParserTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;
};

TEST_F(NginxConfigParserTest, EmptyConfig) {
  EXPECT_TRUE(parser.Parse("example_config/example_empty_config", &out_config));

}

TEST_F(NginxConfigParserTest, BasicConfig) {
  EXPECT_TRUE(parser.Parse("example_config/example_basic_config", &out_config));
}

TEST_F(NginxConfigParserTest, RecursiveConfig) {
  EXPECT_TRUE(parser.Parse("example_config/example_recursive_config", &out_config));
}

TEST_F(NginxConfigParserTest, CommentConfig) {
  EXPECT_TRUE(parser.Parse("example_config/example_comment_config", &out_config));
}

TEST_F(NginxConfigParserTest, IncorrectSingleBlockConfig) {
  EXPECT_FALSE(parser.Parse("example_config/example_incorrect_single_block_config", &out_config));
}

TEST_F(NginxConfigParserTest, IncorrectStatementConfig) {
  EXPECT_FALSE(parser.Parse("example_config/example_incorrect_statement_config", &out_config));
}

TEST_F(NginxConfigParserTest, IncorrectNestedBlockConfig) {
  EXPECT_FALSE(parser.Parse("example_config/example_incorrect_nested_block_config", &out_config));
}

TEST_F(NginxConfigParserTest, IncorrectSingleQuoteConfig) {
  EXPECT_FALSE(parser.Parse("example_config/example_incorrect_single_quote_config", &out_config));
}

TEST_F(NginxConfigParserTest, IncorrectQuotedStringConfig) {
  EXPECT_FALSE(parser.Parse("example_config/example_incorrect_quoted_string_config", &out_config));
}
