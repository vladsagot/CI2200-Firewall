cmake_minimum_required(VERSION 3.14)
project(tarea1_seguridad)

set(CMAKE_CXX_STANDARD 14)

add_executable(tarea1_seguridad main.cpp Store.cpp Store.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/include -I/usr/include/cppconn -Wl,-Bdynamic")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/lib -lmysqlcppconn -static-libstdc++")

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "store.cgi")
