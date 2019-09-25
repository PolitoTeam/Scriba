cmake_minimum_required(VERSION 3.13)
project(Esercitazione_3)

set(CMAKE_CXX_STANDARD 14)

add_executable(Esercitazione_3 main.cpp NetworkServer.cpp NetworkServer.h Symbol.cpp Symbol.h SharedEditor.cpp SharedEditor.h Message.cpp Message.h)