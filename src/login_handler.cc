#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <deque>
#include "request_handler.h"
#include "login_handler.h"
#include "not_found_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;
int MAX_NUM_COOKIES = 15;

LoginHandler::LoginHandler(const std::string& location_path, const NginxConfig& config, std::deque<std::string>& cookies)
  : RequestHandler(location_path, config), cookies_(cookies)
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
          res.set(http::field::set_cookie, "session_id=" + cookie);
        } 

        // TODO: here is probably where we would do the redirection
        // Set rest of fields
        res_body = "Authentication Successful\n";
        content_length = std::to_string(res_body.length());
        res.result(http::status::ok);
        res.set(http::field::content_type, "text/html");
        res.set(http::field::content_length, content_length);
        res.body() = res_body;
        return res; 
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
    std::string fullpath = "../tests/static_folder/login.html";
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

bool LoginHandler::set_cookie(std::string cookie)
{
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
