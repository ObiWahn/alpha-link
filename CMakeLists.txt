cmake_minimum_required(VERSION 3.31)
project(alpha-link CXX)

option(CODE_COVERAGE "Enable code coverage flags" OFF)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    if(CODE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Building with code coverage enabled")
        add_compile_options(--coverage)
        add_link_options(--coverage)
    endif()
endif()

add_library(alink
    ${CMAKE_CURRENT_LIST_DIR}/src/common.cpp
)
set_property(TARGET alink PROPERTY CMAKE_CXX_STANDARD 20)

target_include_directories(alink PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

if(BUILD_TESTING AND (ALINK_BUILD_TESTING OR (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)))
    include(CTest)
    add_subdirectory(tests)
endif()
