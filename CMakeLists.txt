cmake_minimum_required(VERSION 3.0)
project(ThreadPool)
set(CMAKE_CXX_STANDARD 11)
include_directories(src)
enable_testing()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(TestThreadPool test/main.cpp src/ThreadPool.h)
target_link_libraries(TestThreadPool PUBLIC Threads::Threads)
add_test(NAME TestThreadPool COMMAND TestThreadPool)
