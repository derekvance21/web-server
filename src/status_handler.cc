#include <sstream>
#include <string>
#include "request_handler.h"
#include "status_handler.h"
#include "status.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

StatusHandler::StatusHandler(const std::string& location_path, const NginxConfig& config, Status* status)
  : RequestHandler(location_path, config), status_(status)
{}

// TODO: return status of webserver
// call status_->get_requests() and status_->get_request_handlers()
// and iterate through the maps and format their contents as the 
// returned http::response body
http::response<http::string_body> StatusHandler::handle_request(const http::request<http::string_body>& request)
{
  std::map<std::string, std::vector<int>> requests = status_->get_requests();
  std::map<std::string, std::vector<std::string>> request_handlers = status_->get_request_handlers();

  int urls_count = get_urls_count(requests);

  /* Part-1: Display Count of URL requested and breakdown of requests/status codes */
  std::string data = "Server Status Information...\n\nURL Requested Count: " +
    std::to_string(urls_count) + "\n\nList of URLs and their Status Code...\n\n";

  // Note: if no requests the program will not enter the subsequent for loop
  if (requests.empty())
    data += "No requests received yet.\n";

  // Loop through the map of urls -> list of status codes and display each of them
  for (std::map<std::string, std::vector<int>>::iterator it = requests.begin(); it != requests.end(); ++it) {
    std::vector<int> status_vec = it->second;

    // Breakdown each request and its returned status code
    for (int http_status : status_vec) {
      std::string status = std::to_string(http_status);
      data += (it->first + " - " + status + "\n");
    }
  }

  
  /* Part-2: Display Existing Handlers */
  data += "\nExisting Request Handlers and their Prefixes...\n\n";

  // Loop through the map of request_handlers -> list of prefixes and display each of them
  for (std::map<std::string, std::vector<std::string>>::iterator handler_it = request_handlers.begin();
       handler_it != request_handlers.end(); ++handler_it) {

    // Add Handler Name (Ex. StaticHandler - List of Prefixes: )
    data += (handler_it->first + " - List of Prefixes: \n");

    std::vector<std::string> prefixes = handler_it->second;

    // Loop through all the prefixes of given handler and add it to data
    for (std::string prefix : prefixes){
      data += (prefix + "\n");
    }

    data += "\n";
  }

  // Gather necessary information from request to be used in response
  size_t version = request.version();
  size_t content_length = data.length();
  
  /* Generate the response */
  http::response<http::string_body> status_response;

  // Complete necessary fields
  status_response.version(version);
  status_response.result(http::status::ok);
  status_response.set(http::field::content_type, "text/plain");
  status_response.set(http::field::content_length, content_length);
  status_response.body() = data;
  
  return status_response;
}


/* Helper: simply computes the total number of requests made to the server */
int StatusHandler::get_urls_count(std::map<std::string, std::vector<int>> requests)
{
  int urls_count = 0;

  // Simply loop through all different requests and get the size of the vector of responses and add it to the count
  for (std::map<std::string, std::vector<int>>::iterator it = requests.begin(); it != requests.end(); ++it) {
    
    std::vector<int> status_vec = it->second;
    urls_count += status_vec.size();
  }

  return urls_count;
}
