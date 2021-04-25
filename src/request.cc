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
  try {
    int first_ws = request_.find(' ', 0);
    method_ = first_ws == std::string::npos ? "" : request_.substr(0, first_ws);
    return 0;
  } catch (int error) {
    method_ = "";
    return 1;
  }

}

/* Extract the path (i.e., path) */
int Request::ExtractPath()
{
  try {
    int first_ws = request_.find(' ', 0);
    int second_ws = request_.find(' ', first_ws+2);
    // path_ = std::string(request_.begin() + first_ws + 1, request_.begin() + second_ws);
    path_ = request_.substr(first_ws + 1, second_ws - first_ws - 1);
    return 0;
  } catch (int error) {
    path_ = "";
    return 1;
  }

}

/* Extract HTTP version (i.e., HTTP\1.1) */
int Request::ExtractVersion()
{
  try {
  int first_ws = request_.find(' ', 0);
  int second_ws = request_.find(' ', first_ws+2);
  int first_rn = request_.find("\r\n", 0);
  
  version_ = std::string(request_.begin() + second_ws + 1, request_.begin() + first_rn);

  return 0;
  } catch (int error) {
    version_ = "";
    return 1;
  }
}


/* Extract Headers, return a map of header type and its content */
int Request::ExtractHeaders()
{
  int first_rn = request_.find("\r\n", 0);
  int last_rnrn = request_.find("\r\n\r\n", 0);

  // No Headers provided 
  if (first_rn == last_rnrn)
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

  try{
    body_ = request_.substr(start_body+4);
    return 0;
    
  } catch (int error) {
    body_ = "";
    return 1;
  }
}


/* Getters & Setters (mainly for testing) */
std::string Request::GetRequest() { return request_; }
std::string Request::GetMethod() { return method_; }
std::string Request::GetPath() { return path_; }
std::string Request::GetVersion() { return version_; }
std::string Request::GetBody() { return body_; }
std::unordered_map<std::string, std::string> Request::GetHeaders() { return headers_; }

void Request::SetRequest(std::string request) { request_ = request; }
void Request::SetMethod(std::string method) { method_ = method; }
void Request::SetPath(std::string path) { path_ = path; }
void Request::SetVersion(std::string version) { version_ = version; }
void Request::SetBody(std::string body) { body_ = body; }
void Request::SetHeaders(std::unordered_map<std::string, std::string> headers) { headers_ = headers; }
