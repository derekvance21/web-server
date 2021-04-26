#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "static.h"
#include "request.h"
#include "404.h"

StaticResponse::StaticResponse(const std::string& fullpath)
  : fullpath_(fullpath)
{}


/* Main Function: Format the response based on client request */
std::string StaticResponse::GetResponse()
{
  // Get the extension of the file (i.e., txt)
  std::size_t dot = fullpath_.find_last_of(".");
  std::string extension = "";
  
  if (dot != std::string::npos)
    extension = fullpath_.substr(dot+1);

  // Get the file content (reading its data)
  std::string file_content;
  int code = ReadFile(fullpath_, file_content);

  // If file doesnt exist return 404
  if (code == 1){
    NotFoundResponse obj;
    return obj.GetResponse();
  }
    

  
  // Get the content type (i.e., text/html)
  std::string content_type = GetContentType(extension);

  // Get the length of the data
  std::string content_length = std::to_string(file_content.length());

  // Finally, format the proper response message and return it
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
  else if (extension == "zip")
    content_type = "application/zip";
  else
    content_type = "text/plain";

  return content_type;
}


/* Read whole file data to be returned */
int StaticResponse::ReadFile(std::string fullpath, std::string& file_content) {

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
