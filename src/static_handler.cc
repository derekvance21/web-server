#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "static_handler.h"
#include "request.h"
#include "not_found_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

StaticHandler::StaticHandler(const std::string& location_path, const NginxConfig& config)
  : RequestHandler(location_path, config)
{}


/* Main Function: Format the response based on client request */
http::response<http::string_body> StaticHandler::handle_request(const http::request<http::string_body>& request)
{
  std::ostringstream oss;
  oss << request.target();
  std::string fullpath = oss.str();
  
  std::size_t dot = fullpath.find_last_of(".");
  std::string extension = "";
  
  if (dot != std::string::npos)
    extension = fullpath.substr(dot+1);

  // Get the file content (reading its data)
  std::string file_content;
  int code = ReadFile(fullpath, file_content);

  // If file doesnt exist return 404
  if (code == 1){
    NotFoundHandler obj(location_path, config);
    return obj.handle_request(request);
  }
  
  // Get the content type (i.e., text/html)
  std::string content_type = GetContentType(extension);

  // Get the length of the data
  std::string content_length = std::to_string(file_content.length());

  // Get the version
  size_t version = request.version();

  // Finally, format the proper response message and return it
  http::response<http::string_body> res = FormatResponse(version, content_type, content_length, file_content);

  return res;
}

/* Collects all data and format the proper response message */
http::response<http::string_body> StaticHandler::FormatResponse(size_t version,
								std::string content_type,
								std::string content_length,
								std::string file_content) {
  
  /* Generate the response */
  http::response<http::string_body> res;

  // Complete necessary fields
  res.version(version);
  res.result(http::status::ok);
  res.set(http::field::content_type, content_type);
  res.set(http::field::content_length, content_length);
  res.body() = file_content;

  return res;
}

/* Based on file extension, return proper content type attribute */
std::string StaticHandler::GetContentType(std::string extension) {
  std::string content_type;
  
  if (extension == "html" || extension == "htm")
    content_type = "text/html";
  else if (extension == "jpg" || extension == "jpeg")
    content_type = "image/jpeg";
  else if (extension == "png")
    content_type = "image/png";
  else if (extension == "zip")
    content_type = "application/zip";
  else
    content_type = "text/plain";

  return content_type;
}


/* Read whole file data to be returned */
int StaticHandler::ReadFile(std::string fullpath, std::string& file_content) {

  std::ifstream file(fullpath, std::ios::in | std::ios::binary);

  // Check if file or dir
  if( !file.good() )
    return 1;
  
  // Error handling (if file doesn't exist)
  if ( ! file.is_open() )
    return 1;

  // Read file data and return it
  char c;
  while (file.get(c))
    file_content += c;
  
  return 0;
}
