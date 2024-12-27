cmake_minimum_required(VERSION 3.10)
project(DeribitTrading)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(include)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Add executable
add_executable(DeribitTrading ${SOURCES})

# Link required libraries
find_package(CURL REQUIRED)
target_link_libraries(DeribitTrading PRIVATE CURL::libcurl)
