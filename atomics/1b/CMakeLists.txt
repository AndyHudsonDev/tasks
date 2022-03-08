cmake_minimum_required(VERSION 3.1.0)
project(Mill1b)

set(CMAKE_CXX_STANDARD 17)
add_executable(Mill1b main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(Mill1b PRIVATE Threads::Threads)
