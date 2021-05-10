#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "static_handler.h"
#include "not_found_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

StaticHandler::StaticHandler(const std::string& location_path, const NginxConfig& config)
  : RequestHandler(location_path, config)
{}


/* Main Function: Format the response based on client request */
http::response<http::string_body> StaticHandler::handle_request(const http::request<http::string_body>& request)
{
  std::string fullpath = GetPath(request);
  std::cerr << "fullpath: " << fullpath << "\n";
  
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

/* Generates the path for the file requested */
std::string StaticHandler::GetPath(const http::request<http::string_body>& request){
  
  std::ostringstream oss;
  oss << request.target();
  std::string uri_path = oss.str();
  std::string req_path = "", full_path = "";

  // Check for whether the handler's location is specified in the uri path
  // Ex. /static, /static1, /static2 etc.
  size_t loc_found = uri_path.find(location_path, 0);
  if (loc_found != std::string::npos)
    req_path = uri_path.substr(loc_found+location_path.length());
  
  // if file path passed correctly, look for root path in child config block
  if (!config.statements_.empty() &&
      config.statements_[0]->tokens_.size() == 2 &&
      config.statements_[0]->tokens_[0] == "root"){
    // Remove double quoatations from root path
    std::string root = config.statements_[0]->tokens_[1];
    size_t root_length = root.length();

    // In future: might improve by using a predefined library if exists
    if ( root_length > 0 && (root[0] == '"' || root[0] == '\'')){
      root = root.erase(0, 1);
      root_length = root.length();
    }

    if ( root_length > 0 && (root[root_length - 1] == '"' || root[root_length - 1] == '\''))
      root = root.erase(root_length - 1);

    full_path = root + req_path;
  }

  // Note: if full_path is empty or invalid, ReadFile() will catch it and 404 will be returned
  return full_path;
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
