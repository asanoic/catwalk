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
    "${pwd}/include/CwCatwalk.h"
    "${pwd}/include/CwObject.h"
    "${pwd}/src/CwObjectData.h"
    "${pwd}/src/CwObject.cpp"
    "${pwd}/include/CwRequest.h"
    "${pwd}/src/CwRequestData.h"
    "${pwd}/src/CwRequest.cpp"
    "${pwd}/include/CwResponse.h"
    "${pwd}/src/CwResponseData.h"
    "${pwd}/src/CwResponse.cpp"
    "${pwd}/include/CwRouter.h"
    "${pwd}/src/CwRouterData.h"
    "${pwd}/src/CwRouter.cpp"
    "${pwd}/include/CwApplication.h"
    "${pwd}/src/CwApplicationData.h"
    "${pwd}/src/CwApplication.cpp"
    "${pwd}/src/engine/boost-headers.h"
    "${pwd}/src/engine/listener.h"
    "${pwd}/src/engine/listener.cpp"
    "${pwd}/src/engine/utils.h"
    "${pwd}/src/engine/utils.cpp"
    "${pwd}/src/engine/http-session.h"
    "${pwd}/src/engine/http-session.cpp"
    "${pwd}/src/engine/websocket-session.h"
    "${pwd}/src/engine/websocket-session.cpp"
)

add_library(${PROJECT_NAME} SHARED ${beast-src})

target_compile_definitions(${PROJECT_NAME} PUBLIC BOOST_ASIO_NO_DEPRECATED BOOST_BEAST_USE_STD_STRING_VIEW)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC "${pwd}/include" PRIVATE "${pwd}/src" "${pwd}/src/engine")
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::system)

if(MINGW)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${PROJECT_NAME} PRIVATE -Wa,-mbig-obj -Og)
    endif()
    target_link_libraries(${PROJECT_NAME} PUBLIC -lws2_32 -lmswsock)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC -lpthread)
endif()

add_executable(demo "${pwd}/demo/main.cpp")
target_link_libraries(demo PRIVATE ${PROJECT_NAME})

add_custom_target(other-files SOURCES
    "${pwd}/README.md"
)



