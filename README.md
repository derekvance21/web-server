# Documentation


## Getting Started


### Building

Building should be done in a separate directory than the root project directory, called `build/`, to preserve a readable file structure and avoid committing build files to the repository - the .gitignore file ignores files inside `build/`. The following commands will build the project:


```
mkdir build/
cd build/
cmake ..
make
```


If the above commands run without error, the main executable, `webserver`, will now be located at `build/bin/webserver`.


### Testing

After doing the above build process, you can run the included unit tests and integration tests with a single command:


```
make test
```


If you would like to see each failed test’s error messages, use the following command:


```
env CTEST_OUTPUT_ON_FAILURE=1 make test
```



### Running

To run the webserver, run the executable at build/bin/webserver with a single argument: the path to a server configuration file:


```
build/bin/webserver <path/to/config_file>
```


There exists a valid configuration file at `tests/example_config/config_locations`. 


#### Configuration File Format

The port the server listens on (in this example, 8080) should be configured as such:


```
port 8080;
```


Client HTTP request paths can be configured to be handled in different ways. 



*   EchoHandler: the HTTP request is echoed to the body of the HTTP response
*   StaticHandler: the path of the HTTP request is converted to a local filepath, and the contents of that file become the body of the HTTP response

The formats for configuring HTTP request paths are as follows:


```
location <HTTP request path prefix> EchoHandler { }
location <HTTP request path prefix> StaticHandler { 
	path <local route>; 
}
```


HTTP requests with the path of <HTTP request path prefix> then get handled by the specified handler type, and optionally use the arguments inside the { } block. For example, with the following configuration static handler:


```
location /gallery StaticHandler { path ../public/img; }
```


An HTTP request to the file `/gallery/dog.jpg` would then get converted to the local file path `../public/img/dog.jpg` , and the body of the HTTP response would be the contents of that local file. 

NOTE: `<HTTP request path prefix>` and `<local route>` should either both have trailing slashes or neither have trailing slashes.
NOTE: if `<local route>` is a relative file path, it will be relative to the working directory in which the `webserver` executable is ran from. So for the above location configuration example, if `webserver` is ran from the `build/` directory, requests to `/gallery` will be forwarded to `build/../public/img`.

## Source Code


### ConfigParser

The `NginxConfig` object is populated after using `NginxConfigParser::Parse`. It has the following methods:

`int GetPort()` 	gets the port number specified by the required configuration statement `port <port number>;` . On error, returns -1.

`map<string, pair<string, NginxConfig>> GetLocationMap()`	gets the mappings from HTTP uri path prefix to a pair of local route and blocked NginxConfig object (the contents inside { }).


### Server

The webserver executable’s main function instantiations one Server object, using the NginxConfig object that was created by parsing the input configuration file.


```
Server(boost::asio::io_service& io_service, 
NginxConfig& config, 
    bool test_flag = false );	
```


Constructor: `io_service `can be a default instantiated `boost::asio::io_service `object, `config `should be the `NginxConfig `object generated from parsing the input configuration file.

```
int start_accept()
```
starts the server


```
int handle_accept(Session* new_session, 
const boost::system::error_code& error)
```


Function that handles individual client connections. Each client is handled using a new `Session `object, and `new_session` is a pointer to that `Session `object.


### Session

A `Session` object is created in the `Server` class and defines the functionality for reading requests from a client and sending back responses. 


```
 Session(boost::asio::io_service& io_service,
         bool test_flag = false,
         const loc_map_type& loc_map = loc_map_type());
```


Constructor: `io_services` can be a default instantiated `boost::asio::io_service `object, `test_flag` is a bool used to flag logging, and `loc_map` is a map that stores location information of request handlers defined in the config file.


```
typedef std::map<std::string, std::pair<std::string,NginxConfig>> loc_map_type;
```


`loc_map_type` defines a map of a string to a pair of a string and an NginxConfig object. This map stores a string representing the location_path of a request handler, a string representing the handler name of the request handler, an NginxConfig that represents the block-scoped NginxConfig object containing its arguments. These values are retrieved from the config file used on server startup.

Session defines the following functions to read requests from a client:


```
void start();
```


start calls handle_read() to begin accepting requests from a client


```
void handle_read();
```


handle_read reads asynchronously from a socket connection


```
int loopback_read(const boost::system::error_code& error, size_t bytes_transferred);
```


loopback_read calls handle_read() if no errors occur

Session defines the following functions to write responses to the client:


```
void handle_write(std::string response_msg, std::string type);
```


handle_write writes asynchronous to a socket connection


```
int send_response(const boost::system::error_code& error, size_t bytes_transferred);
```


send_response acts as a url dispatcher to get a response from the appropriate request handler and calls handle_write to send the response to the client


```
 RequestHandler* createHandler(std::string location, std::string handler, NginxConfig config_child);
```


createHandler creates and returns an appropriate request handler depending on the handler name passed in the config file


### RequestHandler

The RequestHandler is an abstract class that defines the API for the classes NotFoundHandler, EchoHandler, and StaticHandler. The RequestHandler defines a constructor, a virtual function handle_request, and a function GetBody as follows:


```
RequestHandler(const std::string& location_path, const NginxConfig& config): location_path(location_path), config(config) {}
```


The constructor takes an argument location_path that represents the location of the request handler defined in the config file and an argument config that represents the block-scoped NginxConfig object containing its arguments.


```
virtual http::response<http::string_body> handle_request(const http::request<http::string_body>& request) = 0;
```


`handle_request` is a virtual function that will be implemented in every class that extends `RequestHandler`. It takes a `boost::beast::http::request` object as a parameter that represents a request to the server and returns a `boost::beast::http::response` object that represents the server’s response.


#### NotFoundHandler

A NotFoundHandler is created when a static file is not found or when no other handler is reached. The class extends the abstract RequestHandler class and implements its handle_request function by returning a boost::beast::http::response object with a “Not Found” body.


#### EchoHandler

An EchoHandler is created when an echo request is received by the server that matches the echo request location defined in the config file. The class extends the abstract RequestHandler class and implements its handle_request function by returning a boost::beast::http::response object with the request echoed back in its body.


#### StaticHandler

A StaticHandler is created when a static request is received by the server that matches the static request location defined in the config file. The class extends the abstract RequestHandler class and implements its handle_request function by returning a boost::beast::http::response object that represents the requested static file. The response is formatted by reading the file contents to ensure it exists -- if it doesn’t, a NotFoundHandler is created and a NotFound response is returned -- then determining the file content type to correctly set the response’s content-type, and returning the response object.


## How to Create a Request Handler

To create a new request handler, NewRequest, we must make a header file, implementation file, and add support in session.cc and CmakeLists.txt for the new handler. This tutorial will provide templates for the header and implementation files and demonstrate how to add support in session.cc .


### Header File

new_handler.h


```
#ifndef NEWHANDLER_H
#define NEWHANDLER_H
#include <iostream>
#include <string>
#include <boost/beast/http.hpp>
#include "request_handler.h"

namespace http = boost::beast::http;

class NewHandler : public RequestHandler
{
 public:
   NewHandler(const std::string& location_path, const NginxConfig& config);
   http::response<http::string_body> handle_request(const http::request<http::string_body>& request);


};

#endif
```


Some important notes about a NewHandler header file:



*   Include the request_handler header file with  `#include "request_handler.h"`
*   Extend the RequestHandler class with  `: public RequestHandler`


### Implementation File

new_handler.cc


```
#include <sstream>
#include <string>
#include "request_handler.h"
#include "new_handler.h"
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

NewHandler::NewHandler(const std::string& location_path, const NginxConfig& config)
 : RequestHandler(location_path, config)
{}

http::response<http::string_body> NewHandler::handle_request(const http::request<http::string_body>& request)
{
 // Get version, body and data length from the request
 size_t version = request.version();

 /* Generate the response */
 http::response<http::string_body> res;

 // Complete necessary fields
 // examples of fields include:
 	// res.version(version);
 	// res.result(http::status::ok);
 	// res.set(http::field::content_type, "text/plain");
 	// res.set(http::field::content_length, content_length);
 	// res.body() = body;

 // Return properly formatted response
 return res;


}
```


Some important notes about a NewHandler implementation file



*   Include the request_handler header file with  `#include "request_handler.h"`
*   Include the new_handler header file with  `#include "new_handler.h"`
*   Implement the constructor by calling RequestHandler’s constructor in the initialization list 
*   In handle_request, return a response object with all necessary fields 


### Session.cc

In Session.cc, the NewHandler should be added to the createHandler function as follows: 


```
RequestHandler* createHandler(std::string location, std::string handler, NginxConfig config_child) {
 if (handler == "StaticHandler") {
   return new StaticHandler(location, config_child);
 }
 if (handler == "EchoHandler") {
   return new EchoHandler(location, config_child);
 }
 // add the following: 
 if (handler == "NewHandler") {
   return new NewHandler(location, config_child);
 }
 // Should a NotFoundHandler take any arguments?
 return new NotFoundHandler(location, config_child);
}
```



### CMakeLists.txt

In CMakeLists.txt, 

1. find the add_library command for session_lib and update it to include the new handler as follows:


```
add_library(session_lib src/session.cc src/request.cc src/request_handler.cc src/echo_handler.cc src/not_found_handler.cc src/static_handler.cc src/new_handler.cc)
```


2. Add a new add_library command for the new_handler as follows:


```
add_library(new_lib src/request_handler.cc src/new_handler.cc)
```


3. Include the new_lib in the target_link_libraries command for the webserver as follows:


```
target_link_libraries(webserver pthread server_lib request_lib session_lib static_lib echo_lib new_lib config_parser_lib logger_lib Boost::system Boost::log_setup Boost::log Boost::filesystem Boost::regex)
```


4. Include the new_lib in the target_link_libraries command for any test executable that uses the new_lib
