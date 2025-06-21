cmake_minimum_required(VERSION 3.31)
project(alpha-link CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(alink
    ${CMAKE_CURRENT_LIST_DIR}/src/messages_test.cpp
)
set_property(TARGET alink PROPERTY CMAKE_CXX_STANDARD 20)

target_include_directories(alink PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

if(BUILD_TESTING AND (ALINK_BUILD_TESTING OR (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)))
    include(CTest)
    add_subdirectory(tests)
endif()
