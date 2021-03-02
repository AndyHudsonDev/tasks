project(scheduler_monitoring_app)
cmake_minimum_required(VERSION 3.11)

set(Boost_USE_MULTITHREADED      ON)
find_package(Boost 1.75.0 COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Critical error: Boost library has not been found.")
endif()

add_definitions(-std=c++14)

add_subdirectory(scheduler)

