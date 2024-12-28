cmake_minimum_required(VERSION 3.10)
project(DeribitTrading)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/libs/websocketpp)

file(GLOB SOURCES "src/*.cpp")

add_executable(DeribitTrading ${SOURCES})

find_package(CURL REQUIRED)
target_link_libraries(DeribitTrading PRIVATE CURL::libcurl)

find_package(Boost REQUIRED COMPONENTS system)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(DeribitTrading PRIVATE ${Boost_LIBRARIES})
endif()
find_package(OpenSSL REQUIRED)
target_link_libraries(DeribitTrading PRIVATE OpenSSL::SSL OpenSSL::Crypto)
