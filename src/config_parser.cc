// An nginx config file parser.	
//	
// See:	
//   http://wiki.nginx.org/Configuration	
//   http://blog.martinfjordvald.com/2010/07/nginx-primer/	
//	
// How Nginx does it:	
//   http://lxr.nginx.org/source/src/core/ngx_conf_file.c	
#include <cstdio>	
#include <fstream>	
#include <iostream>	
#include <memory>	
#include <stack>	
#include <string>	
#include <vector>	
#include <map>
#include "config_parser.h"
#include <boost/asio.hpp>
#include "logger.h"

using boost::asio::ip::tcp;

// adding new function to return the port number in the config file
int NginxConfig::GetPort() {

  // set up logging
  Logger* instance = Logger::getInstance();
  
  /* parse config file to find port number */
  std::string config_string = this->ToString();

  /* First, look for port, if found compute the port number position */
  int port_pos = config_string.find("port ");
  if(port_pos == std::string::npos) {

    // If cannot find port, look for listen and if found compute port number position
    // else error
    port_pos = config_string.find("listen ");
    if(port_pos == std::string::npos) {
      std::cerr << "No port in config file. Usage: port <port num>;\n";
      instance->log_server_initialization_failure("No port in config file. Usage: port <port num>;\n");
      return -1;
    }

    port_pos += 7;
  } else {
    port_pos += 5;
  }

  std::string port = config_string.substr(port_pos);

  using namespace std; // For atoi.
  if(atoi(port.c_str()) == 0){
    std::cerr << "Invalid port in config file. Usage: port <port num>;\n";
    instance->log_server_initialization_failure("Invalid port in config file. Usage: port <port num>;\n");
    return -1;
  }
  instance->log_server_initialization(port.c_str());
  return atoi(port.c_str());
}

// returns a map of the request location, request handler name, and nginxconfig child object
std::map<std::string, std::pair<std::string, NginxConfig>> NginxConfig::GetLocationMap() {
  std::map<std::string, std::pair<std::string, NginxConfig>> location_handlers;
  for (const auto& statement : statements_) {
    if (statement->tokens_.size() == 3 && 
        statement->tokens_.front() == "location" &&
        statement->child_block_.get() != nullptr) {
      std::string location = statement->tokens_.at(1);
      std::string handler = statement->tokens_.back();
      NginxConfig child = *(statement->child_block_);

      location_handlers[location] = std::pair<std::string, NginxConfig>(handler, child);
    }
  }
  return location_handlers;
}


std::string NginxConfig::ToString(int depth) {	
  std::string serialized_config;	
  for (const auto& statement : statements_) {	
    serialized_config.append(statement->ToString(depth));	
  }	
  return serialized_config;	
}	

std::string NginxConfigStatement::ToString(int depth) {	
  std::string serialized_statement;	
  for (int i = 0; i < depth; ++i) {	
    serialized_statement.append("  ");	
  }	
  for (unsigned int i = 0; i < tokens_.size(); ++i) {	
    if (i != 0) {	
      serialized_statement.append(" ");	
    }	
    serialized_statement.append(tokens_[i]);	
  }	
  if (child_block_.get() != nullptr) {	
    serialized_statement.append(" {\n");	
    serialized_statement.append(child_block_->ToString(depth + 1));	
    for (int i = 0; i < depth; ++i) {	
      serialized_statement.append("  ");	
    }	
    serialized_statement.append("}");	
  } else {	
    serialized_statement.append(";");	
  }	
  serialized_statement.append("\n");	
  return serialized_statement;	
}	
const char* NginxConfigParser::TokenTypeAsString(TokenType type) {	
  switch (type) {	
    case TOKEN_TYPE_START:         return "TOKEN_TYPE_START";	
    case TOKEN_TYPE_NORMAL:        return "TOKEN_TYPE_NORMAL";	
    case TOKEN_TYPE_START_BLOCK:   return "TOKEN_TYPE_START_BLOCK";	
    case TOKEN_TYPE_END_BLOCK:     return "TOKEN_TYPE_END_BLOCK";	
    case TOKEN_TYPE_COMMENT:       return "TOKEN_TYPE_COMMENT";	
    case TOKEN_TYPE_STATEMENT_END: return "TOKEN_TYPE_STATEMENT_END";	
    case TOKEN_TYPE_EOF:           return "TOKEN_TYPE_EOF";	
    case TOKEN_TYPE_ERROR:         return "TOKEN_TYPE_ERROR";	
    case TOKEN_TYPE_QUOTED_STRING: return "TOKEN_TYPE_QUOTED_STRING";	
    default:                       return "Unknown token type";	
  }	
}	
NginxConfigParser::TokenType NginxConfigParser::ParseToken(std::istream* input,	
                                                           std::string* value) {	
  TokenParserState state = TOKEN_STATE_INITIAL_WHITESPACE;	
  while (input->good()) {	
    const char c = input->get();	
    if (!input->good()) {	
      break;	
    }	
    switch (state) {	
      case TOKEN_STATE_INITIAL_WHITESPACE:	
        switch (c) {	
          case '{':	
            *value = c;	
            return TOKEN_TYPE_START_BLOCK;	
          case '}':	
            *value = c;	
            return TOKEN_TYPE_END_BLOCK;	
          case '#':	
            *value = c;	
            state = TOKEN_STATE_TOKEN_TYPE_COMMENT;	
            continue;	
          case '"':	
            *value = c;	
            state = TOKEN_STATE_DOUBLE_QUOTE;	
            continue;	
          case '\'':	
            *value = c;	
            state = TOKEN_STATE_SINGLE_QUOTE;	
            continue;	
          case ';':	
            *value = c;	
            return TOKEN_TYPE_STATEMENT_END;	
          case ' ':	
          case '\t':	
          case '\n':	
          case '\r':	
            continue;	
          default:	
            *value += c;	
            state = TOKEN_STATE_TOKEN_TYPE_NORMAL;	
            continue;	
        }	
      case TOKEN_STATE_SINGLE_QUOTE:	
      case TOKEN_STATE_DOUBLE_QUOTE:	
        *value += c;	
        if (c == (state == TOKEN_STATE_SINGLE_QUOTE ? '\'' : '"')) {	
          if (input->peek() != ' ' && input->peek() != '\t' && input->peek() != '\n' && input->peek() != '{' && input->peek() != '\r' && input->peek() != ';') {	
            return TOKEN_TYPE_ERROR;	
          }	
          return TOKEN_TYPE_QUOTED_STRING;	
        } else if (c == '\\') {	
          state = state == TOKEN_STATE_SINGLE_QUOTE ? TOKEN_STATE_SINGLE_QUOTE_ESCAPED : TOKEN_STATE_DOUBLE_QUOTE_ESCAPED;	
        }	
        continue;	
      case TOKEN_STATE_SINGLE_QUOTE_ESCAPED:	
        *value += c;	
        state = TOKEN_STATE_SINGLE_QUOTE;	
        continue;	
      case TOKEN_STATE_DOUBLE_QUOTE_ESCAPED:	
        *value += c;	
        state = TOKEN_STATE_DOUBLE_QUOTE;
	      continue;
      case TOKEN_STATE_TOKEN_TYPE_COMMENT:	
        if (c == '\n' || c == '\r') {	
          return TOKEN_TYPE_COMMENT;	
        }	
        *value += c;	
        continue;	
      case TOKEN_STATE_TOKEN_TYPE_NORMAL:	
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' ||	
            c == ';' || c == '{' || c == '}') {	
          input->unget();	
          return TOKEN_TYPE_NORMAL;	
        }	
        *value += c;	
        continue;	
    }	
  }	
  // If we get here, we reached the end of the file.	
  if (state == TOKEN_STATE_SINGLE_QUOTE ||	
      state == TOKEN_STATE_DOUBLE_QUOTE) {	
    return TOKEN_TYPE_ERROR;	
  }	
  return TOKEN_TYPE_EOF;	
}	
bool NginxConfigParser::Parse(std::istream* config_file, NginxConfig* config) {	
  std::stack<NginxConfig*> config_stack;	
  config_stack.push(config);	
  TokenType last_token_type = TOKEN_TYPE_START;	
  TokenType token_type;	
  while (true) {	
    std::string token;	
    token_type = ParseToken(config_file, &token);	
   	
    if (token_type == TOKEN_TYPE_ERROR) {	
      break;	
    }	
    if (token_type == TOKEN_TYPE_COMMENT) {	
      // Skip comments.	
      continue;	
    }	
    if (token_type == TOKEN_TYPE_START) {	
      // Error.	
      break;	
    } else if (token_type == TOKEN_TYPE_NORMAL || token_type == TOKEN_TYPE_QUOTED_STRING) {	
      if (last_token_type == TOKEN_TYPE_START ||	
          last_token_type == TOKEN_TYPE_STATEMENT_END ||	
          last_token_type == TOKEN_TYPE_START_BLOCK ||	
          last_token_type == TOKEN_TYPE_END_BLOCK ||	
          last_token_type == TOKEN_TYPE_NORMAL) {	
        if (last_token_type != TOKEN_TYPE_NORMAL) {	
          config_stack.top()->statements_.emplace_back(	
              new NginxConfigStatement);	
        }	
        config_stack.top()->statements_.back().get()->tokens_.push_back(	
            token);	
      } else {	
        // Error.	
        break;	
      }	
    } else if (token_type == TOKEN_TYPE_STATEMENT_END) {	
      if (last_token_type != TOKEN_TYPE_NORMAL && last_token_type != TOKEN_TYPE_QUOTED_STRING) {	
        // Error.	
        break;	
      }	
    } else if (token_type == TOKEN_TYPE_START_BLOCK) {	
      if (last_token_type != TOKEN_TYPE_NORMAL) {	
        // Error.	
        break;	
      }	
      NginxConfig* const new_config = new NginxConfig;	
      config_stack.top()->statements_.back().get()->child_block_.reset(	
          new_config);	
      config_stack.push(new_config);	
    } else if (token_type == TOKEN_TYPE_END_BLOCK) {	
      if (last_token_type != TOKEN_TYPE_STATEMENT_END && 	
          last_token_type != TOKEN_TYPE_START_BLOCK && // empty blocks	
          last_token_type != TOKEN_TYPE_END_BLOCK) { // nested blocks	
        // Error.	
        break;	
      }	
      config_stack.pop();	
      if (config_stack.size() == 0) {	
        break; // end of block without a matching start of block	
      }	
    } else if (token_type == TOKEN_TYPE_EOF) {	
      if (last_token_type != TOKEN_TYPE_STATEMENT_END &&	
          last_token_type != TOKEN_TYPE_START && // config file can be empty file	
          last_token_type != TOKEN_TYPE_END_BLOCK) {	
        // Error.	
        break;	
      }	
      // check to make sure no extra characters (like unmatching brace)
      else if(!config_stack.empty() && config_stack.size() != 1){
        break;
      }
      return true;	
    } else {	
      // Error. Unknown token.	
      break;	
    }	
    last_token_type = token_type;	
  }	
  printf ("Bad transition from %s to %s\n",	
          TokenTypeAsString(last_token_type),	
          TokenTypeAsString(token_type));	
  return false;	
}	
bool NginxConfigParser::Parse(const char* file_name, NginxConfig* config) {	
  std::ifstream config_file;	
  config_file.open(file_name);	
  if (!config_file.good()) {	
    printf ("Failed to open config file: %s\n", file_name);	
    return false;	
  }	
  const bool return_value =	
      Parse(dynamic_cast<std::istream*>(&config_file), config);	
  config_file.close();	
  return return_value;	
}
