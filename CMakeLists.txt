cmake_minimum_required(VERSION 3.13)
project(catwalk LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

if (NOT Boost_ROOT)
    set(Boost_ROOT "Q:/msys2/lib/boost-1.74.0/stage/lib/cmake/Boost-1.74.0")
endif()

set (pwd ${CMAKE_CURRENT_LIST_DIR})

find_package(Boost 1.70 REQUIRED COMPONENTS system)

list(APPEND beast-src
    "${pwd}/src/beast/main.cpp"
    "${pwd}/src/beast/listener.h"
    "${pwd}/src/beast/listener.cpp"
    "${pwd}/src/beast/utils.h"
    "${pwd}/src/beast/utils.cpp"
    "${pwd}/src/beast/http-session.h"
    "${pwd}/src/beast/http-session.cpp"
    "${pwd}/src/beast/websocket-session.h"
    "${pwd}/src/beast/websocket-session.cpp"
)

add_executable(${CMAKE_PROJECT_NAME} ${beast-src})

target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_17)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wa,-mbig-obj -Og)
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC Boost::system -lws2_32 -lmswsock)
