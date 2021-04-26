#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "static_handler.h"
#include "request.h"
#include "404.h"

StaticResponse::StaticResponse(const std::string& fullpath)
  : fullpath_(fullpath)
{}

std::string StaticResponse::GetResponse()
{
  // Get the filename (i.e., file.txt)
  std::size_t last_slash = fullpath_.find_last_of("/\\");
  std::string filename = fullpath_.substr(last_slash+1);

  // Get the extension of the file (i.e., txt)
  std::size_t dot = filename.find_last_of(".");
  std::string extension = filename.substr(dot+1);

  
  std::cerr << fullpath_ << std::endl; //passes here
  
  std::string file_content = ReadFile(fullpath_);

  std::string content_type = GetContentType(extension);

  std::string content_length = std::to_string(file_content.length());

  std::string response = FormatResponse(content_type, content_length, file_content);

  return response;
}

/* Collects all data and format the proper response message */
std::string StaticResponse::FormatResponse(std::string content_type,
					   std::string content_length,
					   std::string file_content) {
  
  try {
    std::stringstream response_msg;
    response_msg << "HTTP/1.1 200 OK\r\n";
    response_msg << "Content-Type: " << content_type << "\r\n";
    response_msg << "Content-Length: " << content_length;
    response_msg << "\r\n\r\n";
    response_msg << file_content;

    return response_msg.str();
    
  } catch(int error) {
    return "";
  }
}

/* Based on file extension, return proper content type attribute */
std::string StaticResponse::GetContentType(std::string extension) {
  std::string content_type;
  
  if (extension == "html" || extension == "htm")
    content_type = "text/html";
  else if (extension == "jpg" || extension == "jpeg")
    content_type = "image/jpeg";
  else if (extension == "png")
    content_type = "image/png";
  else
    content_type = "text/plain";

  return content_type;
}


/* Read whole file data to be returned */
std::string StaticResponse::ReadFile(std::string fullpath) {

  std::string file_content = "";
  
  // Open up file stdin vs binary
  std::ifstream file(fullpath, std::ios::in | std::ios::binary);

  // ERROR handling
  if ( ! file.is_open() )
    return "";

  char c;
  while (file.get(c))
    file_content += c;
  
  return file_content;
}
