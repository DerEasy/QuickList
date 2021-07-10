cmake_minimum_required(VERSION 3.20)
project(QuickList)

set(CMAKE_CXX_STANDARD 20)

add_executable(QuickList quicklist.cpp baselist.cpp jumplist.cpp node.cpp test.cpp)
