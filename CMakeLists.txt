cmake_minimum_required(VERSION 3.20)
project(webserver)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserver main.cpp server.cpp Request.cpp Response.cpp socket.cpp )
