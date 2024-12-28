cmake_minimum_required(VERSION 3.10)
project(DeribitTrading)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(DeribitTrading ${SOURCES})

find_package(CURL REQUIRED)
target_link_libraries(DeribitTrading PRIVATE CURL::libcurl)
