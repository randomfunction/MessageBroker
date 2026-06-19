cmake_minimum_required(VERSION 3.10)
project(MessageBroker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_library(messagebroker messagebroker.cpp)
target_include_directories(messagebroker PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(main_test main_test.cpp)
target_link_libraries(main_test PRIVATE messagebroker Threads::Threads)

add_executable(LatencyTest LatencyTest.cpp)
target_link_libraries(LatencyTest PRIVATE messagebroker Threads::Threads)
