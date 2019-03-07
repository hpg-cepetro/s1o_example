cmake_minimum_required(VERSION 2.6)

project(s1o_example)

include_directories("${PROJECT_SOURCE_DIR}/include")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

add_subdirectory(src)
