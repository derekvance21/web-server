#include "status.h"
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <map>
#include <cstdlib>
namespace http = boost::beast::http;

class NginxConfig;
using boost::asio::ip::tcp;

Status::Status(std::map<std::string, std::pair<std::string, NginxConfig>> loc_map) 
 : loc_map_(loc_map) {}

// return response that lists all requests, broken down by url and response code
std::map<std::string, http::status> Status::get_requests() {
    return requests_;
}

// return response that lists all request handlers and their url prefixes
std::map<std::string, std::string> Status::get_request_handlers() {
    return request_handlers_;
}

// TODO: call this function in session.cc
void Status::insert_request(std::string url, http::status response_code) {
    requests_[url] = response_code;
}

// TODO: call this function 
// potentially make a function in session.cc or server.cc called 
// get_request_handlers or location_map_status which will interate over our 
// loc_map_ and call this function insert_handler on every element of loc_map_
void Status::insert_handler(std::string handler, std::string url) {
    request_handlers_[handler] = url;
}