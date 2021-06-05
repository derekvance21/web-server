#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <deque>
#include "request_handler.h"
#include "login_handler.h"
#include "not_found_handler.h"
#include "static_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;
int MAX_NUM_COOKIES = 15;

LoginHandler::LoginHandler(const std::string& location_path, const NginxConfig& config, std::deque<std::string>& cookies, http::response<http::string_body> redirect_result)
  : RequestHandler(location_path, config), cookies_(cookies), redirect_res(redirect_result)
{}

http::response<http::string_body> LoginHandler::handle_request(const http::request<http::string_body>& request)
{
    // if we recieve a get request to /login, then we want to serve the html form for the user
    if(request.method() == http::verb::get) {
        return handle_get_request(request);
    }
    // if we get a post request to /login, this means that the user entered a password in the
    //  html form and we want to check the validity of the password from the request body
    else if(request.method() == http::verb::post) {
        return handle_post_request(request);
    }
    else {
        NotFoundHandler obj(location_path, config);
        return obj.handle_request(request);
    }
}

// handle_post_request is called when the user inputs a password and hits the Login button
// this function verifies the correct password is entered
http::response<http::string_body> LoginHandler::handle_post_request(const http::request<http::string_body>& request)
{
    http::response<http::string_body> res;
    std::string res_body;
    std::string content_length;
    res.version(request.version());

    std::string body = request.body();
    std::string password = body.substr(body.find("password=")+9);
    
    if(password == "juzang-password") {
        // Generate and set cookie
        std::string cookie = generate_cookie();
        if(set_cookie(cookie)){
          redirect_res.set(http::field::set_cookie, "session_id=" + cookie);
        } 

        // return the resource originally requested
        return redirect_res; 
    }

    // if the cookie is incorrect, return an unauthorized response
    res_body = "Authentication Denied\n";
    content_length = std::to_string(res_body.length());
    res.result(http::status::unauthorized);
    res.set(http::field::content_type, "text/html");
    res.set(http::field::content_length, content_length);
    res.body() = res_body;
    return res;
}

// handle_get_request is called when a user makes a request to /login
// this function renders the html login page
http::response<http::string_body> LoginHandler::handle_get_request(const http::request<http::string_body>& request)
{
    // Currently the handle_get_request function has the same functionality as the Static Handler's login function.
// Do we want to create an instance of a static handler and use its handle_request function instead 
// to display the html page?

    // get the login html file
    //StaticHandler obj("/static", config);
    //return obj.handle_request(request);
    
    std::string fullpath = GetPath(request);
    std::cerr << "FULL PATH:" <<fullpath;
    std::string file_content;
    int code = ReadFile(fullpath, file_content);

    if (code == 1){
        NotFoundHandler obj(location_path, config);
        return obj.handle_request(request);
    }

    size_t version = request.version();

    http::response<http::string_body> res;
    std::string content_length = std::to_string(file_content.length());

    // return the login html file as the response body so the user can see the html form and login
    res.version(version);
    res.result(http::status::ok);
    res.set(http::field::content_type, "text/html");
    res.set(http::field::content_length, content_length);
    res.body() = file_content;

    return res;
}

/* Generates the path for the file requested */
std::string LoginHandler::GetPath(const http::request<http::string_body>& request){

  std::string uri_path = "/login.html";
  std::string req_path = "", full_path = "";

  /*// Check for whether the handler's location is specified in the uri path
  // Ex. /static, /static1, /static2 etc.
  size_t loc_found = uri_path.find(location_path, 0);
  if (loc_found != std::string::npos)
    req_path = uri_path.substr(loc_found+location_path.length());
  */
  // if file path passed correctly, look for root path in child config block
  //std::cerr <<"CONFIG" << std::string(config);
  std::cerr << "CONFIG" << config.statements_[0]->tokens_[1];
  if (!config.statements_.empty() &&
      config.statements_[0]->tokens_.size() == 2 &&
      config.statements_[0]->tokens_[0] == "root"){
    std::string root = config.statements_[0]->tokens_[1];
    full_path = root + uri_path;
  }

  // Note: if full_path is empty or invalid, ReadFile() will catch it and 404 will be returned
  return full_path;
}

// helper function to check if we can read the file
int LoginHandler::ReadFile(std::string fullpath, std::string& file_content) {

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

std::string LoginHandler::generate_cookie() // used https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(15,0);
    std::generate_n( str.begin(), 15, randchar );
    return str;
}

bool LoginHandler::set_cookie(std::string cookie){
  if(cookies_.size() >= MAX_NUM_COOKIES){
    cookies_.pop_front();
  }
  cookies_.push_back(cookie);
  return true;
}

std::deque<std::string> LoginHandler::get_cookies()
{
  return cookies_;
}