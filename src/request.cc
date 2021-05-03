#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "request.h"



Request::Request(const std::string& request) 
  : request_(request)
{
  // std::cerr << "parsing request: " << request << "\n";
  // ParseRequest();
  // std::cerr << "done parsing request" << "\n";
}

/* Transform request string into request data structure */
int Request::ParseRequest()
{
  // TODO: Validate - Make Sure Basic Format is Met
  //std::string valid_request = validate_request(request);

  // Get: All Relevant Data From Request
  try {
    ExtractMethod();
    ExtractPath();
    ExtractVersion();
    ExtractHeaders();
    ExtractBody();
    
    return 0;
  } catch (int error) { return 1; }
}

/* Extract the method (i.e., GET/POST etc.) */
int Request::ExtractMethod()
{
  int first_ws = request_.find(' ', 0);

  // Error handling
  if (first_ws == std::string::npos) {
    method_ = "";
    return 1;
  }
    
  method_ = request_.substr(0, first_ws);
  
  return 0;
}

/* Extract the path (i.e., path) */
int Request::ExtractPath()
{
  int first_ws = request_.find(' ', 0);
  int second_ws = request_.find(' ', first_ws+2);

  // Error handling
  if (first_ws == std::string::npos || second_ws == std::string::npos) {
    path_ = "";
    return 1;
  }
    
  path_ = std::string(request_.begin() + first_ws + 1, request_.begin() + second_ws);

  return 0;
}

/* Extract HTTP version (i.e., HTTP\1.1) */
int Request::ExtractVersion()
{
  int first_ws = request_.find(' ', 0);
  int second_ws = request_.find(' ', first_ws+2);
  int first_rn = request_.find("\r\n", 0);

  // Error handling
  if (first_ws == std::string::npos ||
      second_ws == std::string::npos ||
      first_rn == std::string::npos) {
    version_ = "";
    return 1;
  }
  
  version_ = std::string(request_.begin() + second_ws + 1, request_.begin() + first_rn);

  return 0;
}


/* Extract Headers, return a map of header type and its content */
int Request::ExtractHeaders()
{
  int first_rn = request_.find("\r\n", 0);
  int last_rnrn = request_.find("\r\n\r\n", 0);

  // No Headers provided 
  if (first_rn == last_rnrn ||
      first_rn == std::string::npos ||
      last_rnrn == std::string::npos)
    return 1;
  
  // Gets the blocks of headers (only) as a string to be transformed into a map
  std::string headers = std::string(request_.begin() + first_rn + 1, request_.begin() + last_rnrn);

  std::string header_type = "";
  std::string header_content = "";
  bool switch_flag = false;
  int end = 0;

  // Loop through the headers and get whatever before ": " as a header type (i.e, host)
  // and the data between ": " and "\r\n" will be its header content
  // Ex: "host: ucla.edu\r\n" becomes {{"host": "ucla.edu"}}
  
  for (int i = 0; i < headers.size(); i++){

    if (!switch_flag)
      header_type += headers[i];
    else
      header_content += headers[i];

    if (headers[i] == '\r' && headers[i+1] == '\n'){
      std::pair<std::string, std::string> pair (header_type, header_content);
      headers_.insert(pair);

      header_type = "";
      header_content = "";
      switch_flag = false;
      i++;
    }

    if (headers[i] == ':' && headers[i+1] == ' '){
      i++;
      switch_flag = true;
    }
  }

  return 0;
}

int Request::ExtractBody()
{
  int start_body = request_.find("\r\n\r\n", 0);

  // Error handling
  if (start_body == std::string::npos) {
    body_ = "";
    return 1;
  }
  
  body_ = request_.substr(start_body+4);
  return 0;

}


/* Getters & Setters (mainly for testing) */
std::string Request::GetRequest() { return request_; }
std::string Request::GetMethod() { return method_; }
std::string Request::GetPath() { return path_; }
std::string Request::GetVersion() { return version_; }
std::string Request::GetBody() { return body_; }
std::unordered_map<std::string, std::string> Request::GetHeaders() { return headers_; }
