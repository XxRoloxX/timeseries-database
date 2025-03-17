# cmake_minimum_required(VERSION 3.10)
# project(TSDB)
#
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
# # Find all .cpp files inside src/ and store them in SRCS
# file(GLOB SRCS src/*.cpp)
#
# # Fetch Google Test (no need to install it)
# include(FetchContent)
#
# Set(FETCHCONTENT_QUIET FALSE) # Needed to print downloading progress
#
# # Create a library with all source files
# add_library(tsdb_lib STATIC ${SRCS})  # Change tsdb from executable to static library
#
# # # Create an executable with all source files
# # add_executable(tsdb ${SRCS})
#
# # Enable compile_commands.json for clangd
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#
# # Enable testing
# enable_testing()
#
#
# FetchContent_Declare(
#   googletest
#   URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
# )
#
# FetchContent_MakeAvailable(googletest)
#
# file(GLOB TSRCS tests/*.cpp)
#
# add_executable(tsdb_tests ${TSRCS})
# target_link_libraries(
#   tsdb_tests 
#   GTest::gtest_main
# )
#
# include(GoogleTest)
#
# # Include src/ directory headers for the test
# target_include_directories(tsdb_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
#
# # Register the test with CTest
# add_test(NAME TSDBTests COMMAND tsdb_tests)
#
# gtest_discover_tests(tsdb_tests)
#
cmake_minimum_required(VERSION 3.10)
project(TSDB)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find all .cpp files inside src/ and store them in SRCS
file(GLOB SRCS src/*.cpp)

# Fetch Google Test (no need to install it)
include(FetchContent)

Set(FETCHCONTENT_QUIET FALSE) # Needed to print downloading progress

# Create a library with all source files
add_library(tsdb_lib STATIC ${SRCS})  # Static library
set(CMAKE_BUILD_TYPE Debug)

# # Create an executable for the main program
add_executable(tsdb src/main.cpp)  # Assuming you have a main.cpp for the program entry point
target_link_libraries(tsdb tsdb_lib)  # Link the executable to the static library

# Enable compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable testing
enable_testing()

# FetchContent for GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

# Find all test source files
file(GLOB TSRCS tests/*.cpp)

# Create an executable for the tests
add_executable(tsdb_tests ${TSRCS})
target_link_libraries(
  tsdb_tests 
  GTest::gtest_main
  tsdb_lib  # Link the tests to the library as well
)

include(GoogleTest)

# Include src/ directory headers for the test
target_include_directories(tsdb_tests PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Register the test with CTest
add_test(NAME TSDBTests COMMAND tsdb_tests)

# Discover tests using GoogleTest
gtest_discover_tests(tsdb_tests)
