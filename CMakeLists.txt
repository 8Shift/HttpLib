cmake_minimum_required(VERSION 3.28)

project(HttpPacketTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(HttpPacketTest
        main.cpp
        httpdata.h
        httpdata.h
        httpdata.cpp
)

target_link_libraries(HttpPacketTest wsock32 ws2_32)