cmake_minimum_required(VERSION 3.14)
project(tarea1_seguridad)

set(CMAKE_CXX_STANDARD 14)

add_executable(tarea1_seguridad main.cpp)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include/mysql-cppconn-8 -L/usr/lib -lmysqlcppconn")