cmake_minimum_required(VERSION 3.1.0)
project(1a_cas)

set(CMAKE_CXX_STANDARD 17)
add_executable(1a_cas main.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(1a_cas PRIVATE Threads::Threads)