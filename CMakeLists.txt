cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 11)

project(mcrxtx)

add_executable(mcsend mcsend.cpp)

add_executable(mcrecv mcreceive.cpp)
