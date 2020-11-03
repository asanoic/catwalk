cmake_minimum_required(VERSION 3.13)
project(catwalk LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE OFF)

if (NOT Boost_ROOT)
    if (MINGW)
        set(Boost_ROOT "q:/msys2/lib/boost-1.74.0/stage/lib/cmake/Boost-1.74.0")
    else()
        set(Boost_ROOT "/opt/boost/stage/lib/cmake/Boost-1.74.0")
    endif()
endif()

set (pwd ${CMAKE_CURRENT_LIST_DIR})

find_package(Boost 1.73 REQUIRED COMPONENTS system)

list(APPEND beast-src
    "${pwd}/src/beast/boost-headers.h"
    "${pwd}/src/beast/main.cpp"
    "${pwd}/src/beast/listener.h"
    "${pwd}/src/beast/listener.cpp"
    "${pwd}/src/beast/utils.h"
    "${pwd}/src/beast/utils.cpp"
    "${pwd}/src/beast/http-session.h"
    "${pwd}/src/beast/http-session.cpp"
    "${pwd}/src/beast/websocket-session.h"
    "${pwd}/src/beast/websocket-session.cpp"
    "${pwd}/src/api/catwalk.h"
    "${pwd}/src/api/router.cpp"
    "${pwd}/src/lib/path.h"
    "${pwd}/src/lib/path.cpp"
)

add_executable(${CMAKE_PROJECT_NAME} ${beast-src})

target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC BOOST_ASIO_NO_DEPRECATED BOOST_BEAST_USE_STD_STRING_VIEW)

target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_17)

if(MINGW)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wa,-mbig-obj -Og)
    endif()
    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC -lws2_32 -lmswsock)
else()
    target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC -lpthread)
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC Boost::system)
