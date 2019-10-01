cmake_minimum_required(VERSION 3.13)
project(lab_03)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab_03 main.cpp NetworkServer.cpp NetworkServer.h SharedEditor.h Symbol.h Message.h)
