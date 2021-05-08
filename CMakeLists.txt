cmake_minimum_required(VERSION 3.10.0)
project(team-juzang)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)             # TODO: not sure if log_setup is supposed to be included here
find_package(Boost 1.50 REQUIRED COMPONENTS log_setup log system filesystem regex)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(${CMAKE_SOURCE_DIR}/include)

# TODO(!): Update name and srcs
add_library(config_parser_lib src/config_parser.cc)
add_library(server_lib src/server.cc)
add_library(session_lib src/session.cc src/request.cc src/echo_handler.cc src/not_found_handler.cc src/static_handler.cc)
add_library(echo_lib src/echo_handler.cc)
add_library(request_lib src/request.cc)
add_library(logger_lib src/logger.cc)
add_library(static_lib src/request.cc src/not_found_handler.cc src/static_handler.cc)

# TODO(!): Update executable name, srcs, and deps
add_executable(webserver src/_main.cc)     
target_link_libraries(webserver pthread server_lib request_lib session_lib static_lib config_parser_lib logger_lib
                            Boost::system Boost::log_setup Boost::log Boost::filesystem Boost::regex)

# TODO(!): Update test executable name, srcs, and deps
#add_executable(config_parser_test tests/unit/config_parser_test.cc)
#target_link_libraries(config_parser_test config_parser_lib gtest_main logger_lib
                    #Boost::system Boost::log_setup Boost::log Boost::filesystem Boost::regex)

#add_executable(echo_response_test tests/unit/echo_response_test.cc)
#target_link_libraries(echo_response_test echo_lib gtest_main)

#add_executable(request_test tests/unit/request_test.cc)
#target_link_libraries(request_test request_lib gtest_main)

#add_executable(session_test tests/unit/session_test.cc)
#target_link_libraries(session_test session_lib gtest_main logger_lib
                    #Boost::system Boost::log_setup Boost::log Boost::filesystem Boost::regex)


#add_executable(server_test tests/unit/server_test.cc)                   
#target_link_libraries(server_test config_parser_lib server_lib  static_lib session_lib gtest_main logger_lib
                    #Boost::system Boost::log_setup Boost::log Boost::filesystem Boost::regex)
		    
#add_executable(static_test tests/unit/static_test.cc)
#target_link_libraries(static_test static_lib gtest_main)


# TODO(!): Update with test binary
#gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
#gtest_discover_tests(echo_response_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
#gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
#gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
#gtest_discover_tests(request_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
#gtest_discover_tests(static_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

#add_test(NAME IntegrationTest 
   # COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration/integration.sh 
   # WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration)

# TODO(!): Update with target/test targets
#include(cmake/CodeCoverageReportConfig.cmake)   
#generate_coverage_report(TARGETS webserver server_lib request_lib  static_lib session_lib logger_lib TESTS config_parser_test request_test session_test)  
#generate_coverage_report(TARGETS webserver server_lib request_lib  static_lib session_lib logger_lib TESTS config_parser_test static_test echo_response_test request_test session_test server_test)
