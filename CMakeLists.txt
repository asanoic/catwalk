cmake_minimum_required(VERSION 3.13)
project(catwalk LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE OFF)

if (NOT Boost_ROOT)
    if (MINGW)
        set(Boost_ROOT "q:/msys2/lib/boost-1.76.0/stage/lib/cmake/Boost-1.76.0")
    else()
        set(Boost_ROOT "/opt/boost/stage/lib/cmake/Boost-1.76.0")
    endif()
endif()

set (pwd ${CMAKE_CURRENT_LIST_DIR})

function(add_files files)
    set(local-list ${${files}})
    foreach(file IN LISTS ARGN)
        list(APPEND local-list
            "${pwd}/include/${file}.h"
            "${pwd}/src/${file}Data.h"
            "${pwd}/src/${file}.cpp"
        )
    endforeach()
    set(${files} ${local-list} PARENT_SCOPE)
endfunction()

find_package(Boost 1.75 REQUIRED COMPONENTS system)

list(APPEND engine-src
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

list(APPEND catwalk-src "${pwd}/include/CwCatwalk.h")
add_files(catwalk-src
    "CwObject"
    "CwRequest"
    "CwResponse"
    "CwRouter"
    "CwApplication"
    "CwStaticMiddleware"
)

add_library(${PROJECT_NAME} SHARED ${engine-src} ${catwalk-src})

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
