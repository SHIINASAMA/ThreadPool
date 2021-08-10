cmake_minimum_required(VERSION 3.0)
project(threadPool)
set(CMAKE_CXX_STANDARD 17)
include_directories(src)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(testThreadPool test/main.cpp src/threadPool.h)
target_link_libraries(testThreadPool PUBLIC Threads::Threads)
